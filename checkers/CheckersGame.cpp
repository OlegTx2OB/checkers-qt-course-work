#include "CheckersGame.h"

CheckersGame::CheckersGame()
{
	setGameType(RUSSIAN);

	srand(time(NULL));
    state = nullptr;
}

CheckersGame::~CheckersGame()
{
	endGame();
}


/*setting the game mode: Russian checkers or international.
 * in Russian there are 3 rows of checkers and a board 8 by 8,
 * and in international there are 4 rows and 10 by 10
 */
bool CheckersGame::setGameType(int gameType)
{
    if(gameType == RUSSIAN)
    {
        this->gameType = gameType;
        squaresCountOnDiagonal = 8; checkersRowsCount = 3;
		return true;
	}
    if(gameType == INTERNATIONAL)
    {
        this->gameType = gameType;
        squaresCountOnDiagonal = 10; checkersRowsCount = 4;
		return true;
	}
	return false;
}

//When installing more than 7 levels, the game starts to slow down
void CheckersGame::setSearchDepth(int level)
{
    if(level >=3 && level <=7) searchDepth = level;
    else searchDepth = 3;
}

void CheckersGame::startNewGame(int chosenComputerColor)
{
    computerColor = chosenComputerColor;
    if(chosenComputerColor == WHITE) humanColor = BLACK;
    else humanColor = WHITE;
    state = new CheckersState(squaresCountOnDiagonal);

    for(int i=0; i<squaresCountOnDiagonal; i++)
    {
        for(int j=0; j<checkersRowsCount; j++)
        {
            if(i%2 == j%2)
                state->at(i,j) = WHITE;
        }
        for(int j=squaresCountOnDiagonal-checkersRowsCount; j<squaresCountOnDiagonal; j++)
        {
            if(i%2 == j%2)
                state->at(i,j) = BLACK;
        }
    }

    isSecondClick = false;

    directionsX[0] = -1; directionsX[1] = 1; directionsX[2] = -1; directionsX[3] = 1;
    directionsY[0] = 1; directionsY[1] = 1; directionsY[2] = -1; directionsY[3] = -1;

    if(computerColor == WHITE) performComputerMove();
    else
    {
        emit stateChanged(state);
        findAndProcessPossibleMoves(state,humanColor);
    }
}

void CheckersGame::endGame()
{
    if(state)
    {
        clearTree(state, true, true);
        state = NULL;
	}
}

void CheckersGame::findAndProcessPossibleMoves(CheckersState * state, uint8 color)
{
    std::vector <Point> possibleMovePoints;
    isCaptureFound = false;

    /*iterating over all the squares on the game board.
     *Inside this loop, it checks if the square is accessible
     *for a move for the current color of pieces*/
    for(unsigned i=0; i<squaresCountOnDiagonal; i++)
    {
        for(unsigned j=0; j<squaresCountOnDiagonal; j++)
        {
            if(i%2 == j%2 && state->getFigureColor(i,j) == color)
            {
                searchMove(state, i, j, possibleMovePoints); //search moves from point(i, j)
                clearTree(state, false, true);
			}
		}
	}
    //If a capture is found, the list of possible moves is processed
    if(isCaptureFound)
    {
        for(unsigned i=0; i<possibleNextStatesCGame.size(); i++)
        {
            if( possibleNextStatesCGame.at(i)->getMovePath().size() == 2 )
            {
                delete possibleNextStatesCGame.at(i);
                possibleNextStatesCGame.erase( possibleNextStatesCGame.begin()+i );
				i--;
            }
            else break;
		}
	}
    /*is used to determine the maximum number of captured checkers among all possible moves
     *  and to remove any moves where the number of captured checkers is less than this maximum value
     */
    if(gameType == INTERNATIONAL)
    {
        int maxPossibleCaptures = 0;
        /*For each move, it checks the number of captured checkers.
         * If the number of captured checkers in the current move is greater
         * than max_deleted, then max_deleted is updated with this value.
        */
        for(unsigned i=0; i<possibleNextStatesCGame.size(); i++)
        {
            if( possibleNextStatesCGame.at(i)->getDeletedFiguresCount() > maxPossibleCaptures )
                maxPossibleCaptures = possibleNextStatesCGame.at(i)->getDeletedFiguresCount();
		}
        /*checks if the number of captured checkers in the current move is less than max_deleted.
         * If it is, then that move is deleted from moveSearch,
         * and the memory occupied by that object is freed.
         */
        for(unsigned i = 0; i < possibleNextStatesCGame.size(); i++)
        {
            if( possibleNextStatesCGame.at(i)->getDeletedFiguresCount() < maxPossibleCaptures )
            {
                delete possibleNextStatesCGame.at(i);
                possibleNextStatesCGame.erase( possibleNextStatesCGame.begin() + i );
				i--;
			}
		}
	}

    //processing all possible moves found in the game of checkers and preparing them for further use
    for(unsigned i=0; i < possibleNextStatesCGame.size(); i++)
    {
        std::vector<Point>::iterator it;
        for ( it = possibleNextStatesCGame.at(i)->getMovePath().begin() ; it < possibleNextStatesCGame.at(i)->getMovePath().end(); it++ )
        {
			if( it->type == DELETED )
                possibleNextStatesCGame.at(i)->at( it->x, it->y ) = 0;
		}
        state->nextPossibleStatesCState.push_back( possibleNextStatesCGame.at(i) );
	}

    possibleNextStatesCGame.clear();
}

