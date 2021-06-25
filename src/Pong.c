#include "raylib.h"
#include "../include/Pong.h"
#include <stdio.h>

Player createPlayer(Color color, int height, int width, Rectangle bar) {
	Player p = (Player)malloc(sizeof(struct sPlayer));
	p->color = color;
	p->height = height;
	p->width = width;
	p->bar = bar;
	p->score = 0;
	p->matchScore = 0;
	p->game = 0;
	return p;
}
Ball createBall(Color color, int ratio, Vector2 speed, Vector2 position) {
	Ball b = (Ball)malloc(sizeof(struct sBall));
	b->color = color;
	b->ratio = ratio;
	b->speed = speed;
	b->position = position;
	return b;
}
Field createField(Color colorBackground, Color colorLine, int lineThickness, int top, int bottom, int leftGoal, int rightGoal) {
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
void resetGame(Player player1, Player player2) {
	menu = 1;
	game = 0;
	halt = 0;
	pause = 0;
	player1->score = 0;
	player2->score = 0;
	player1->matchScore = 0;
	player2->matchScore = 0;
	player1->game = 0;
	player2->game = 0;
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
void scoreManager(Player player1, Player player2, int maxScore, int totalMatches) {
	if (player1->matchScore >= totalMatches) player1->game = 1;
	else if (player2->matchScore >= totalMatches) player2->game = 1;
	if (player1->score >= maxScore) {
		player1->matchScore += 1;
		halt = 1;
		player1->score = 0;
		player2->score = 0;
	} else if (player2->score >= maxScore) {
		player2->matchScore += 1;
		halt = 1;
		player1->score = 0;
		player2->score = 0;
	}
}
// Movement of the ball
void ballMovement(Ball ball) {
	ball->position.x += ball->speed.x;
	ball->position.y += ball->speed.y;
}
void eyeBall(Player player1, Player player2, Ball ball) {
	if ((ball->position.x > SCREENWIDTH + 2*RATIO )) { 
		ball = ballReset(ball);
		player1->score += 1;
	} else if ((ball->position.x < - 2*RATIO)) { 
		ball = ballReset(ball);
		player2->score += 1;
	}
}	
void collisions(Player player1, Player player2, Ball ball, Field field) {
	// Check roof and floor collisions for bouncing
	int randomPitch = GetRandomValue(4,7);
	float randomPitchReal = randomPitch*.1 + 0.5;
	if ((ball->position.y >= (SCREENHEIGHT - ball->ratio)) || (ball->position.y <= ball->ratio)) ball->speed.y *= -1.0f;
	if (CheckCollisionCircleRec(ball->position, RATIO, player1->bar)) collision = 1;
	if (CheckCollisionCircleRec(ball->position, RATIO, player2->bar)) collision = 1;
	//Check collisions 
	if (collision) ball->speed.x *= (-1.0f*randomPitchReal);
	if (collision) ball->speed.y *= (1.0f*randomPitchReal);
	printf("\n\n\n\n");
	printf("-----------------------\n");
	printf("ball->speed.x:  %.2f\n", ball->speed.x);
	printf("randomPitchReal:  %.2f\n", randomPitchReal);

	// Checks collisions between BARS, LIMIT SUP and INF
	if (player1->bar.y <=  field->top) player1->bar.y *= field->top;
	else if (player1->bar.y >= field->bottom) player1->bar.y = field->bottom;
	if (player2->bar.y <=  field->top) player2->bar.y *= field->top;
	else if (player2->bar.y >= field->bottom) player2->bar.y = field->bottom;		
}
void referee(Player player1, Player player2, Ball ball, Field field, int maxScore, int totalMatches) {	
	eyeBall(player1, player2, ball);
	scoreManager(player1, player2, maxScore, totalMatches);
	collisions(player1, player2, ball, field);
	ballMovement(ball);
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
	while (!WindowShouldClose()) {

		// Update
		//pause and reset match
		if (IsKeyPressed(KEY_SPACE)) {			
			if (!pause && !halt && !game && !menu) pause = !pause;
			else if (IsKeyPressed(KEY_SPACE) && !halt && !game && !menu) pause = !pause;
			if (!pause && halt && !game && !menu) halt = !halt;
		}

		// When game is finish go to main menu
		if (IsKeyPressed(KEY_ENTER) && (player1->game || player2->game)) resetGame(player1, player2);
		if (IsKeyPressed(KEY_ENTER) && !pause && !halt && !game && menu && !(player1->game || player2->game)) menu = !menu;

		collision = false;    

		if (!pause && !halt && !game && !menu) {

			// Movement player 1
			if (IsKeyDown(KEY_W)  && !pause) player1->bar.y -= 15.0f;
			else if (IsKeyDown(KEY_S) && !pause) player1->bar.y += 15.0f;

			// Movement player 2
			if (IsKeyDown(KEY_UP) && !pause) player2->bar.y -= 15.0f;
			else if (IsKeyDown(KEY_DOWN) && !pause) player2->bar.y += 15.0f;

			// Check when scores and reset the match
			if (!game && !menu) referee(player1, player2, ball, field, maxScore, totalMatches);

		// Go to menu
		} else if (IsKeyPressed(KEY_M) && !pause && !halt && !game && menu && !(player1->game || player2->game)) menu = !menu;
		else if (IsKeyPressed(KEY_M) && halt && (player1->game || player2->game)) resetGame(player1, player2);
		else if (IsKeyPressed(KEY_M) && pause && !menu && !halt && !game) resetGame(player1, player2);
		// Draw +1 frame
		else framesCounter++;

		BeginDrawing();
			if (!game && !menu) {
				//Draw the Field 
				ClearBackground(field->colorBackground);
				DrawLineEx(startPos, endPos, field->lineThickness, field->colorLine);
				
				//Draw the Ball
				DrawCircleV(ball->position, ball->ratio, ball->color);
				
				//Draw the Players
				DrawRectangle(0,player1->bar.y, 9, player1->bar.height, player1->color);
				DrawRectangle(player2->bar.x,player2->bar.y, 9, player2->bar.height, player1->color);

				//Draw players Scores 
				DrawText(FormatText("Score Player 1: %i", player1->score), SCREENWIDTH/30, 25, 20, player1->color);
				DrawText(FormatText("Score Player 2: %i", player2->score), 3*SCREENWIDTH/4, 25, 20, player1->color);
				DrawText(FormatText("Matches: %i / %i", player1->matchScore, totalMatches), SCREENWIDTH/30, 50, 20, player1->color);
				DrawText(FormatText("Matches: %i / %i", player2->matchScore, totalMatches), 3*SCREENWIDTH/4, 50, 20, player1->color);

				// On pause, we draw a blinking message
				if (pause && ((framesCounter/30)%2)) DrawText("PAUSED", SCREENWIDTH/2 - 70, SCREENHEIGHT/3, 30, GRAY);

				// On halt, we draw a blinking message to continnue
				if (halt && !(player1->game || player2->game) && ((framesCounter/30)%2)) DrawText("Press SPACE to CONTINUE.", SCREENWIDTH/2 - 260, SCREENHEIGHT/2, 30, GREEN);
			}

			//Draw the front menu 
			if (menu && !(player1->game || player2->game)) {
				ClearBackground(field->colorBackground);
				DrawText("Pong ", SCREENWIDTH/2 - 210, SCREENHEIGHT/3, 180, RED);
				DrawText("Pong ", SCREENWIDTH/2 - 213, SCREENHEIGHT/3, 178, BLUE);
				DrawText("titoreboot inc.", SCREENWIDTH/2 - 25, 3*SCREENHEIGHT/4, 10, WHITE);
				DrawText("press ENTER or M to START.", SCREENWIDTH/2 - 100, 5*SCREENHEIGHT/6, 15, WHITE);
				DrawText("press SPACE to PAUSE.", SCREENWIDTH/2 - 77, 7*SCREENHEIGHT/8, 15, WHITE);
			}//Draw victory player 1
			else if (halt && !menu && player1->game) {	
				ClearBackground(field->colorBackground);
				DrawText("Player 1 WINS", SCREENWIDTH/3 - 200, SCREENHEIGHT/2, 30, BLUE);
				DrawText("Press ENTER for REMATCH   Press ESC to EXIT", SCREENWIDTH/2 - 305, 3*SCREENHEIGHT/4, 20, WHITE);
				DrawText("Press M to go MAIN MENU", SCREENWIDTH/2 - 305, 6*SCREENHEIGHT/7, 20, WHITE);
			}//Draw victory player 2
			else if (halt && !menu && player2->game) {
				ClearBackground(field->colorBackground);
				DrawText("Press ENTER for REMATCH   Press ESC to EXIT", SCREENWIDTH/2 - 305, 3*SCREENHEIGHT/4, 20, WHITE);
				DrawText("Player 2 WINS", SCREENWIDTH/2 + 100, SCREENHEIGHT/2, 30, RED);
				DrawText("Press M to go MAIN MENU", SCREENWIDTH/2 - 305, 6*SCREENHEIGHT/7, 20, WHITE);
			}

			//FPS
			DrawFPS(10, 10);

		EndDrawing();
	}
	CloseWindow();
	return 0;
}