#include "CheckersState.h"

CheckersState::CheckersState(int squaresCountByDiagonal)
{
    allocate(squaresCountByDiagonal);
}

CheckersState::CheckersState(const CheckersState *source)
{
    allocate(source->squaresCountByDiagonalTMP);
    for(int i = 0; i < squaresCountByDiagonalTMP; i++)
        for(int j = 0; j < squaresCountByDiagonalTMP / 2; j++)
            data[i][j] = source->data[i][j];
    xCount = source->xCount;
}

CheckersState::~CheckersState()
{
    for(int i=0; i<squaresCountByDiagonalTMP; i++)
    delete[] data[i];
	delete[] data;
}

void CheckersState::allocate(uint8 squaresCountByDiagonal)
{
    squaresCountByDiagonalTMP = squaresCountByDiagonal;
    data = new uint8 * [squaresCountByDiagonalTMP];
    for(int i=0; i<squaresCountByDiagonalTMP; i++)
	{
        data[i] = new uint8[squaresCountByDiagonalTMP/2];
        memset( data[i], 0, squaresCountByDiagonalTMP/2*sizeof(uint8) );
	}
	p = NULL;
    deletedMove = 0;
}

uint8 CheckersState::getSquaresCountByDiagonal()
{
    return squaresCountByDiagonalTMP;
}

std::vector <uint8> & CheckersState::getXCount()
{
    return xCount;
}

CheckersState * CheckersState::genNextState(std::vector <Point> & v) {
	CheckersState * state = new CheckersState(this);
	uint8 tmp;
	for(unsigned k=0; k<v.size(); k++) {
		uint8 i = v.at(k).x;
		uint8 j = v.at(k).y;
		switch(v.at(k).type) {
			case(MOVEDFROM):
				tmp = state->at(i,j);
				state->at(i,j) = 0;
				break;
			case(MOVEDTO):
				state->at(i,j) = tmp;
				break;
			case(MARKDELETED):
				state->at(i,j) = MARKDELETED;
				break;
			case(DELETED):
				state->at(i,j) = 0;
				break;
			case(TOKING):
				if( tmp == WHITE )
                    state->at(i,j) = WHITEQUEEN;
				if( tmp == BLACK )
                    state->at(i,j) = BLACKQUEEN;
				break;
			default:
				break;
		}
	}
    state->getXMove() = v;
    state->getXCount().clear();
	return state;
}

void CheckersState::setParent(CheckersState * parent)
{
	p = parent;
}

CheckersState * CheckersState::getParent()
{
	return p;
}

std::vector < CheckersState * > & CheckersState::getXChild()
{
    return xChild;
}

std::vector < Point > & CheckersState::getXMove()
{
    return xMove;
}

int & CheckersState::getScore()
{
	return xscore;
}

int & CheckersState::getSetDeletedMove()
{
    return deletedMove;
}

uint8 & CheckersState::at(uint8 i, uint8 j)
{
    if(i%2 != j%2)
    {
        tmp = CLEARCELL;
        return tmp;
	}
	return data[i][j/2];
}

uint8 CheckersState::color(uint i, uint j)
{
    if( at(i,j) == WHITE || at(i,j) == WHITEQUEEN)
		return WHITE;
    if( at(i,j) == BLACK || at(i,j) == BLACKQUEEN)
		return BLACK;
    return 0;
}

bool CheckersState::isWhite(uint8 i, uint8 j)
{
    return (at(i,j)==WHITE || at(i,j)==WHITEQUEEN);
}

bool CheckersState::isBlack(uint8 i, uint8 j)
{
    return (at(i,j)==BLACK || at(i,j)==BLACKQUEEN);
}

bool CheckersState::isQueen(uint8 i, uint8 j)
{
    return (at(i,j)==WHITEQUEEN || at(i,j)==BLACKQUEEN);
}

bool CheckersState::isNull(uint8 i, uint8 j)
{
    if( at(i,j) == CLEARCELL )
		return true;
    return !at(i,j);
}
