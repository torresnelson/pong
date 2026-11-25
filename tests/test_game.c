// Unit tests for the game logic in src/game.c (no window required).
#include <math.h>
#include <stdio.h>
#include "../include/pong.h"

static int failures = 0;

#define CHECK(cond) do { \
	if (!(cond)) { \
		failures++; \
		printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
	} \
} while (0)

static Field newField(void) {
	return createField(colorBackground, lineColor, lineThickness, top, bottom, leftGoal, rightGoal);
}

static void testClampSpeed(void) {
	Vector2 c = clampSpeed((Vector2){ -40, 20 }, maxSpeed);
	CHECK(c.x == -maxSpeed.x);
	CHECK(c.y == maxSpeed.y);
	Vector2 same = clampSpeed((Vector2){ 3, -2 }, maxSpeed);
	CHECK(same.x == 3 && same.y == -2);
}

static void testScoreManager(void) {
	Player p1 = createPlayer(WHITE, PLAYERHEIGHT, PLAYERWIDTH, rectangleLeft);
	Player p2 = createPlayer(BLUE, PLAYERHEIGHT, PLAYERWIDTH, rectangleRight);
	CHECK(scoreManager(p1, p2, 10, 3) == PLAYING);
	for (int match = 1; match <= 3; match++) {
		p1->score = 10;
		p2->score = 4;
		GameState state = scoreManager(p1, p2, 10, 3);
		// The deciding match must end the game immediately
		CHECK(state == (match < 3 ? MATCH_OVER : GAME_OVER));
		CHECK(p1->matchScore == match);
		CHECK(p1->score == 0 && p2->score == 0);
	}
	resetGame(p1, p2);
	CHECK(p1->matchScore == 0 && p2->matchScore == 0);
	p2->score = 10;
	CHECK(scoreManager(p1, p2, 10, 1) == GAME_OVER);
	CHECK(p2->matchScore == 1);
	free(p1);
	free(p2);
}

// Fire the ball at a paddle and count how many times it changes direction
static int paddleFlips(float speedX, float startY, int rightSide) {
	Field field = newField();
	Player p1 = createPlayer(WHITE, PLAYERHEIGHT, PLAYERWIDTH, rectangleLeft);
	Player p2 = createPlayer(BLUE, PLAYERHEIGHT, PLAYERWIDTH, rectangleRight);
	Vector2 position = { rightSide ? SCREENWIDTH - 100 : 100, startY };
	Vector2 speed = { rightSide ? speedX : -speedX, 0.05f };
	Ball ball = createBall(RED, RATIO, speed, position);
	int flips = 0;
	float last = ball->speed.x;
	for (int i = 0; i < (int)(300 / speedX); i++) {
		collisions(p1, p2, ball, field);
		ballMovement(ball);
		if ((ball->speed.x > 0) != (last > 0)) flips++;
		last = ball->speed.x;
	}
	free(field);
	free(p1);
	free(p2);
	free(ball);
	return flips;
}

static void testPaddleBouncesOnce(void) {
	for (float speedX = 1; speedX <= maxSpeed.x; speedX += 0.5f)
		for (float y = SCREENHEIGHT/2 + 5; y < SCREENHEIGHT/2 + PLAYERHEIGHT - 5; y += 10) {
			CHECK(paddleFlips(speedX, y, 0) == 1);
			CHECK(paddleFlips(speedX, y, 1) == 1);
		}
}

static void testBallBehindPaddleIsNotSaved(void) {
	Field field = newField();
	Player p1 = createPlayer(WHITE, PLAYERHEIGHT, PLAYERWIDTH, rectangleLeft);
	Player p2 = createPlayer(BLUE, PLAYERHEIGHT, PLAYERWIDTH, rectangleRight);
	Ball ball = createBall(RED, RATIO, (Vector2){ -5, 0 }, (Vector2){ 3, SCREENHEIGHT/2 + 50 });
	collisions(p1, p2, ball, field);
	CHECK(ball->speed.x == -5);
	free(field);
	free(p1);
	free(p2);
	free(ball);
}

static void testFloorBounceStaysInField(void) {
	Field field = newField();
	Player p1 = createPlayer(WHITE, PLAYERHEIGHT, PLAYERWIDTH, rectangleLeft);
	Player p2 = createPlayer(BLUE, PLAYERHEIGHT, PLAYERWIDTH, rectangleRight);
	Ball ball = createBall(RED, RATIO, (Vector2){ 3, 0.3f }, (Vector2){ 400, SCREENHEIGHT - 2 });
	collisions(p1, p2, ball, field);
	CHECK(ball->position.y == SCREENHEIGHT - RATIO);
	CHECK(ball->speed.y < 0);
	free(field);
	free(p1);
	free(p2);
	free(ball);
}

int main(void) {
	testClampSpeed();
	testScoreManager();
	testPaddleBouncesOnce();
	testBallBehindPaddleIsNotSaved();
	testFloorBounceStaysInField();
	printf(failures ? "%d check(s) failed\n" : "all tests passed\n", failures);
	return failures != 0;
}
