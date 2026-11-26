#include "raylib.h"
#include "../include/pong.h"

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
	// A browser tab cannot be closed by the game, so there is no exit hint
	#define REMATCH_HINT "Press ENTER for REMATCH"
#else
	#define REMATCH_HINT "Press ENTER for REMATCH   Press ESC to EXIT"
#endif

// Game objects shared by every frame
static Field field;
static Player player1;
static Player player2;
static Ball ball;
// Computer opponent, it plays player 2 in one player mode
static Computer computer;
static int onePlayer = 1;
static MenuOption menuOption = MENU_ONE_PLAYER;

// Main menu labels and where each option is drawn (also its mouse click area)
static const char *menuLabels[MENU_OPTIONS] = { "ONE PLAYER", "TWO PLAYERS" };
static Rectangle menuOptionRect(int option) {
	return (Rectangle){ SCREENWIDTH/2 - 110, 355 + option*30, 220, 26 };
}
static GameState state = MENU;
static int framesCounter = 0;

// Touch input: controls are drawn only once the player has touched the screen
static int touchSeen = 0;
static int lastTouchCount = 0;

// Draw the touch controls for player 1, brighter while pressed
static void drawTouchControls(int upHeld, int downHeld, int paused) {
	Rectangle up = touchControlRect(TOUCH_UP);
	Rectangle down = touchControlRect(TOUCH_DOWN);
	Rectangle pause = touchControlRect(TOUCH_PAUSE);
	float cx = up.x + up.width/2;
	float cy = up.y + up.height/2;
	DrawTriangle((Vector2){ cx, cy - 35 }, (Vector2){ cx - 40, cy + 25 }, (Vector2){ cx + 40, cy + 25 }, Fade(WHITE, upHeld ? 0.45f : 0.15f));
	cy = down.y + down.height/2;
	DrawTriangle((Vector2){ cx - 40, cy - 25 }, (Vector2){ cx, cy + 35 }, (Vector2){ cx + 40, cy - 25 }, Fade(WHITE, downHeld ? 0.45f : 0.15f));
	DrawRectangleRec(pause, BLACK);
	DrawRectangleLinesEx(pause, 2, Fade(WHITE, 0.3f));
	if (paused) {
		float px = pause.x + pause.width/2;
		float py = pause.y + pause.height/2;
		DrawTriangle((Vector2){ px - 10, py - 15 }, (Vector2){ px - 10, py + 15 }, (Vector2){ px + 15, py }, Fade(WHITE, 0.6f));
	} else {
		DrawRectangle(pause.x + 24, pause.y + 15, 8, 30, Fade(WHITE, 0.6f));
		DrawRectangle(pause.x + 38, pause.y + 15, 8, 30, Fade(WHITE, 0.6f));
	}
}

// Update and draw one frame
static void UpdateDrawFrame(void);

int main(void) {
	InitWindow(SCREENWIDTH, SCREENHEIGHT, "PONG version: beta 0.6.1 ");

	//Field
	field = createField(colorBackground, lineColor, lineThickness, top, bottom, leftGoal, rightGoal);

	//Bars
	player1 = createPlayer(WHITE, PLAYERHEIGHT, PLAYERWIDTH, rectangleLeft);
	player2 = createPlayer(BLUE, PLAYERHEIGHT, PLAYERWIDTH, rectangleRight);

	//Ball
	ball = createBall(ballColor, RATIO, ballSpeed, ballPosition);

	//Computer opponent
	computer = createComputer();

#if defined(PLATFORM_WEB)
	// The browser drives the loop at 60 frames-per-second; ESC must not close the page
	SetExitKey(KEY_NULL);
	emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
	// Set our game to run at 60 frames-per-second
	SetTargetFPS(60);

	// Main game loop
	// Detect window close button or ESC key
	while (!WindowShouldClose()) {
		UpdateDrawFrame();
	}
#endif

	CloseWindow();
	return 0;
}

