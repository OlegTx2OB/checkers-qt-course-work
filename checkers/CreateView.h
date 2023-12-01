#pragma once
#include <QObject>
#include <QWidget>
#include <QGraphicsTextItem>

class CreateView
{
public:
    static QGraphicsTextItem* textView(QString title, int fontSize, QColor textColor, QGraphicsItem *parent);
};
