#include "CheckersGame.h"

CheckersGame::CheckersGame()
{
	setGameType(RUSSIAN);

	srand(time(NULL));
	cleared = 0;
	created = 0;
    currentCheckersState = NULL;
    firstCheckersState = NULL;
}

CheckersGame::~CheckersGame()
{
	endGame();
}


/*setting the game mode: Russian checkers or international.
 * in Russian there are 3 rows of checkers and a board 8 by 8,
 * and in international there are 4 rows and 10 by 10
 */
bool CheckersGame::setGameType(int type)
{
    if(type == RUSSIAN)
    {
		this->type = type;
        squaresCountOnDiagonal = 8; checkersRowsCount = 3;
		return true;
	}
    if(type == INTERNATIONAL)
    {
		this->type = type;
        squaresCountOnDiagonal = 10; checkersRowsCount = 4;
		return true;
	}
	return false;
}

//When installing more than 7 levels, the game starts to slow down
void CheckersGame::setSearchDepth(int level)
{
    if(level >=3 && level <=7) maxLevel = level;
    else maxLevel = 3;
}

void CheckersGame::startNewGame(int chosenComputerColor)
{
    computerColor = chosenComputerColor;
    if(chosenComputerColor == WHITE) humanColor = BLACK;
    else humanColor = WHITE;
    firstCheckersState = new CheckersState(squaresCountOnDiagonal);
    created++;


    for(int i=0; i<squaresCountOnDiagonal; i++)
    {
        for(int j=0; j<checkersRowsCount; j++)
        {
            if(i%2 == j%2)
                firstCheckersState->at(i,j) = WHITE;
        }
        for(int j=squaresCountOnDiagonal-checkersRowsCount; j<squaresCountOnDiagonal; j++)
        {
            if(i%2 == j%2)
                firstCheckersState->at(i,j) = BLACK;
        }
    }

    if(currentCheckersState)
        delete currentCheckersState;
    currentCheckersState = new CheckersState(firstCheckersState);
    historyStates.push_back(firstCheckersState);
    created++;
    click = 0;

    historyNum = 0;

    ix[0] = -1; ix[1] = 1; ix[2] = -1; ix[3] = 1;
    jx[0] = 1; jx[1] = 1; jx[2] = -1; jx[3] = -1;

    gameRunning = true;

    if(computerColor == WHITE) go();
    else
    {
        emit stateChanged(currentCheckersState);
        findAndProcessPossibleMoves(currentCheckersState,humanColor);
    }
}

void CheckersGame::endGame()
{
	cleared = 0;
	created = 0;

    gameRunning = false;
    if(currentCheckersState)
    {
        clearTree(currentCheckersState, true, true);
        currentCheckersState = NULL;
	}
    firstCheckersState = NULL;
	history.clear();
	for(unsigned i=0; i<historyStates.size(); i++)
		delete historyStates.at(i);
	historyStates.clear();
}

void CheckersGame::findAndProcessPossibleMoves(CheckersState * state, uint8 color)
{
    std::vector <Point> possibleMovePoints;
    captureFound = false;

    /*iterating over all the squares on the game board.
     *Inside this loop, it checks if the square is accessible
     *for a move for the current color of pieces*/
    for(unsigned i=0; i<squaresCountOnDiagonal; i++)
    {
        for(unsigned j=0; j<squaresCountOnDiagonal; j++)
        {
            if(i%2 == j%2 && state->color(i,j) == color)
            {
                searchMove(state, i, j, possibleMovePoints); //search moves from point(i, j)
                clearTree(state, false, true);
			}
		}
	}
    //If a capture is found, the list of possible moves is processed
    if(captureFound)
    {
        for(unsigned i=0; i<moveSearch.size(); i++)
        {
            if( moveSearch.at(i)->getXMove().size() == 2 )
            {
                delete moveSearch.at(i);
                moveSearch.erase( moveSearch.begin()+i );
				i--;
            }
            else break;
		}
	}
    /*is used to determine the maximum number of captured checkers among all possible moves
     *  and to remove any moves where the number of captured checkers is less than this maximum value
     */
    if(type == INTERNATIONAL)
    {
        int maxPossibleCaptures = 0;
        /*For each move, it checks the number of captured checkers.
         * If the number of captured checkers in the current move is greater
         * than max_deleted, then max_deleted is updated with this value.
        */
        for(unsigned i=0; i<moveSearch.size(); i++)
        {
            if( moveSearch.at(i)->getSetDeletedMove() > maxPossibleCaptures )
                maxPossibleCaptures = moveSearch.at(i)->getSetDeletedMove();
		}
        /*checks if the number of captured checkers in the current move is less than max_deleted.
         * If it is, then that move is deleted from moveSearch,
         * and the memory occupied by that object is freed.
         */
        for(unsigned i = 0; i < moveSearch.size(); i++)
        {
            if( moveSearch.at(i)->getSetDeletedMove() < maxPossibleCaptures )
            {
                delete moveSearch.at(i);
                moveSearch.erase( moveSearch.begin() + i );
				i--;
			}
		}
	}

    //processing all possible moves found in the game of checkers and preparing them for further use
    for(unsigned i=0; i < moveSearch.size(); i++)
    {
        std::vector<Point>::iterator it;
        for ( it=moveSearch.at(i)->getXMove().begin() ; it < moveSearch.at(i)->getXMove().end(); it++ )
        {
			if( it->type == DELETED )
                moveSearch.at(i)->at( it->x, it->y ) = 0;
		}
        moveSearch.at(i)->setParent(state);
        state->getXChild().push_back( moveSearch.at(i) );
	}

    moveSearch.clear();
}

