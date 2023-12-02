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
    bool setGameType(int type);
    void setSearchDepth(int level);
    void startNewGame(int color);
	void endGame();
public slots:
	void setClicked(int i, int j);
signals:
    void stateChanged(CheckersState *);
    void vectorChanged(std::vector <Point> & v);
	void vectorDeleted();
	void gameEnded(uint8);
private:
    void firstClick(int i, int j);
    void secondClick(int i, int j);

    // AI
    void go();
    int goRecursive(CheckersState * state, int level, int alpha, int beta);
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
	uint8 whoWin(CheckersState * state);

    std::vector <std::vector <Point> > history;
	std::vector <CheckersState *> historyStates;
	int historyNum;

    std::vector <CheckersState *> moveSearch;
    std::vector < Point > tmpVector;
    Point tmpPoint;
    void printPointVector(std::vector <Point> & v);

    CheckersState * firstCheckersState;
    CheckersState * currentCheckersState;
    CheckersState * tmpCheckersState;

    int squaresCountOnDiagonal;
    int checkersRowsCount;
    int type;
    int computerColor;
    int humanColor;

    int level;
    int click;
    char ix[4];
    char jx[4];

    bool captureFound;
    bool gameRunning;
    int maxLevel;


	int cleared;
	int created;
};