int CheckersGame::countAvailableMoves(CheckersState * state, int i, int j)
{
    uint8 color = state->getFigureColor(i,j);
    int moves = 0;

    if(!color) return 0;

    int startDirectionIndex, endDirectionIndex;
    int stepIndex = 1;

    if(color == WHITE)
    {
        startDirectionIndex = 0;
        endDirectionIndex = 1;
    }
    else
    {
        startDirectionIndex = 2;
        endDirectionIndex = 3;
    }

    if( state->isQueen(i,j) )
    {
        startDirectionIndex = 0;
        endDirectionIndex = 3;
        stepIndex = squaresCountOnDiagonal;
    }
    for(char k = startDirectionIndex; k<=endDirectionIndex; k++)
    {
        for(char pk = 1; pk <= stepIndex; pk++)
        {
            char xi = i + pk*directionsX[(int)k];
            char xj = j + pk*directionsY[(int)k];
            if( !isCoordinateValid(xi) || !isCoordinateValid(xj) ) break;

            if(state->isNull(xi,xj)) moves ++;
            else break;
        }
    }

    bool captureflag;
    for(int k=0; k<=3; k++)
    {
        captureflag = false;
        for(char pk=1; pk <= stepIndex+1; pk++)
        {
            char xi = i + pk*directionsX[k];
            char xj = j + pk*directionsY[k];
            if( !isCoordinateValid(xi) || !isCoordinateValid(xj) )
                break;
            if( !captureflag && state->isNull(xi,xj) )
                continue;
            if( state->getFigureColor(xi,xj) == color || state->at(xi,xj) == MARKDELETED )
                break;
            if( !captureflag && state->getFigureColor(xi, xj)!= color )
            {
                captureflag = true;
                continue;
            }
            if(captureflag)
            {
                if( !state->isNull(xi,xj) )
                {
                    captureflag = false;
                    break;
                }
                moves++;
            }
        }
    }
    return moves;
}

