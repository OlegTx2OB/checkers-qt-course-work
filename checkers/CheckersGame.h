#pragma once
#include <QObject>
#include <QVector>
#include <vector>
#include <ctime>
#include "CheckersState.h"

enum GameType
{
	RUSSIAN = 1,
    INTERNATIONAL = 2,
};

class CheckersGame : public QObject
{
	Q_OBJECT
public:
    CheckersGame();
	~CheckersGame();
    bool setGameType(int gameType);
    void setSearchDepth(int searchDepth);
    void startNewGame(int color);
	void endGame();
public slots:
	void setClicked(int i, int j);
signals:
    void stateChanged(CheckersState *);
    void vectorChanged(std::vector <Point> & v);
	void vectorDeleted();
private:
    void firstClick(int i, int j);
    void secondClick(int i, int j);

    // AI
    void go();
    int goRecursive(CheckersState * state, int searchDepth, int alpha, int beta);
    void calcCounts(CheckersState * state);
    int evaluation(CheckersState * state);
    void findAndProcessPossibleMoves(CheckersState * state, uint8 color);
    bool checkTerminatePosition(CheckersState * state);
    int countAvailableMoves(CheckersState * state, int i, int j);
    int searchMove(CheckersState * state, int i, int j, std::vector <Point> & vp);
    bool isCoordinateValid(char x);
	void clearTree(CheckersState * state, bool clearlists = true, bool onlychilds = false);
	void clearTreeRecursive(CheckersState * state, bool clearlists = false);
    bool makeMove(Point p1, Point p2);
    void printPointVector(std::vector <Point> & v);



    std::vector <CheckersState *> possibleNextStates;
    Point clickPointCoordinates;

    CheckersState * firstState;
    CheckersState * currentState;

    int squaresCountOnDiagonal;
    int checkersRowsCount;
    int gameType;
    int computerColor;
    int humanColor;
    int searchDepth;


    bool isSecondClick;
    bool isCaptureFound;


//Arrays are used to store values that determine the direction of movement of checkers in the game
    int directionsX[4];
    int directionsY[4];

};
