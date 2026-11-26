#include "raylib.h"
#include "../include/pong.h"

int main(void) {
	InitWindow(SCREENWIDTH, SCREENHEIGHT, "PONG version: beta 0.6.1 ");

	//Field
	Field field = createField(colorBackground, lineColor, lineThickness, top, bottom, leftGoal, rightGoal);

	//Bars
	Player player1 = createPlayer(WHITE, PLAYERHEIGHT, PLAYERWIDTH, rectangleLeft);
	Player player2 = createPlayer(BLUE, PLAYERHEIGHT, PLAYERWIDTH, rectangleRight);

	//Ball``
	Ball ball = createBall(ballColor, RATIO, ballSpeed, ballPosition);

	//Computer opponent, it plays player 2 in one player mode
	Computer computer = createComputer();
	int onePlayer = 1;

	// Set our game to run at 60 frames-per-second
	SetTargetFPS(60);

	// Main game loop
	// Detect window close button or ESC key
	GameState state = MENU;
	int framesCounter = 0;
	while (!WindowShouldClose()) {
		framesCounter++;

		// Update
		switch (state) {
		case MENU:
			if (IsKeyPressed(KEY_ONE)) {
				onePlayer = 1;
				state = PLAYING;
			} else if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_M)) {
				onePlayer = 0;
				state = PLAYING;
			}
			break;
		case PLAYING:
			if (IsKeyPressed(KEY_SPACE)) {
				state = PAUSED;
				break;
			}

			// Movement player 1, alone against the computer the arrows work too
			if (IsKeyDown(KEY_W) || (onePlayer && IsKeyDown(KEY_UP))) player1->bar.y -= 15.0f;
			else if (IsKeyDown(KEY_S) || (onePlayer && IsKeyDown(KEY_DOWN))) player1->bar.y += 15.0f;

			// Movement player 2
			if (onePlayer) computerUpdate(computer, player2, player1, ball);
			else if (IsKeyDown(KEY_UP)) player2->bar.y -= 15.0f;
			else if (IsKeyDown(KEY_DOWN)) player2->bar.y += 15.0f;

			// Check when scores and reset the match
			state = referee(player1, player2, ball, field, maxScore, totalMatches);
			break;
		case PAUSED:
			if (IsKeyPressed(KEY_SPACE)) state = PLAYING;
			else if (IsKeyPressed(KEY_M)) {
				resetGame(player1, player2);
				state = MENU;
			}
			break;
		case MATCH_OVER:
			if (IsKeyPressed(KEY_SPACE)) state = PLAYING;
			break;
		case GAME_OVER:
			if (IsKeyPressed(KEY_ENTER)) {
				resetGame(player1, player2);
				state = PLAYING;
			} else if (IsKeyPressed(KEY_M)) {
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

			// On pause, we draw a blinking message
			if (state == PAUSED && ((framesCounter/30)%2)) DrawText("PAUSED", SCREENWIDTH/2 - 70, SCREENHEIGHT/3, 30, GRAY);

			// Between matches, we draw a blinking message to continue
			if (state == MATCH_OVER && ((framesCounter/30)%2)) DrawText("Press SPACE to CONTINUE.", SCREENWIDTH/2 - 260, SCREENHEIGHT/2, 30, GREEN);
		}

		//Draw the front menu
		if (state == MENU) {
			DrawText("Pong ", SCREENWIDTH/2 - 210, SCREENHEIGHT/3, 180, RED);
			DrawText("Pong ", SCREENWIDTH/2 - 213, SCREENHEIGHT/3, 178, BLUE);
			DrawText("titoreboot inc.", SCREENWIDTH/2 - 25, 3*SCREENHEIGHT/4, 10, WHITE);
			DrawText("press 1 for ONE PLAYER, 2 or ENTER for TWO PLAYERS.", SCREENWIDTH/2 - 190, 5*SCREENHEIGHT/6, 15, WHITE);
			DrawText("press SPACE to PAUSE.", SCREENWIDTH/2 - 77, 7*SCREENHEIGHT/8, 15, WHITE);
		}//Draw victory player 1
		else if (state == GAME_OVER && player1->matchScore >= totalMatches) {
			DrawText("Player 1 WINS", SCREENWIDTH/3 - 200, SCREENHEIGHT/2, 30, player1->color);
			DrawText("Press ENTER for REMATCH   Press ESC to EXIT", SCREENWIDTH/2 - 305, 3*SCREENHEIGHT/4, 20, WHITE);
			DrawText("Press M to go MAIN MENU", SCREENWIDTH/2 - 305, 6*SCREENHEIGHT/7, 20, WHITE);
		}//Draw victory player 2
		else if (state == GAME_OVER) {
			DrawText("Press ENTER for REMATCH   Press ESC to EXIT", SCREENWIDTH/2 - 305, 3*SCREENHEIGHT/4, 20, WHITE);
			DrawText(onePlayer ? "Computer WINS" : "Player 2 WINS", SCREENWIDTH/2 + 100, SCREENHEIGHT/2, 30, player2->color);
			DrawText("Press M to go MAIN MENU", SCREENWIDTH/2 - 305, 6*SCREENHEIGHT/7, 20, WHITE);
		}

		//FPS
		DrawFPS(10, 10);

		EndDrawing();
	}

	free(computer);
	CloseWindow();
	return 0;
}
