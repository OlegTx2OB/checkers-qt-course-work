#pragma once
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <cstdint>
#include <stdint.h>

typedef unsigned char uint8;
typedef unsigned int uint;

enum checker
{
    WHITE = 1,
    BLACK = 2,
    WHITEQUEEN = 3,
    BLACKQUEEN = 4,

    CLEARCELL = 10,

    MOVEDFROM = 20,
    MOVEDTO = 21,

    MOVEDTHROUGH = 22,
    DELETED = 23,
    MARKDELETED = 24,
    TOQUEEN = 25
};

class Point
{
public:
    Point() {x=0; y=0; type=0;}
    Point(int x, int y, int type) : x(x), y(y), type(type) {}

    int x;
    int y;
    int type;

bool operator == (const Point & p ) {return ( x==p.x && y==p.y && type==p.type );}

};

class CheckersState
{
public:
    CheckersState(int squaresCountByDiagonal);
    ~CheckersState();
    CheckersState(const CheckersState *source);

    CheckersState * generateNextState(std::vector <Point> & v);

    uint8 & at(uint8 i, uint8 j);
    uint8 getFigureColor(uint i, uint j);
    void allocateMemory(uint8 getSquaresCountByDiagonal);

    bool isQueen(uint8 i, uint8 j);
    bool isNull(uint8 i, uint8 j);


//getters-setters
    uint8 getSquaresCountByDiagonal();
    int & getMoveEvaluationScore();
    int & getDeletedFiguresCount();
    std::vector < Point > & getMovePath();

    void setMoveEvaluationScore(int moveEvaluationScore);
    void setDeletedFiguresCount(int deletedFiguresCount);
    void setMovePath(std::vector < Point > movePath);

public:
    std::vector < uint8 > figuresCount;
    std::vector < CheckersState * > nextPossibleStatesCState;

private:
    std::vector < Point > movePath;
    uint8 ** data;
    uint8 squaresCountByDiagonalTMP;
    int deletedFiguresCount;
    int moveEvaluationScore;

    //because of stupid spp this variable is used
    uint8 tmp;
};
