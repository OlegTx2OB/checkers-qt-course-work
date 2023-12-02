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

    zoom = 32;
	curstate = NULL;
	n = 10;
    setMinimumSize(zoom * n,zoom * n);
}

CheckersPicture::~CheckersPicture()
{

}

void CheckersPicture::setComputerColor(uint8 color)
{
	this->color = color;
}

void CheckersPicture::setState(CheckersState * state) {
    if(state)
    {
		curstate = state;
        n = state->getSquaresCountByDiagonal();
		v.clear();
		update();
    }
    else
    {
		clear();
	}
}

void CheckersPicture::setSize(int n) {
	this->n = n;
	update();
}

void CheckersPicture::deleteVector()
{
    if(v.size())
    {
		v.clear();
		update();
	}
}

void CheckersPicture::setVector(std::vector <Point> & v) {
	this->v = v;
	update();
}

void CheckersPicture::clear() {
	curstate = NULL;
	v.clear();
	update();
}

void CheckersPicture::mousePressEvent(QMouseEvent *event)
{

	if (event->buttons() && Qt::LeftButton) {
		qreal i = (event->pos().x() - p.x() + side/(2*n+2))*(n+1)/side - 1.0;
		qreal j = (double)n - (event->pos().y() - p.y() + side/(2*n+2))*(n+1)/side;
		if(color==BLACK)
			emit mouseClicked((int)i,(int)j);
		else
			emit mouseClicked(n -1 - (int)i, n - 1 - (int)j);
	}
}

void CheckersPicture::mouseMoveEvent(QMouseEvent *event)
{

}

void CheckersPicture::paintEvent(QPaintEvent *event)
{
	qDebug() << "CheckersPicture::paintEvent()";

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setViewport(p.x(),p.y(),side,side);
	painter.setWindow(0, 0, zoom*(n+1.0), zoom*(n+1.0));

    QColor border(88, 62, 30);
	painter.fillRect(QRect(0,0,zoom*(n+1.0),zoom*0.5), border);
	painter.fillRect(QRect(0,zoom*(n+0.5),zoom*(n+1.0),zoom*0.5), border);
	painter.fillRect(QRect(0,0,zoom*(0.5),zoom*(n+1.0)), border);
	painter.fillRect(QRect(zoom*(n+0.5),0,zoom*0.5,zoom*(n+1.0)), border);

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

	int ix,jx;
    QColor endColor(100,100,100);
    QColor startColor(0x33,0xff,0x00);
    QColor capturedColor(0xff,0x33,0x33);
    QColor normalColor(0x4c,0x4c,0xcc);
	if(v.size()) {
		int type;
		for(unsigned i=0; i< v.size(); i++) {
			color==WHITE ? jx = n-1-v.at(i).y : jx = v.at(i).y;
			color==WHITE ? ix = n-1-v.at(i).x : ix = v.at(i).x;
			QRect rect = pixelRect(ix, jx);
			type = v.at(i).type;
			if(type == MOVEDFROM)
				painter.fillRect(rect, startColor);
			else if(type == MOVEDTO || type == TOKING)
				painter.fillRect(rect, endColor);
			else if(type == MOVEDTHROUGH)
				painter.fillRect(rect, normalColor);
			else if(type == DELETED)
				painter.fillRect(rect, capturedColor);
		}
	}

	int s = zoom*0.4;
	int sd = zoom*0.2;
	if(curstate) {
		painter.setPen(QPen(Qt::black,zoom*0.1));
		painter.setBrush(QBrush(Qt::white));
		for(int i=0; i<n; i++) {
			for(int j=0; j<n; j++) {
				color==WHITE ? jx = j+1 : jx = n-j;
				color==WHITE? ix = n-i : ix = i+1;
				if(curstate->at(i,j)==WHITE)
					painter.drawEllipse(QPoint(zoom*(ix),zoom*(jx)),s,s);
                if(curstate->at(i,j)==WHITEQUEEN) {
					painter.drawEllipse(QPoint(zoom*(ix),zoom*(jx)),s,s);
					painter.drawEllipse(QPoint(zoom*(ix),zoom*(jx)),sd,sd);
				}
			}
		}
		painter.setBrush(QBrush(Qt::black));
		for(int i=0; i<n; i++) {
			for(int j=0; j<n; j++) {
				color==WHITE ? jx = j+1 : jx = n-j;
				color==WHITE ? ix = n-i : ix = i+1;
				if(curstate->at(i,j)==BLACK)
					painter.drawEllipse(QPoint(zoom*(ix),zoom*(jx)),s,s);
                if(curstate->at(i,j)==BLACKQUEEN) {
					painter.drawEllipse(QPoint(zoom*(ix),zoom*(jx)),s,s);
					painter.setPen(QPen(Qt::white,zoom*0.1));
					painter.drawEllipse(QPoint(zoom*(ix),zoom*(jx)),sd,sd);
					painter.setPen(QPen(Qt::black,zoom*0.1));
				}
			}
		}
	}
}

void CheckersPicture::resizeEvent (QResizeEvent * event) {
	if(event->oldSize()!=event->size()) {
		update();
		side = qMin(width(), height());
		p = QPoint((width() - side) / 2, (height() - side) / 2);
	} else {
		event->ignore();
	}
}

QRect CheckersPicture::pixelRect(int i, int j) const
{
    return QRect(zoom * i + zoom*0.5, zoom*(n-0.5) - zoom * j, zoom, zoom);
}


