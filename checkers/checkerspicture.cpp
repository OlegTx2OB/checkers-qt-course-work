#include "CheckersPicture.h"

CheckersPicture::CheckersPicture(QWidget *parent) : QWidget(parent)
{
	QPalette palette;
    palette.setColor(QPalette::Light, QColor(39, 62, 58));
	setPalette(palette);
	setBackgroundRole(QPalette::Light);
	setAutoFillBackground(true);

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setAttribute(Qt::WA_StaticContents);

    displayScale = 32;
    currState = NULL;
	n = 10;
    setMinimumSize(displayScale * n,displayScale * n);
}

CheckersPicture::~CheckersPicture() {}

void CheckersPicture::setComputerColor(uint8 color)
{
    this->computerColor = color;
}

void CheckersPicture::setState(CheckersState * state)
{
    if(state)
    {
        currState = state;
        n = state->getSquaresCountByDiagonal();
        possibleMovesVector.clear();
		update();
    }
    else clear();
}

void CheckersPicture::deletePossibleMovesVector()
{
    if(possibleMovesVector.size())
    {
       possibleMovesVector.clear();
        update();
	}
}

void CheckersPicture::clear()
{
    currState = NULL;
    possibleMovesVector.clear();
	update();
}

void CheckersPicture::mousePressEvent(QMouseEvent *event)
{

    if (event->buttons() && Qt::LeftButton)
    {
        qreal i = (event->pos().x() - qPoint.x() + boardSizeInPixels/(2*n+2))*(n+1)/boardSizeInPixels - 1.0;
        qreal j = (double)n - (event->pos().y() - qPoint.y() + boardSizeInPixels/(2*n+2))*(n+1)/boardSizeInPixels;
        if(computerColor==BLACK)
			emit mouseClicked((int)i,(int)j);
		else
			emit mouseClicked(n -1 - (int)i, n - 1 - (int)j);
	}
}

void CheckersPicture::displayBorder(QPainter & painter)
{
    QColor border(88, 62, 30);
    painter.fillRect(QRect(0,0,displayScale*(n+1.0),displayScale*0.5), border);
    painter.fillRect(QRect(0,displayScale*(n+0.5),displayScale*(n+1.0),displayScale*0.5), border);
    painter.fillRect(QRect(0,0,displayScale*(0.5),displayScale*(n+1.0)), border);
    painter.fillRect(QRect(displayScale*(n+0.5),0,displayScale*0.5,displayScale*(n+1.0)), border);
}
void CheckersPicture::displayBoard(QPainter & painter)
{
    QColor darkSquareColor(234, 186, 102);
    QColor lightSquareColor(255, 255, 220);

    for(int i=0; i<n; i++)
    {
        for(int j=0; j<n; j++)
        {
            QRect rect = pixelRect(i, j);
            if( !((i+j%2)%2) ) painter.fillRect(rect, darkSquareColor);
            else painter.fillRect(rect, lightSquareColor);
        }
    }
}


void CheckersPicture::displayFigures(QPainter &painter)
{
    int xCoordinate;
    int yCoordinate;



    int s = displayScale*0.4;
    int sd = displayScale*0.2;
    if(currState) {
        painter.setPen(QPen(Qt::black,displayScale*0.1));
        painter.setBrush(QBrush(Qt::white));
        for(int i=0; i<n; i++)
        {
            for(int j=0; j<n; j++)
            {
                computerColor==WHITE ? yCoordinate = j+1 : yCoordinate = n-j;
                computerColor==WHITE? xCoordinate = n-i : xCoordinate = i+1;
                if(currState->at(i,j)==WHITE)
                    painter.drawEllipse(QPoint(displayScale*(xCoordinate),displayScale*(yCoordinate)),s,s);
                if(currState->at(i,j)==WHITEQUEEN)
                {
                    painter.drawEllipse(QPoint(displayScale*(xCoordinate),displayScale*(yCoordinate)),s,s);
                    painter.drawEllipse(QPoint(displayScale*(xCoordinate),displayScale*(yCoordinate)),sd,sd);
                }
            }
        }
        painter.setBrush(QBrush(Qt::black));
        for(int i=0; i<n; i++)
        {
            for(int j=0; j<n; j++)
            {
                computerColor==WHITE ? yCoordinate = j+1 : yCoordinate = n-j;
                computerColor==WHITE ? xCoordinate = n-i : xCoordinate = i+1;
                if(currState->at(i,j)==BLACK)
                    painter.drawEllipse(QPoint(displayScale*(xCoordinate),displayScale*(yCoordinate)),s,s);
                if(currState->at(i,j)==BLACKQUEEN)
                {
                    painter.drawEllipse(QPoint(displayScale*(xCoordinate),displayScale*(yCoordinate)),s,s);
                    painter.setPen(QPen(Qt::white,displayScale*0.1));
                    painter.drawEllipse(QPoint(displayScale*(xCoordinate),displayScale*(yCoordinate)),sd,sd);
                    painter.setPen(QPen(Qt::black,displayScale*0.1));
                }
            }
        }
    }
}
void CheckersPicture::displayActiveBoardSquares(QPainter& painter)
{
    int xCoordinate;
    int yCoordinate;

    QColor endColor(100,100,100);
    QColor startColor(0x33,0xff,0x00);
    QColor capturedColor(0xff,0x33,0x33);
    QColor normalColor(0x4c,0x4c,0xcc);

    if(true)
    {
        int type;
        for(unsigned i=0; i< possibleMovesVector.size(); i++)
        {
            computerColor == WHITE ? yCoordinate = n-1-possibleMovesVector.at(i).y : yCoordinate = possibleMovesVector.at(i).y;
            computerColor == WHITE ? xCoordinate = n-1-possibleMovesVector.at(i).x : xCoordinate = possibleMovesVector.at(i).x;

            QRect rect = pixelRect(xCoordinate, yCoordinate);
            type = possibleMovesVector.at(i).type;
            if(type == MOVEDFROM)
                painter.fillRect(rect, startColor);
            else if(type == MOVEDTO || type == TOQUEEN)
                painter.fillRect(rect, endColor);
            else if(type == MOVEDTHROUGH)
                painter.fillRect(rect, normalColor);
            else if(type == DELETED)
                painter.fillRect(rect, capturedColor);
        }
    }
}

void CheckersPicture::paintEvent(QPaintEvent *event)
{
	qDebug() << "CheckersPicture::paintEvent()";

	QPainter painter(this);

    //Antialiasing true
    painter.setRenderHint(QPainter::Antialiasing, true);
    //set Window
    painter.setViewport(qPoint.x(),qPoint.y(),boardSizeInPixels,boardSizeInPixels);
    painter.setWindow(0, 0, displayScale*(n+1.0), displayScale*(n+1.0));



    displayBorder(painter);
    displayBoard(painter);

    displayActiveBoardSquares(painter);
    displayFigures(painter);

}

void CheckersPicture::resizeEvent (QResizeEvent * event)
{
    if(event->oldSize()!=event->size())
    {
        update();
        boardSizeInPixels = qMin(width(), height());
        qPoint = QPoint((width() - boardSizeInPixels) / 2, (height() - boardSizeInPixels) / 2);
    }
    else event->ignore();

}

QRect CheckersPicture::pixelRect(int i, int j) const
{
    return QRect(displayScale * i + displayScale*0.5, displayScale*(n-0.5) - displayScale * j, displayScale, displayScale);
}


