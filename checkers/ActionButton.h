#pragma once
#include <QObject>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QBrush>
#include <QGraphicsRectItem>
#include <QFont>
#include "CreateView.h"

class ActionButton: public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    ActionButton(QString title, int width, int height);

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

signals:
    void buttonPressed();
};
