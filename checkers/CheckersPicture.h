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
	CheckersState * state();
	void setComputerColor(uint8 color);
public slots:
	void setState(CheckersState *state);
    void setVector(std::vector <Point> & v);
	void deleteVector();
	void setSize(int n);
	void clear();
signals:
	void mouseClicked(int, int);
protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);
	void resizeEvent (QResizeEvent * event);
private:
	QRect pixelRect(int i, int j) const;

	CheckersState * curstate;
    std::vector <Point> v;
	QPoint p;
	int side;
	int zoom;
	int n;
    int color;
};
