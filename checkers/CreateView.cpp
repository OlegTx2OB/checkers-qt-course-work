#include "CreateView.h"

QGraphicsTextItem* CreateView::textView(QString title, int fontSize, QColor textColor, QGraphicsItem *parent)
{
    QGraphicsTextItem *textView = new QGraphicsTextItem(title, parent);
    QFont titleFont("Dubai Light", fontSize);
    textView->setDefaultTextColor(textColor);
    textView->setFont(titleFont);

    return textView;
}
