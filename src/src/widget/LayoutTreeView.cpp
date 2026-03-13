//
// Created by everettjf on 2018/3/29.
//

#include "LayoutTreeView.h"

#include <QKeyEvent>

LayoutTreeView::LayoutTreeView(QWidget *parent) :
        QTreeView(parent)
{

}
QSize LayoutTreeView::sizeHint() const
{
    return QSize(300, 0);
}

void LayoutTreeView::keyPressEvent(QKeyEvent *event)
{
    const QModelIndex before = currentIndex();
    QTreeView::keyPressEvent(event);

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right: {
        const QModelIndex after = currentIndex();
        if (after.isValid() && after == before) {
            emit keyboardNavigationActivated(after);
        }
        break;
    }
    case Qt::Key_Return:
    case Qt::Key_Enter: {
        const QModelIndex after = currentIndex();
        if (after.isValid()) {
            emit keyboardNavigationActivated(after);
        }
        break;
    }
    default:
        break;
    }
}
