#include "raylib.h"
#include "../include/pong.h"
#include <math.h>

Player createPlayer(
	Color color,
	int height,
	int width,
	Rectangle bar
) {
	Player p = (Player)malloc(sizeof(struct sPlayer));
	p->color = color;
	p->height = height;
	p->width = width;
	p->bar = bar;
	p->score = 0;
	p->matchScore = 0;
	return p;
}

Ball createBall(
	Color color,
	int ratio,
	Vector2 speed,
	Vector2 position
) {
	Ball b = (Ball)malloc(sizeof(struct sBall));
	b->color = color;
	b->ratio = ratio;
	b->speed = speed;
	b->position = position;
	return b;
}

Field createField(
	Color colorBackground,
	Color colorLine,
	int lineThickness,
	int top, int bottom,
	int leftGoal,
	int rightGoal
) {
	Field f = (Field)malloc(sizeof(struct sField));
	f->colorBackground = colorBackground;
	f->colorLine = colorLine;
	f->lineThickness = lineThickness;
	f->top = top;
	f->bottom = bottom;
	f->leftGoal = leftGoal;
	f->rightGoal = rightGoal;
	return f;
}

void resetGame(
	Player player1,
	Player player2
) {
	player1->score = 0;
	player2->score = 0;
	player1->matchScore = 0;
	player2->matchScore = 0;
}

//manage the scores of matches
Ball ballReset(Ball ball) {
	if ((ball->position.x > SCREENWIDTH + 2*RATIO )) { 
		ball->speed.x =  - ballSpeed.x;
		ball->speed.y =  - ballSpeed.y;
		ball->position.x = SCREENWIDTH/2 - ball->speed.x; 
		ball->position.y = SCREENHEIGHT/2 + ball->speed.y; 
	} else if ((ball->position.x < - 2*RATIO)) { 
		ball->speed.x = ballSpeed.x;
		ball->speed.y = ballSpeed.y;
		ball->position.x = SCREENWIDTH/2 + ball->speed.x; 
		ball->position.y = SCREENHEIGHT/2 - ball->speed.y;
	}
	return ball;
}

// Award a match to whoever reached maxScore and return the resulting state
GameState scoreManager(
	Player player1,
	Player player2,
	int maxScore,
	int totalMatches
) {
	Player winner = NULL;
	if (player1->score >= maxScore) winner = player1;
	else if (player2->score >= maxScore) winner = player2;
	if (winner == NULL) return PLAYING;

	winner->matchScore += 1;
	player1->score = 0;
	player2->score = 0;
	return winner->matchScore >= totalMatches ? GAME_OVER : MATCH_OVER;
}

// Keep each speed component within the given maximum magnitude
Vector2 clampSpeed(Vector2 speed, Vector2 max) {
	if (fabsf(speed.x) > max.x) speed.x = copysignf(max.x, speed.x);
	if (fabsf(speed.y) > max.y) speed.y = copysignf(max.y, speed.y);
	return speed;
}

// Send the ball back with a random pitch, capped at maxSpeed
void paddleBounce(Ball ball) {
	float randomPitchReal = GetRandomValue(4, 7)*.1f + 0.5f;
	ball->speed.x *= -randomPitchReal;
	ball->speed.y *= randomPitchReal;
	ball->speed = clampSpeed(ball->speed, maxSpeed);
}

// Movement of the ball
void ballMovement(Ball ball) {
	ball->position.x += ball->speed.x;
	ball->position.y += ball->speed.y;
}

void eyeBall(
	Player player1,
	Player player2,
	Ball ball
) {
	if ((ball->position.x > SCREENWIDTH + 2*RATIO )) { 
		ball = ballReset(ball);
		player1->score += 1;
	} else if ((ball->position.x < - 2*RATIO)) { 
		ball = ballReset(ball);
		player2->score += 1;
	}
}	