static void UpdateDrawFrame(void) {
	framesCounter++;

	// Touches held this frame, and a tap when a new finger lands (raylib gives
	// touch positions in game coordinates, even when the page scales the canvas)
	int touchCount = GetTouchPointCount();
	int tapped = touchCount > lastTouchCount;
	Vector2 tap = tapped ? GetTouchPosition(touchCount - 1) : (Vector2){ -1, -1 };
	lastTouchCount = touchCount;
	if (touchCount > 0) touchSeen = 1;
	int touchUp = 0, touchDown = 0;
	for (int i = 0; i < touchCount; i++) {
		TouchControl control = touchControlAt(GetTouchPosition(i));
		if (control == TOUCH_UP) touchUp = 1;
		else if (control == TOUCH_DOWN) touchDown = 1;
	}
	int tapPause = tapped && touchControlAt(tap) == TOUCH_PAUSE;

	// Update
	switch (state) {
	case MENU:
		{
			// Choose with the arrows, W/S or the mouse; start with Enter, M or a click.
			// 1 and 2 (also on the numpad) start that option right away.
			int start = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_M);
			if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) menuOption = menuSelect(menuOption, -1);
			if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) menuOption = menuSelect(menuOption, 1);
			if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
				menuOption = MENU_ONE_PLAYER;
				start = 1;
			} else if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
				menuOption = MENU_TWO_PLAYERS;
				start = 1;
			}
			for (int option = 0; option < MENU_OPTIONS; option++) {
				// Phones: tapping an option starts it (taps never become mouse clicks)
				if (tapped && CheckCollisionPointRec(tap, menuOptionRect(option))) {
					menuOption = (MenuOption)option;
					start = 1;
				}
				if (!CheckCollisionPointRec(GetMousePosition(), menuOptionRect(option))) continue;
				if (GetMouseDelta().x != 0 || GetMouseDelta().y != 0) menuOption = (MenuOption)option;
				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					menuOption = (MenuOption)option;
					start = 1;
				}
			}
			if (start) {
				onePlayer = menuOption == MENU_ONE_PLAYER;
				state = PLAYING;
			}
		}
		break;
	case PLAYING:
		if (IsKeyPressed(KEY_SPACE) || tapPause) {
			state = PAUSED;
			break;
		}

		// Movement player 1, alone against the computer the arrows and the touch controls work too
		if (IsKeyDown(KEY_W) || (onePlayer && (IsKeyDown(KEY_UP) || touchUp))) player1->bar.y -= 15.0f;
		else if (IsKeyDown(KEY_S) || (onePlayer && (IsKeyDown(KEY_DOWN) || touchDown))) player1->bar.y += 15.0f;

		// Movement player 2
		if (onePlayer) computerUpdate(computer, player2, player1, ball);
		else if (IsKeyDown(KEY_UP)) player2->bar.y -= 15.0f;
		else if (IsKeyDown(KEY_DOWN)) player2->bar.y += 15.0f;

		// Check when scores and reset the match
		state = referee(player1, player2, ball, field, maxScore, totalMatches);
		break;
	case PAUSED:
		if (IsKeyPressed(KEY_SPACE) || tapped) state = PLAYING;
		else if (IsKeyPressed(KEY_M)) {
			resetGame(player1, player2);
			state = MENU;
		}
		break;
	case MATCH_OVER:
		if (IsKeyPressed(KEY_SPACE) || tapped) state = PLAYING;
		break;
	case GAME_OVER:
		if (IsKeyPressed(KEY_ENTER)) {
			resetGame(player1, player2);
			state = PLAYING;
		} else if (IsKeyPressed(KEY_M) || tapped) {
			resetGame(player1, player2);
			state = MENU;
		}
		break;
	}

	BeginDrawing();
	ClearBackground(field->colorBackground);
	if (state == PLAYING || state == PAUSED || state == MATCH_OVER) {
		//Draw the Field
		DrawLineEx(startPos, endPos, field->lineThickness, field->colorLine);

		//Draw the Ball
		DrawCircleV(ball->position, ball->ratio, ball->color);

		//Draw the Players
		DrawRectangleRec(player1->bar, player1->color);
		DrawRectangleRec(player2->bar, player2->color);

		//Draw players Scores
		DrawText(TextFormat("Score Player 1: %i", player1->score), SCREENWIDTH/30, 25, 20, player1->color);
		DrawText(TextFormat("Score %s: %i", onePlayer ? "Computer" : "Player 2", player2->score), 3*SCREENWIDTH/4, 25, 20, player2->color);
		DrawText(TextFormat("Matches: %i / %i", player1->matchScore, totalMatches), SCREENWIDTH/30, 50, 20, player1->color);
		DrawText(TextFormat("Matches: %i / %i", player2->matchScore, totalMatches), 3*SCREENWIDTH/4, 50, 20, player2->color);
		if (onePlayer) DrawText(TextFormat("Level: %i / %i", computer->level, MAXLEVEL), 3*SCREENWIDTH/4, 75, 20, player2->color);

		// Touch controls, only for a one player game on a touch screen
		if (touchSeen && onePlayer) drawTouchControls(touchUp, touchDown, state != PLAYING);

		// On pause, we draw a blinking message
		if (state == PAUSED && ((framesCounter/30)%2)) DrawText("PAUSED", SCREENWIDTH/2 - 70, SCREENHEIGHT/3, 30, GRAY);
		if (state == PAUSED && touchSeen) DrawText("Tap to CONTINUE.", SCREENWIDTH/2 - MeasureText("Tap to CONTINUE.", 20)/2, SCREENHEIGHT/3 + 45, 20, GRAY);

		// Between matches, we draw a blinking message to continue
		const char *next = touchSeen ? "Tap to CONTINUE." : "Press SPACE to CONTINUE.";
		if (state == MATCH_OVER && ((framesCounter/30)%2)) DrawText(next, SCREENWIDTH/2 - MeasureText(next, 30)/2, SCREENHEIGHT/2, 30, GREEN);
	}

	//Draw the front menu
	if (state == MENU) {
		DrawText("Pong ", SCREENWIDTH/2 - 210, SCREENHEIGHT/3, 180, RED);
		DrawText("Pong ", SCREENWIDTH/2 - 213, SCREENHEIGHT/3, 178, BLUE);
		DrawText("titoreboot inc.", SCREENWIDTH/2 - 25, 3*SCREENHEIGHT/4, 10, WHITE);
		for (int option = 0; option < MENU_OPTIONS; option++) {
			Rectangle area = menuOptionRect(option);
			int selected = option == (int)menuOption;
			const char *label = selected ? TextFormat("> %s <", menuLabels[option]) : menuLabels[option];
			DrawText(label, area.x + area.width/2 - MeasureText(label, 20)/2, area.y + 3, 20, selected ? YELLOW : GRAY);
		}
		const char *hint = touchSeen ? "TAP an option to start." : "UP/DOWN to choose, ENTER to start, SPACE to PAUSE in game.";
		DrawText(hint, SCREENWIDTH/2 - MeasureText(hint, 15)/2, 422, 15, WHITE);
	}//Draw victory player 1
	else if (state == GAME_OVER && player1->matchScore >= totalMatches) {
		DrawText("Player 1 WINS", SCREENWIDTH/3 - 200, SCREENHEIGHT/2, 30, player1->color);
	}//Draw victory player 2
	else if (state == GAME_OVER) {
		DrawText(onePlayer ? "Computer WINS" : "Player 2 WINS", SCREENWIDTH/2 + 100, SCREENHEIGHT/2, 30, player2->color);
	}
	if (state == GAME_OVER && touchSeen) {
		DrawText("Tap to go MAIN MENU", SCREENWIDTH/2 - 305, 3*SCREENHEIGHT/4, 20, WHITE);
	} else if (state == GAME_OVER) {
		DrawText(REMATCH_HINT, SCREENWIDTH/2 - 305, 3*SCREENHEIGHT/4, 20, WHITE);
		DrawText("Press M to go MAIN MENU", SCREENWIDTH/2 - 305, 6*SCREENHEIGHT/7, 20, WHITE);
	}

	//FPS
	DrawFPS(10, 10);

	EndDrawing();
}