int CheckersGame::searchMove(CheckersState *state, int i, int j, std::vector <Point> & vp)
{
    std::vector < std::vector <Point> > vectorOfPointsVectors;
    std::vector <Point> tmpPoints;
    Point point;
    int normMoves = 0;
    int delmoves = 0;

    uint8 color = state->getFigureColor(i,j);
    bool isking = state->isQueen(i,j);

    if(!color) return false;

    int startIndex;
    int endIndex;
    int pathIndex = 1;
    if(color == WHITE)
    {
        startIndex = 0;
        endIndex = 1;
    }
    if(color == BLACK)
    {
        startIndex = 2;
        endIndex = 3;
    }
    if( state->isQueen(i,j) )
    {
        startIndex = 0;
        endIndex = 3;
        pathIndex = squaresCountOnDiagonal;
    }

    for(char k=startIndex; k<=endIndex && !isCaptureFound; k++)
    {
        for(char pk=1; pk <= pathIndex; pk++)
        {
            char xi = i + pk*directionsX[(int)k];
            char xj = j + pk*directionsY[(int)k];
            if( !isCoordinateValid(xi) || !isCoordinateValid(xj) )
                break;
            if(state->isNull(xi,xj))
            {
                tmpPoints.clear();
                tmpPoints.push_back(Point(i,j,MOVEDFROM));
                if ( (color==BLACK && xj==0 && !isking) || (color==WHITE && xj==squaresCountOnDiagonal-1 && !isking)  )
                {
                    tmpPoints.push_back(Point(xi,xj,TOQUEEN));
                }
                else
                {
                    tmpPoints.push_back(Point(xi,xj,MOVEDTO));
                }
                vectorOfPointsVectors.push_back(tmpPoints);
                normMoves ++;
            }
            else break;
        }
    }
    bool captureflag;
    // проверка возможности боя
    for(int k=0; k<=3; k++)
    {
        captureflag = false;
        for(char pk=1; pk <= pathIndex+1; pk++)
        {
            char xi = i + pk*directionsX[k];
            char xj = j + pk*directionsY[k];
            if( !isCoordinateValid(xi) || !isCoordinateValid(xj) )
                break;
            if( !captureflag && state->isNull(xi,xj) )
                continue;
            // встретился на пути такой же цвет - не можем обойти и перепрыгнуть :(
            if( state->getFigureColor(xi,xj) == color || state->at(xi,xj) == MARKDELETED )
                break;
            // если не найдена сбитая фишка и в текущей позиции другая по цвету фишка
            if( !captureflag && state->getFigureColor(xi, xj)!= color )
            {
                captureflag = true;
                point.x = xi; point.y = xj; point.type = MARKDELETED;
                continue;
            }
            if(captureflag) {
                    // если непустая клетка то побить не можем
                if( !state->isNull(xi,xj) )
                {
                    captureflag = false;
                    break;
                }
                isCaptureFound = true;
                delmoves++;

                tmpPoints.clear();
                tmpPoints.push_back(Point(i,j,MOVEDFROM));
                tmpPoints.push_back(point);
                if ( (color==BLACK && xj==0 && !isking) || (color==WHITE && xj==squaresCountOnDiagonal-1 && !isking)  )
                    tmpPoints.push_back(Point(xi,xj,TOQUEEN));
                else
                    tmpPoints.push_back(Point(xi,xj,MOVEDTO));

                CheckersState *tmpstate = state->generateNextState(tmpPoints);

                std::vector <Point> history_vector = vp;
                if( vp.size() ) {
                    (history_vector.end()-1)->type = MOVEDTHROUGH;
                    (history_vector.end()-2)->type = DELETED;
                    tmpstate->setDeletedFiguresCount(state->getDeletedFiguresCount() + 1);
                } else {
                    history_vector.push_back( tmpPoints.at(0) );
                    tmpstate->setDeletedFiguresCount(1);
                }
                point.type = DELETED;
                history_vector.push_back( point );
                history_vector.push_back( tmpPoints.at(2) );

                tmpstate->setMovePath(history_vector);
                state->nextPossibleStatesCState.push_back(tmpstate);
                if(! searchMove(tmpstate, xi, xj, history_vector ))
                    possibleNextStatesCGame.push_back(tmpstate);
            }
        }
    }

    if(isCaptureFound) normMoves = 0;
    else
    {
        for(unsigned i=0; i<vectorOfPointsVectors.size(); i++) {
            CheckersState * tmpstate = state->generateNextState(vectorOfPointsVectors.at(i));
            state->nextPossibleStatesCState.push_back(tmpstate);
            possibleNextStatesCGame.push_back(tmpstate);
        }
    }
    return normMoves + delmoves;
}

