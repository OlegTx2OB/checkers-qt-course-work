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
    TOKING = 25
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

    CheckersState * genNextState(std::vector <Point> & v);

    void setParent(CheckersState * getParent);
    std::vector <uint8> & getXCount();
    CheckersState * getParent();
    std::vector < CheckersState * > & getXChild();
    std::vector < Point > & getXMove();
    int & getSetDeletedMove();
    int & getScore();
    uint8 getSquaresCountByDiagonal();
    uint8 & at(uint8 i, uint8 j);
    uint8 color(uint i, uint j);
    void allocate(uint8 getSquaresCountByDiagonal);



    bool isWhite(uint8 i, uint8 j);
    bool isBlack(uint8 i, uint8 j);
    bool isQueen(uint8 i, uint8 j);
    bool isNull(uint8 i, uint8 j);

private:
    CheckersState * p;
    std::vector < CheckersState * > xChild;
    std::vector < Point > xMove;
    std::vector < uint8 > xCount;
    uint8 ** data;
    uint8 squaresCountByDiagonalTMP;
    int deletedMove;


    //because of stupid spp these variables are used
    uint8 tmp;
    int xscore;
};
