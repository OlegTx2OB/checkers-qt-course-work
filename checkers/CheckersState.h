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

    uint8 getSquaresCountByDiagonal();
    uint8 & at(uint8 i, uint8 j);
    uint8 getFigureColor(uint i, uint j);
    void allocateMemory(uint8 getSquaresCountByDiagonal);

    bool isQueen(uint8 i, uint8 j);
    bool isNull(uint8 i, uint8 j);


//getters-setters
    int & getXScore();
    int & getDeletedMove();
    std::vector < Point > & getXMove();

    void setXScore(int xScore);
    void setDeletedMove(int deletedMove);
    void setXMove(std::vector < Point > xMove);
    void setParent(CheckersState * parent);


public:
    std::vector < uint8 > figuresCount;
    std::vector < CheckersState * > childStatesVector;

private:
    CheckersState * parent;
    std::vector < Point > xMove;
    uint8 ** data;
    uint8 squaresCountByDiagonalTMP;
    int deletedMove;
    int xScore;

    //because of stupid spp this variable is used
    uint8 tmp;
};