void CheckersGame::performComputerMove()
{
    recursiveSearchMinimax(state, 1, -9999, 9999);
    int xMax = -9999;
	int id = 0;
    for(unsigned i=0; i<state->nextPossibleStatesCState.size(); i++)
    {
        if( state->nextPossibleStatesCState.at(i)->getMoveEvaluationScore() > xMax )
            xMax = state->nextPossibleStatesCState.at(i)->getMoveEvaluationScore();
	}

	std::vector <CheckersState *> tmp;
    for(unsigned i=0; i<state->nextPossibleStatesCState.size(); i++)
    {
        if( state->nextPossibleStatesCState.at(i)->getMoveEvaluationScore() == xMax )
            tmp.push_back( state->nextPossibleStatesCState.at(i) );
	}

	id = rand() % tmp.size();

    makeMove( tmp.at(id)->getMovePath().front(), tmp.at(id)->getMovePath().back() );
    findAndProcessPossibleMoves(state,humanColor);
}

int CheckersGame::recursiveSearchMinimax(CheckersState *state, int level, int alpha, int beta)
{
    bool max = true;
    uint8 color = humanColor;

    if(computerColor == WHITE) max = false;

    if(level % 2)
    {
        color = computerColor;
		max = !max;
	}
    if(level == searchDepth || checkTerminatePosition(state))
    {
        state->setMoveEvaluationScore(calculateGameStateScore(state));
        return state->getMoveEvaluationScore();
	}
    findAndProcessPossibleMoves(state, color);

    for(int i=0; i< state->nextPossibleStatesCState.size(); i++)
    {
        alpha = std::max( alpha, - recursiveSearchMinimax( state->nextPossibleStatesCState.at(i), level+1 , -beta, -alpha ) );

        if ( beta < alpha ) break;
	}
    int xMax=-9999, xMin=9999;
    for(unsigned j=0; j<state->nextPossibleStatesCState.size(); j++) {
        if(max)
        {
            if( state->nextPossibleStatesCState.at(j)->getMoveEvaluationScore() > xMax )
                xMax = state->nextPossibleStatesCState.at(j)->getMoveEvaluationScore();
        }
        else
        {
            if( state->nextPossibleStatesCState.at(j)->getMoveEvaluationScore() < xMin )
                xMin = state->nextPossibleStatesCState.at(j)->getMoveEvaluationScore();
		}
	}
	if(max)
        state->setMoveEvaluationScore(xMax);
	else
        state->setMoveEvaluationScore(xMin);
	return alpha;
}

bool CheckersGame::isCoordinateValid(char x)
{
    return (x >= 0 && x < squaresCountOnDiagonal);
}

void CheckersGame::calculatePieceStatistics(CheckersState * state)
{
	std::vector <CheckersState *> tmp;

    state->figuresCount.clear();
    state->figuresCount.resize(8,0);

	int movescount;
    for(unsigned i=0; i < squaresCountOnDiagonal; i++)
    {
        for(unsigned j=0; j < squaresCountOnDiagonal; j++)
        {
			if(i%2!=j%2)
				continue;
            movescount = countAvailableMoves(state, i, j);
            if(state->at(i,j)==WHITE)
            {
                state->figuresCount[0]++;
                state->figuresCount[2] += movescount;
			}
            if(state->at(i,j)==WHITEQUEEN)
            {
                state->figuresCount[1]++;
                state->figuresCount[2] += movescount;
			}
            if(state->at(i,j)==BLACK)
            {
                state->figuresCount[4]++;
                state->figuresCount[6] += movescount;
			}
            if(state->at(i,j)==BLACKQUEEN)
            {
                state->figuresCount[5]++;
                state->figuresCount[6] += movescount;
			}
		}
	}
	if(tmp.size()) {
        state->nextPossibleStatesCState = tmp;
	}
}

int CheckersGame::calculateGameStateScore(CheckersState * state)
{
    if(!state->figuresCount.size())
        calculatePieceStatistics(state);
	int evaluation = 0;
    evaluation += 4 * ( state->figuresCount[0] - state->figuresCount[4] );
    evaluation += 6 * ( state->figuresCount[1] - state->figuresCount[5] );
    evaluation += ( state->figuresCount[2] - state->figuresCount[6] );
	return evaluation;
}

