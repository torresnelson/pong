#include "raylib.h"
#include "../include/pong.h"
#include <math.h>

//Players scores
int maxScore = 10;
int totalMatches = 3;

//Size of the bar
Rectangle rectangleLeft = { 0, SCREENHEIGHT/2, PLAYERWIDTH, PLAYERHEIGHT };
Rectangle rectangleRight = { SCREENWIDTH - PLAYERWIDTH, SCREENHEIGHT/2, PLAYERWIDTH, PLAYERHEIGHT };

//Ball
Color ballColor = RED;
Vector2 ballPosition = { SCREENWIDTH/2, SCREENHEIGHT/2 };
Vector2 maxSpeed = { 15.0f, 8.0f };
Vector2 ballSpeed = { 7.2f, 3.25f };

//Field
Vector2 startPos = { SCREENWIDTH/2, SCREENHEIGHT };
Vector2 endPos = { SCREENWIDTH/2, 0 };
Color colorBackground = BLACK;
Color lineColor = WHITE;
int leftGoal = - RATIO;
int rightGoal = SCREENWIDTH + RATIO;
int lineThickness = 5;
int top = 0;
int bottom = SCREENHEIGHT - PLAYERHEIGHT;

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

// Move the main menu cursor by delta entries, wrapping around at both ends
MenuOption menuSelect(MenuOption option, int delta) {
	int next = ((int)option + delta) % MENU_OPTIONS;
	return (MenuOption)(next < 0 ? next + MENU_OPTIONS : next);
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
