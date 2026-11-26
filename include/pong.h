 #ifndef _PONG_H_
 #define _PONG_H_ 
 
	#include "raylib.h"
	#include <stdlib.h>

	#define SCREENWIDTH 800
	#define SCREENHEIGHT 450
	#define PLAYERHEIGHT 100
	#define PLAYERWIDTH 10
	#define RATIO 10
	//Player
	typedef struct sPlayer { 
		Color color;
		int height;
		int width;
		Rectangle bar;
		Rectangle hitBar;
		int score;
		int matchScore;
	} * Player;

	//Ball
	typedef struct sBall { 
		Color color;
		int ratio;
		Vector2 speed;
		Vector2 position;
	} * Ball;
	//Computer opponent
	#define MAXLEVEL 10
	#define PREDICTLEVEL 4
	typedef struct sComputer {
		int level;
		float aimOffset;
		int tracking;
	} * Computer;

	//Field
	typedef struct sField { 
		Color colorBackground;
		Color colorLine;
		int lineThickness;
		int top;
		int bottom;
		int leftGoal;
		int rightGoal;
	} * Field;

	//Players scores
	extern int maxScore;
	extern int totalMatches;

	//Size of the bar
	extern Rectangle rectangleLeft;
	extern Rectangle rectangleRight;

	//Ball
	extern Color ballColor;
	extern Vector2 ballPosition;
	extern Vector2 maxSpeed;
	extern Vector2 ballSpeed;

	//Field
	extern Vector2 startPos;
	extern Vector2 endPos;
	extern Color colorBackground;
	extern Color lineColor;
	extern int leftGoal;
	extern int rightGoal;
	extern int lineThickness;
	extern int top;
	extern int bottom;

	//States
	typedef enum { MENU, PLAYING, PAUSED, MATCH_OVER, GAME_OVER } GameState;

	//Functions
	Player createPlayer(Color color, int height, int width, Rectangle bar);
	Ball createBall(Color color, int ratio, Vector2 speed, Vector2 position);
	Field createField(Color colorBackground, Color colorLine, int lineThickness, int top, int bottom, int leftGoal, int rightGoal);
	Ball ballReset(Ball ball);
	void eyeBall(Player player1, Player player2, Ball ball);
	GameState scoreManager(Player player1, Player player2, int maxScore, int totalMatches);
	GameState referee(Player player1, Player player2, Ball ball, Field field, int maxScore, int totalMatches);
	void resetGame(Player player1, Player player2);
	void collisions(Player player1, Player player2, Ball ball, Field field);
	void ballMovement(Ball ball);
	Vector2 clampSpeed(Vector2 speed, Vector2 max);
	void paddleBounce(Ball ball);

	//Computer opponent functions
	Computer createComputer(void);
	int computerLevel(Player human, Player cpu);
	float computerSpeed(int level);
	float computerReactionX(int level);
	int computerAimError(int level);
	float predictBallY(Vector2 position, Vector2 speed, float targetX, float radius);
	float stepToward(float from, float to, float maxStep);
	void computerUpdate(Computer c, Player cpu, Player human, Ball ball);

#endif