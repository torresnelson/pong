 #ifndef _PONG_H_
 #define _PONG_H_ 
 
	#include "raylib.h"
	#include "stdlib.h"

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

	//States
	typedef enum { MENU, PLAYING, PAUSED, MATCH_OVER, GAME_OVER } GameState;

	//Frezze
	int framesCounter = 0;

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

#endif