void collisions(
	Player player1,
	Player player2,
	Ball ball,
	Field field
) {
	// Bounce off roof and floor, keeping the ball inside the field
	if (ball->position.y >= SCREENHEIGHT - ball->ratio) {
		ball->position.y = SCREENHEIGHT - ball->ratio;
		ball->speed.y = -fabsf(ball->speed.y);
	} else if (ball->position.y <= ball->ratio) {
		ball->position.y = ball->ratio;
		ball->speed.y = fabsf(ball->speed.y);
	}

	// Bounce off a paddle only while moving towards it and still in front of it,
	// then push the ball out so it cannot bounce again on the next frame
	if (ball->speed.x < 0
		&& ball->position.x > player1->bar.x + player1->bar.width/2
		&& CheckCollisionCircleRec(ball->position, ball->ratio, player1->bar)) {
		ball->position.x = player1->bar.x + player1->bar.width + ball->ratio;
		paddleBounce(ball);
	} else if (ball->speed.x > 0
		&& ball->position.x < player2->bar.x + player2->bar.width/2
		&& CheckCollisionCircleRec(ball->position, ball->ratio, player2->bar)) {
		ball->position.x = player2->bar.x - ball->ratio;
		paddleBounce(ball);
	}

	// Checks collisions between BARS, LIMIT SUP and INF
	if (player1->bar.y <=  field->top) player1->bar.y = field->top;
	else if (player1->bar.y >= field->bottom) player1->bar.y = field->bottom;
	if (player2->bar.y <=  field->top) player2->bar.y = field->top;
	else if (player2->bar.y >= field->bottom) player2->bar.y = field->bottom;		
}

GameState referee(
	Player player1,
	Player player2,
	Ball ball,
	Field field,
	int maxScore,
	int totalMatches
) {	
	eyeBall(player1, player2, ball);
	GameState state = scoreManager(player1, player2, maxScore, totalMatches);
	collisions(player1, player2, ball, field);
	ballMovement(ball);
	return state;
}

int main(void) {
	InitWindow(SCREENWIDTH, SCREENHEIGHT, "PONG version: beta 0.6.1 ");

	//Field
	Field field = createField(colorBackground, lineColor, lineThickness, top, bottom, leftGoal, rightGoal);

	//Bars
	Player player1 = createPlayer(WHITE, PLAYERHEIGHT, PLAYERWIDTH, rectangleLeft);
	Player player2 = createPlayer(BLUE, PLAYERHEIGHT, PLAYERWIDTH, rectangleRight);

	//Ball``
	Ball ball = createBall(ballColor, RATIO, ballSpeed, ballPosition);

	// Set our game to run at 60 frames-per-second
	SetTargetFPS(60);

	// Main game loop
	// Detect window close button or ESC key
	GameState state = MENU;
	while (!WindowShouldClose()) {
		framesCounter++;

		// Update
		switch (state) {
		case MENU:
			if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_M)) state = PLAYING;
			break;
		case PLAYING:
			if (IsKeyPressed(KEY_SPACE)) {
				state = PAUSED;
				break;
			}

			// Movement player 1
			if (IsKeyDown(KEY_W)) player1->bar.y -= 15.0f;
			else if (IsKeyDown(KEY_S)) player1->bar.y += 15.0f;

			// Movement player 2
			if (IsKeyDown(KEY_UP)) player2->bar.y -= 15.0f;
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
			DrawText(TextFormat("Score Player 2: %i", player2->score), 3*SCREENWIDTH/4, 25, 20, player2->color);
			DrawText(TextFormat("Matches: %i / %i", player1->matchScore, totalMatches), SCREENWIDTH/30, 50, 20, player1->color);
			DrawText(TextFormat("Matches: %i / %i", player2->matchScore, totalMatches), 3*SCREENWIDTH/4, 50, 20, player2->color);

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
			DrawText("press ENTER or M to START.", SCREENWIDTH/2 - 100, 5*SCREENHEIGHT/6, 15, WHITE);
			DrawText("press SPACE to PAUSE.", SCREENWIDTH/2 - 77, 7*SCREENHEIGHT/8, 15, WHITE);
		}//Draw victory player 1
		else if (state == GAME_OVER && player1->matchScore >= totalMatches) {
			DrawText("Player 1 WINS", SCREENWIDTH/3 - 200, SCREENHEIGHT/2, 30, player1->color);
			DrawText("Press ENTER for REMATCH   Press ESC to EXIT", SCREENWIDTH/2 - 305, 3*SCREENHEIGHT/4, 20, WHITE);
			DrawText("Press M to go MAIN MENU", SCREENWIDTH/2 - 305, 6*SCREENHEIGHT/7, 20, WHITE);
		}//Draw victory player 2
		else if (state == GAME_OVER) {
			DrawText("Press ENTER for REMATCH   Press ESC to EXIT", SCREENWIDTH/2 - 305, 3*SCREENHEIGHT/4, 20, WHITE);
			DrawText("Player 2 WINS", SCREENWIDTH/2 + 100, SCREENHEIGHT/2, 30, player2->color);
			DrawText("Press M to go MAIN MENU", SCREENWIDTH/2 - 305, 6*SCREENHEIGHT/7, 20, WHITE);
		}

		//FPS
		DrawFPS(10, 10);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