bool CheckersGame::checkTerminatePosition(CheckersState * state)
{
    if(!state->figuresCount.size())
        calculatePieceStatistics(state);

    if ( !(state->figuresCount[0]+state->figuresCount[1]) ||
         !(state->figuresCount[4]+state->figuresCount[5]) )
		return true;

    if( !state->figuresCount[2] || !state->figuresCount[6] )
		return true;
	return false;
}

void CheckersGame::clearTree(CheckersState * state, bool clearlists, bool isOnlyChilds)
{
    if (isOnlyChilds)
    {
        for(unsigned i =0; i < state->nextPossibleStatesCState.size(); i++)
            clearTreeRecursive( state->nextPossibleStatesCState.at(i), clearlists );
        state->nextPossibleStatesCState.clear();
    }
    else clearTreeRecursive( state, clearlists );

}

void CheckersGame::clearTreeRecursive(CheckersState * state, bool clearlists)
{
    if( state->nextPossibleStatesCState.size() == 0 && clearlists )
    {
		delete state;
    }
    else
    {
        for(unsigned i =0; i < state->nextPossibleStatesCState.size(); i++)
            clearTreeRecursive( state->nextPossibleStatesCState.at(i), clearlists );
        state->nextPossibleStatesCState.clear();

        if(clearlists) delete state;
	}
}

void CheckersGame::setClicked(int i, int j)
{
    if(i>=0 && i<squaresCountOnDiagonal && j>=0 && j<squaresCountOnDiagonal && i%2==j%2 )
    {
        std::cout << "Clicked at " << (int)i << " " << (int)j << " = "<< (int)state->at(i,j) << std::endl;

        if(!isSecondClick) firstClick(i, j);
        else secondClick(i, j);
    }
    else
    {
		emit vectorDeleted();
        isSecondClick = false;
	}
}

void CheckersGame::firstClick(int i, int j)
{
    if( (humanColor == state->getFigureColor(i,j)))
    {
        std::vector < Point > possibleMovesVectorTMP;
        std::cout << "Click 1" << "\n"; std::cout.flush();
        clickPointCoordinates.x = i; clickPointCoordinates.y = j; clickPointCoordinates.type = MOVEDFROM;
        for(unsigned ii=0; ii< state->nextPossibleStatesCState.size(); ii++ )
        {
            if( state->nextPossibleStatesCState.at(ii)->getMovePath().at(0) == clickPointCoordinates )
            {
                for( unsigned jj=0; jj<state->nextPossibleStatesCState.at(ii)->getMovePath().size(); jj++ )
                {
                    possibleMovesVectorTMP.push_back( state->nextPossibleStatesCState.at(ii)->getMovePath().at(jj) );
				}
			}
		}
        if(possibleMovesVectorTMP.size()) {
            isSecondClick = true;
            emit vectorChanged(possibleMovesVectorTMP);
			return;
		}
	}
	emit vectorDeleted();
}

void CheckersGame::secondClick(int i, int j)
{
    std::cout << "Click 2" << "\n"; std::cout.flush();
	bool move = false;
    if ( (clickPointCoordinates.x == i && clickPointCoordinates.y == j) ) return;

    if( state->isNull(i,j) )
        move = this->makeMove( clickPointCoordinates , Point(i,j,MOVEDTO) );
    if( !move )
    {
        isSecondClick = false;
		firstClick(i, j);
    }
    else
    {
        performComputerMove();
        isSecondClick = false;
	}
}

bool CheckersGame::makeMove(Point p1, Point p2)
{
    for(unsigned i=0; i< state->nextPossibleStatesCState.size(); i++ )
    {
        if( state->nextPossibleStatesCState.at(i)->getMovePath().front() == p1 &&
            state->nextPossibleStatesCState.at(i)->getMovePath().back().x == p2.x &&
            state->nextPossibleStatesCState.at(i)->getMovePath().back().y == p2.y )
		{

            std::cout << "Move ok!\n"; std::cout.flush();
            CheckersState * tmpstate = state->nextPossibleStatesCState.at(i);
            state->nextPossibleStatesCState.erase(state->nextPossibleStatesCState.begin()+i);

            clearTree(state,true);
			clearTree(tmpstate,true,true);

            state = tmpstate;

            emit stateChanged(state);

			return true;
		}
	}
	return false;
}

