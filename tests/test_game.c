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

static void testPredictBallY(void) {
	// Straight shot lands where it is
	CHECK(predictBallY((Vector2){ 100, 200 }, (Vector2){ 5, 0 }, 700, RATIO) == 200);
	// Ball moving away keeps its current height
	CHECK(predictBallY((Vector2){ 100, 200 }, (Vector2){ -5, 3 }, 700, RATIO) == 200);
	// Bounces once off the floor: 200 + 300 = 500 is 60 past the floor (440)
	CHECK(fabsf(predictBallY((Vector2){ 400, 200 }, (Vector2){ 4, 4 }, 700, RATIO) - 380) < 0.01f);
	// Bounces once off the roof
	CHECK(fabsf(predictBallY((Vector2){ 400, 100 }, (Vector2){ 4, -4 }, 700, RATIO) - 220) < 0.01f);
}

static void testStepToward(void) {
	CHECK(stepToward(100, 105, 10) == 105);
	CHECK(stepToward(100, 300, 10) == 110);
	CHECK(stepToward(100, 0, 10) == 90);
}

static void testComputerLevelGrows(void) {
	Player p1 = createPlayer(WHITE, PLAYERHEIGHT, PLAYERWIDTH, rectangleLeft);
	Player p2 = createPlayer(BLUE, PLAYERHEIGHT, PLAYERWIDTH, rectangleRight);
	CHECK(computerLevel(p1, p2) == 1);
	p1->score = 2;
	p2->score = 2;
	CHECK(computerLevel(p1, p2) == 2);
	p1->matchScore = 1;
	CHECK(computerLevel(p1, p2) == 4);
	p2->matchScore = 10;
	CHECK(computerLevel(p1, p2) == MAXLEVEL);
	free(p1);
	free(p2);
}

static void testComputerGetsHarder(void) {
	for (int level = 2; level <= MAXLEVEL; level++) {
		CHECK(computerSpeed(level) > computerSpeed(level - 1));
		CHECK(computerReactionX(level) < computerReactionX(level - 1));
		CHECK(computerAimError(level) < computerAimError(level - 1));
	}
	// Never faster than a human paddle, and perfect aim only at the top
	CHECK(computerSpeed(MAXLEVEL) < 15.0f);
	CHECK(computerAimError(MAXLEVEL) == 0);
}

// Serve towards the computer and report whether it sends the ball back
static int computerReturns(int level, Vector2 speed, float startY) {
	Field field = newField();
	Player p1 = createPlayer(WHITE, PLAYERHEIGHT, PLAYERWIDTH, rectangleLeft);
	Player p2 = createPlayer(BLUE, PLAYERHEIGHT, PLAYERWIDTH, rectangleRight);
	p1->score = 4*(level - 1);
	Computer computer = createComputer();
	Ball ball = createBall(RED, RATIO, speed, (Vector2){ SCREENWIDTH/2, startY });
	int returned = 0;
	while (!returned && ball->position.x < SCREENWIDTH + 2*RATIO) {
		computerUpdate(computer, p2, p1, ball);
		collisions(p1, p2, ball, field);
		ballMovement(ball);
		returned = ball->speed.x < 0;
	}
	CHECK(computer->level == level);
	free(field);
	free(p1);
	free(p2);
	free(computer);
	free(ball);
	return returned;
}

static void testTopLevelReturnsEveryServe(void) {
	for (float sx = 4; sx <= maxSpeed.x; sx += 1.5f)
		for (float sy = -maxSpeed.y; sy <= maxSpeed.y; sy += 2)
			for (float y = 50; y < SCREENHEIGHT; y += 100)
				CHECK(computerReturns(MAXLEVEL, (Vector2){ sx, sy }, y));
}

static void testFirstLevelMissesSometimes(void) {
	int misses = 0;
	for (float sy = -maxSpeed.y; sy <= maxSpeed.y; sy += 2)
		misses += !computerReturns(1, (Vector2){ maxSpeed.x, sy }, 50);
	CHECK(misses > 0);
}

static void testMenuSelectWraps(void) {
	CHECK(menuSelect(MENU_ONE_PLAYER, 1) == MENU_TWO_PLAYERS);
	CHECK(menuSelect(MENU_TWO_PLAYERS, 1) == MENU_ONE_PLAYER);
	CHECK(menuSelect(MENU_ONE_PLAYER, -1) == MENU_TWO_PLAYERS);
	CHECK(menuSelect(MENU_TWO_PLAYERS, -1) == MENU_ONE_PLAYER);
	CHECK(menuSelect(MENU_ONE_PLAYER, 0) == MENU_ONE_PLAYER);
}

static void testTouchControls(void) {
	CHECK(touchControlAt((Vector2){ 50, 50 }) == TOUCH_UP);
	CHECK(touchControlAt((Vector2){ 50, SCREENHEIGHT - 10 }) == TOUCH_DOWN);
	CHECK(touchControlAt((Vector2){ SCREENWIDTH/3 - 1, SCREENHEIGHT/2 - 1 }) == TOUCH_UP);
	CHECK(touchControlAt((Vector2){ SCREENWIDTH/3 - 1, SCREENHEIGHT/2 + 1 }) == TOUCH_DOWN);
	CHECK(touchControlAt((Vector2){ SCREENWIDTH/2, 20 }) == TOUCH_PAUSE);
	// The rest of the field, where the ball and the computer play, is not a control
	CHECK(touchControlAt((Vector2){ SCREENWIDTH/2, SCREENHEIGHT/2 }) == TOUCH_NONE);
	CHECK(touchControlAt((Vector2){ SCREENWIDTH - 20, 20 }) == TOUCH_NONE);
	CHECK(touchControlAt((Vector2){ -1, -1 }) == TOUCH_NONE);
	// Controls must not overlap
	CHECK(!CheckCollisionRecs(touchControlRect(TOUCH_PAUSE), touchControlRect(TOUCH_UP)));
}

int main(void) {
	testClampSpeed();
	testScoreManager();
	testPaddleBouncesOnce();
	testBallBehindPaddleIsNotSaved();
	testFloorBounceStaysInField();
	testPredictBallY();
	testStepToward();
	testComputerLevelGrows();
	testComputerGetsHarder();
	testTopLevelReturnsEveryServe();
	testFirstLevelMissesSometimes();
	testMenuSelectWraps();
	testTouchControls();
	printf(failures ? "%d check(s) failed\n" : "all tests passed\n", failures);
	return failures != 0;
}
