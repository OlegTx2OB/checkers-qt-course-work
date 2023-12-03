#include "CheckersState.h"

CheckersState::CheckersState(int squaresCountByDiagonal)
{
    allocateMemory(squaresCountByDiagonal);
}

CheckersState::CheckersState(const CheckersState *source)
{
    allocateMemory(source->squaresCountByDiagonalTMP);
    for(int i = 0; i < squaresCountByDiagonalTMP; i++)
        for(int j = 0; j < squaresCountByDiagonalTMP / 2; j++)
            data[i][j] = source->data[i][j];
    figuresCount = source->figuresCount;
}

CheckersState::~CheckersState()
{
    for(int i=0; i<squaresCountByDiagonalTMP; i++)
    delete[] data[i];
	delete[] data;
}

void CheckersState::allocateMemory(uint8 squaresCountByDiagonal)
{
    squaresCountByDiagonalTMP = squaresCountByDiagonal;
    data = new uint8 * [squaresCountByDiagonalTMP];
    for(int i=0; i < squaresCountByDiagonalTMP; i++)
	{
        data[i] = new uint8[squaresCountByDiagonalTMP/2];
        memset( data[i], 0, squaresCountByDiagonalTMP/2*sizeof(uint8) );
	}
    deletedFiguresCount = 0;
}

uint8 CheckersState::getSquaresCountByDiagonal()
{
    return squaresCountByDiagonalTMP;
}

CheckersState * CheckersState::generateNextState(std::vector <Point> & v)
{
	CheckersState * state = new CheckersState(this);
	uint8 tmp;
    for(unsigned k=0; k<v.size(); k++)
    {
		uint8 i = v.at(k).x;
		uint8 j = v.at(k).y;
        if(v.at(k).type == MOVEDFROM)
        {
            tmp = state->at(i,j);
            state->at(i,j) = 0;
        }
        else if(v.at(k).type == MOVEDTO)
        {
            state->at(i,j) = tmp;
        }
        else if(v.at(k).type == MARKDELETED)
        {
            state->at(i,j) = MARKDELETED;

        }
        else if(v.at(k).type == DELETED)
        {
            state->at(i,j) = 0;
        }
        else if(v.at(k).type == TOQUEEN)
        {
            if( tmp == WHITE )
                state->at(i,j) = WHITEQUEEN;
            if( tmp == BLACK )
                state->at(i,j) = BLACKQUEEN;
        }
	}
    state->setMovePath(v);
    state->figuresCount.clear();
	return state;
}

std::vector < Point > & CheckersState::getMovePath()
{
    return movePath;
}
void CheckersState::setMovePath(std::vector < Point > xMove)
{
    this->movePath = xMove;
}


int & CheckersState::getMoveEvaluationScore()
{
    return moveEvaluationScore;
}
void CheckersState::setMoveEvaluationScore(int xScore)
{
    this->moveEvaluationScore = xScore;
}

int & CheckersState::getDeletedFiguresCount()
{
    return deletedFiguresCount;
}
void CheckersState::setDeletedFiguresCount(int deletedMove)
{
    this->deletedFiguresCount = deletedMove;
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

uint8 CheckersState::getFigureColor(uint i, uint j)
{
    if( at(i,j) == WHITE || at(i,j) == WHITEQUEEN)
		return WHITE;
    if( at(i,j) == BLACK || at(i,j) == BLACKQUEEN)
		return BLACK;
    return 0;
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
