#include "ActionButton.h"

ActionButton::ActionButton(QString title, int width, int height)
{
    setRect(0, 0, width, height);

    QColor backgroundColor = QColor(234, 186, 102);
    QBrush brush;
    brush.setStyle((Qt::SolidPattern));
    brush.setColor(backgroundColor);
    this->setBrush(brush);

    QColor textColor = QColor(39, 62, 58);
    QGraphicsTextItem *text = CreateView::textView(title, 20, textColor, this);
    double xPosition = rect().width()/2 - text->boundingRect().width()/2;
    double yPosition = rect().height()/2 - text->boundingRect().height()/2;
    text->setPos(xPosition, yPosition);

    setAcceptHoverEvents(true);
}

void ActionButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit buttonPressed();
}

void ActionButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QColor backgroundColor = QColor(218, 165, 32);
    QBrush brush;
    brush.setStyle((Qt::SolidPattern));
    brush.setColor(backgroundColor);
    this->setBrush(brush);
}

void ActionButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QColor backgroundColor = QColor(234, 186, 102);
    QBrush brush;
    brush.setStyle((Qt::SolidPattern));
    brush.setColor(backgroundColor);
    this->setBrush(brush);
}