int CheckersGame::countAvailableMoves(CheckersState * state, int i, int j)
{
    uint8 color = state->color(i,j);
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
            char xi = i + pk*ix[(int)k];
            char xj = j + pk*jx[(int)k];
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
            char xi = i + pk*ix[k];
            char xj = j + pk*jx[k];
            if( !isCoordinateValid(xi) || !isCoordinateValid(xj) )
                break;
            if( !captureflag && state->isNull(xi,xj) )
                continue;
            if( state->color(xi,xj) == color || state->at(xi,xj) == MARKDELETED )
                break;
            if( !captureflag && state->color(xi, xj)!= color )
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
    std::vector < std::vector <Point> > vpp;
    std::vector <Point> tmp_vp;
    Point cp;
    int normmoves = 0, delmoves = 0;

    uint8 color = state->color(i,j);
    bool isking = state->isQueen(i,j);
    if(!color)
        return false;

    int sidx, eidx, pidx = 1;
    if(color == WHITE)
    {
        sidx = 0; eidx = 1;
    }
    if(color == BLACK)
    {
        sidx = 2; eidx = 3;
    }
    if( state->isQueen(i,j) )
    {
        sidx = 0; eidx = 3; pidx = squaresCountOnDiagonal;
    }

    for(char k=sidx; k<=eidx && !captureFound; k++) {
        for(char pk=1; pk <= pidx; pk++) {
            char xi = i + pk*ix[(int)k];
            char xj = j + pk*jx[(int)k];
            if( !isCoordinateValid(xi) || !isCoordinateValid(xj) )
                break;
            if(state->isNull(xi,xj))
            {
                tmp_vp.clear();
                tmp_vp.push_back(Point(i,j,MOVEDFROM));
                if ( (color==BLACK && xj==0 && !isking) || (color==WHITE && xj==squaresCountOnDiagonal-1 && !isking)  ) {
                    tmp_vp.push_back(Point(xi,xj,TOKING));
                } else {
                    tmp_vp.push_back(Point(xi,xj,MOVEDTO));
                }
                vpp.push_back(tmp_vp);
                normmoves ++;
            } else break;
        }
    }
    bool captureflag;
    // проверка возможности боя
    for(int k=0; k<=3; k++) {
        captureflag = false;
        for(char pk=1; pk <= pidx+1; pk++) {
            char xi = i + pk*ix[k];
            char xj = j + pk*jx[k];
            if( !isCoordinateValid(xi) || !isCoordinateValid(xj) )
                break;
            if( !captureflag && state->isNull(xi,xj) )
                continue;
            // встретился на пути такой же цвет - не можем обойти и перепрыгнуть :(
            if( state->color(xi,xj) == color || state->at(xi,xj) == MARKDELETED )
                break;
            // если не найдена сбитая фишка и в текущей позиции другая по цвету фишка
            if( !captureflag && state->color(xi, xj)!= color )
            {
                captureflag = true;
                cp.x = xi; cp.y = xj; cp.type = MARKDELETED;
                continue;
            }
            if(captureflag) {
                    // если непустая клетка то побить не можем
                if( !state->isNull(xi,xj) )
                {
                    captureflag = false;
                    break;
                }
                captureFound = true;
                delmoves++;

                tmp_vp.clear();
                tmp_vp.push_back(Point(i,j,MOVEDFROM));
                tmp_vp.push_back(cp);
                if ( (color==BLACK && xj==0 && !isking) || (color==WHITE && xj==squaresCountOnDiagonal-1 && !isking)  ) // 检查
                    tmp_vp.push_back(Point(xi,xj,TOKING));
                else
                    tmp_vp.push_back(Point(xi,xj,MOVEDTO));

                CheckersState *tmpstate = state->genNextState(tmp_vp);
                created++;

                std::vector <Point> history_vector = vp;
                if( vp.size() ) {
                    (history_vector.end()-1)->type = MOVEDTHROUGH;
                    (history_vector.end()-2)->type = DELETED;
                    tmpstate->getSetDeletedMove() = state->getSetDeletedMove()+1;
                } else {
                    history_vector.push_back( tmp_vp.at(0) );
                    tmpstate->getSetDeletedMove() = 1;
                }
                cp.type = DELETED;
                history_vector.push_back( cp );
                history_vector.push_back( tmp_vp.at(2) );

                tmpstate->getXMove() = history_vector;
                tmpstate->setParent(state);
                state->getXChild().push_back(tmpstate);
                if(! searchMove(tmpstate, xi, xj, history_vector ))
                    moveSearch.push_back(tmpstate);
            }
        }
    }

    if(captureFound)
        normmoves = 0;
    else
    {
        for(unsigned i=0; i<vpp.size(); i++) {
            CheckersState * tmpstate = state->genNextState(vpp.at(i));
            created++;
            tmpstate->setParent(state);
            state->getXChild().push_back(tmpstate);
            moveSearch.push_back(tmpstate);
        }
    }
    return normmoves + delmoves;
}

