#pragma once
#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QResizeEvent>
#include <QPaintEvent>

#include "CheckersState.h"

class CheckersPicture : public QWidget
{
	Q_OBJECT
public:
    CheckersPicture(QWidget * parent = 0);
	~CheckersPicture();
    void setComputerColor(uint8 computerColor);

public slots:
	void setState(CheckersState *state);
   void setPossibleMovesVector(std::vector <Point> & possibleMovesVector);
    void deletePossibleMovesVector();
	void clear();
signals:
	void mouseClicked(int, int);

protected:
	void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
	void resizeEvent (QResizeEvent * event);

    void displayBorder(QPainter& painter);
    void displayBoard(QPainter& painter);
    void displayActiveBoardSquares(QPainter & painter);
    void displayFigures(QPainter & painter);


private:
	QRect pixelRect(int i, int j) const;

    CheckersState * currState;
    std::vector <Point> possibleMovesVector;
    QPoint qPoint;
    int boardSizeInPixels;
    int displayScale;
    //squaresCountByDiagonal (but this name is too much big)
    int n;
    int computerColor;
};
