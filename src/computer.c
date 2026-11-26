#include "raylib.h"
#include "../include/pong.h"
#include <math.h>

// Computer opponent. It plays the right paddle and gets a little better as
// the game goes on: it reacts sooner, moves faster, aims better and, from
// PREDICTLEVEL on, predicts where the ball will land instead of chasing it.

Computer createComputer(void) {
	Computer c = (Computer)malloc(sizeof(struct sComputer));
	c->level = 1;
	c->aimOffset = 0;
	c->tracking = 0;
	return c;
}

// Two levels per match already played plus one every four points of the current match
int computerLevel(Player human, Player cpu) {
	int matches = human->matchScore + cpu->matchScore;
	int points = human->score + cpu->score;
	int level = 1 + 2*matches + points/4;
	return level > MAXLEVEL ? MAXLEVEL : level;
}

// Paddle speed in pixels per frame, always slower than the human (15)
float computerSpeed(int level) {
	return 3.0f + 0.8f*level;
}

// Ball x position from which the computer starts to follow the ball
float computerReactionX(int level) {
	return SCREENWIDTH*(0.75f - 0.05f*level);
}

// Maximum distance in pixels between where the computer aims and the ball
int computerAimError(int level) {
	return (MAXLEVEL - level)*12;
}

// Y position of the ball when it reaches targetX, folding in roof/floor bounces
float predictBallY(Vector2 position, Vector2 speed, float targetX, float radius) {
	if (speed.x <= 0) return position.y;
	float low = radius;
	float span = SCREENHEIGHT - 2*radius;
	float y = position.y + speed.y*(targetX - position.x)/speed.x - low;
	y = fmodf(y, 2*span);
	if (y < 0) y += 2*span;
	if (y > span) y = 2*span - y;
	return low + y;
}

// Move from towards to by at most maxStep
float stepToward(float from, float to, float maxStep) {
	float delta = to - from;
	if (fabsf(delta) > maxStep) delta = copysignf(maxStep, delta);
	return from + delta;
}

void computerUpdate(Computer c, Player cpu, Player human, Ball ball) {
	c->level = computerLevel(human, cpu);
	float center = cpu->bar.y + cpu->bar.height/2;
	float target = center;

	if (ball->speed.x <= 0) {
		// Ball going away: relax back to the middle of the field
		c->tracking = 0;
		target = SCREENHEIGHT/2;
	} else if (ball->position.x >= computerReactionX(c->level)) {
		if (!c->tracking) {
			// Pick a new aim error once per approach so the paddle does not jitter
			int error = computerAimError(c->level);
			c->aimOffset = GetRandomValue(-error, error);
			c->tracking = 1;
		}
		float y = c->level >= PREDICTLEVEL
			? predictBallY(ball->position, ball->speed, cpu->bar.x - ball->ratio, ball->ratio)
			: ball->position.y;
		target = y + c->aimOffset;
	}

	cpu->bar.y += stepToward(center, target, computerSpeed(c->level)) - center;
}