void CheckersGame::go()
{
    goRecursive(currentCheckersState, 1, -9999, 9999);
    int xMax = -9999;
	int id = 0;
    for(unsigned i=0; i<currentCheckersState->getXChild().size(); i++)
    {
        if( currentCheckersState->getXChild().at(i)->getScore() > xMax )
            xMax = currentCheckersState->getXChild().at(i)->getScore();
	}

	std::vector <CheckersState *> tmp;
    for(unsigned i=0; i<currentCheckersState->getXChild().size(); i++)
    {
        if( currentCheckersState->getXChild().at(i)->getScore() == xMax )
            tmp.push_back( currentCheckersState->getXChild().at(i) );
	}

	id = rand() % tmp.size();

    makeMove( tmp.at(id)->getXMove().front(), tmp.at(id)->getXMove().back() );
    findAndProcessPossibleMoves(currentCheckersState,humanColor);
}

int CheckersGame::goRecursive(CheckersState *state, int level, int alpha, int beta)
{
    bool max;
    if(computerColor == WHITE) max = false;
    else max = true;


    uint8 color = humanColor;
    if(level % 2)
    {
        color = computerColor;
		max = !max;
	}
    if(level == maxLevel || checkTerminatePosition(state))
    {
        state->getScore() = evaluation(state);
        return state->getScore();
	}
    findAndProcessPossibleMoves(state, color);

    for(int i=0; i< state->getXChild().size(); i++)
    {
        alpha = std::max( alpha, - goRecursive( state->getXChild().at(i), level+1 , -beta, -alpha ) );

        if ( beta < alpha ) break;
	}
    int xMax=-9999, xMin=9999;
    for(unsigned j=0; j<state->getXChild().size(); j++) {
        if(max)
        {
            if( state->getXChild().at(j)->getScore() > xMax )
                xMax = state->getXChild().at(j)->getScore();
        }
        else
        {
            if( state->getXChild().at(j)->getScore() < xMin )
                xMin = state->getXChild().at(j)->getScore();
		}
	}
	if(max)
        state->getScore() = xMax;
	else
        state->getScore() = xMin;
	return alpha;
}

bool CheckersGame::isCoordinateValid(char x)
{
    return (x >= 0 && x < squaresCountOnDiagonal);
}

void CheckersGame::calcCounts(CheckersState * state)
{
	std::vector <CheckersState *> tmp;

    state->getXCount().clear();
    state->getXCount().resize(8,0);

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
                state->getXCount()[0]++;
                state->getXCount()[2] += movescount;
			}
            if(state->at(i,j)==WHITEQUEEN)
            {
                state->getXCount()[1]++;
                state->getXCount()[2] += movescount;
			}
            if(state->at(i,j)==BLACK)
            {
                state->getXCount()[4]++;
                state->getXCount()[6] += movescount;
			}
            if(state->at(i,j)==BLACKQUEEN)
            {
                state->getXCount()[5]++;
                state->getXCount()[6] += movescount;
			}
		}
	}
	if(tmp.size()) {
        state->getXChild() = tmp;
	}
}

int CheckersGame::evaluation(CheckersState * state)
{
    if(!state->getXCount().size())
		calcCounts(state);
	int evaluation = 0;
    evaluation += 4 * ( state->getXCount()[0] - state->getXCount()[4] );
    evaluation += 6 * ( state->getXCount()[1] - state->getXCount()[5] );
    evaluation += ( state->getXCount()[2] - state->getXCount()[6] );
	return evaluation;
}

bool CheckersGame::checkTerminatePosition(CheckersState * state)
{
    if(!state->getXCount().size())
		calcCounts(state);
    if ( !(state->getXCount()[0]+state->getXCount()[1]) ||
         !(state->getXCount()[4]+state->getXCount()[5]) )
		return true;

    if( !state->getXCount()[2] || !state->getXCount()[6] )
		return true;
	return false;
}

uint8 CheckersGame::whoWin(CheckersState *state)
{
    if ( !(state->getXCount()[0]+state->getXCount()[1]) || !state->getXCount()[2] )
        return BLACK;
    if ( !(state->getXCount()[4]+state->getXCount()[5]) || !state->getXCount()[6] )
        return WHITE;
	return -1;
}

void CheckersGame::clearTree(CheckersState * state, bool clearlists, bool onlychilds)
{
    if (onlychilds)
    {
        for(unsigned i =0; i < state->getXChild().size(); i++)
            clearTreeRecursive( state->getXChild().at(i), clearlists );
        state->getXChild().clear();
    }
    else clearTreeRecursive( state, clearlists );

}

void CheckersGame::clearTreeRecursive(CheckersState * state, bool clearlists)
{
    if( state->getXChild().size() == 0 && clearlists )
    {
		cleared ++;
		delete state;
    }
    else
    {
        for(unsigned i =0; i < state->getXChild().size(); i++)
            clearTreeRecursive( state->getXChild().at(i), clearlists );
        state->getXChild().clear();
        if(clearlists)
        {
			cleared ++;
			delete state;
		}
	}
}

void CheckersGame::printPointVector(std::vector <Point> & v)
{
    for(unsigned i=0; i<v.size(); i++)
		std::cout << (int)v.at(i).x << " " << (int)v.at(i).y << " " << (int)v.at(i).type << "\n";
	std::cout.flush();
}

void CheckersGame::setClicked(int i, int j)
{
    if(i>=0 && i<squaresCountOnDiagonal && j>=0 && j<squaresCountOnDiagonal && i%2==j%2 && gameRunning)
    {
        std::cout << "Clicked at " << (int)i << " " << (int)j << " = "<< (int)currentCheckersState->at(i,j) << std::endl;

        if(click == 0) firstClick(i, j);
        else secondClick(i, j);
    }
    else
    {
		emit vectorDeleted();
		click = 0;
	}
}

void CheckersGame::firstClick(int i, int j)
{
    if( (humanColor == currentCheckersState->color(i,j)))
    {
		std::cout << "Click 0" << "\n"; std::cout.flush();
        tmpPoint.x = i; tmpPoint.y = j; tmpPoint.type = MOVEDFROM;
        tmpVector.clear();
        for(unsigned ii=0; ii< currentCheckersState->getXChild().size(); ii++ )
        {
            if( currentCheckersState->getXChild().at(ii)->getXMove().at(0) == tmpPoint )
            {
                for( unsigned jj=0; jj<currentCheckersState->getXChild().at(ii)->getXMove().size(); jj++ )
                {
                    tmpVector.push_back( currentCheckersState->getXChild().at(ii)->getXMove().at(jj) );
				}
			}
		}
        if(tmpVector.size()) {
			click = 1;
            emit vectorChanged(tmpVector);
			return;
		}
	}
	emit vectorDeleted();
}

void CheckersGame::secondClick(int i, int j)
{
	std::cout << "Click 1" << "\n"; std::cout.flush();
	bool move = false;
    if ( (tmpPoint.x == i && tmpPoint.y == j) ) return;

    if( currentCheckersState->isNull(i,j) )
        move = this->makeMove( tmpPoint , Point(i,j,MOVEDTO) );
    if( !move )
    {
		click =0;
		firstClick(i, j);
    }
    else
    {
        if(gameRunning) go();
		click = 0;
	}
}

bool CheckersGame::makeMove(Point p1, Point p2)
{
    for(unsigned i=0; i< currentCheckersState->getXChild().size(); i++ )
    {
        if( currentCheckersState->getXChild().at(i)->getXMove().front() == p1 &&
            currentCheckersState->getXChild().at(i)->getXMove().back().x == p2.x &&
            currentCheckersState->getXChild().at(i)->getXMove().back().y == p2.y )
		{

            std::cout << "Move ok!\n"; std::cout.flush();
            CheckersState * tmpstate = currentCheckersState->getXChild().at(i);
            currentCheckersState->getXChild().erase(currentCheckersState->getXChild().begin()+i);

            clearTree(currentCheckersState,true);
			clearTree(tmpstate,true,true);

            currentCheckersState = tmpstate;
            history.push_back( currentCheckersState->getXMove() );
            tmpCheckersState = new CheckersState(currentCheckersState);
            tmpCheckersState->getXChild().clear();
            historyStates.push_back( tmpCheckersState );
            emit stateChanged(currentCheckersState);
            historyNum = historyStates.size()-1;

            if(checkTerminatePosition(currentCheckersState))
            {
                emit gameEnded( whoWin(currentCheckersState) );
                gameRunning = false;
			}
			return true;
		}
	}
	return false;
}

