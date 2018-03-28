//
// Created by everettjf on 2018/3/29.
//

#include "LayoutTreeView.h"


LayoutTreeView::LayoutTreeView(QWidget *parent) :
        QTreeView(parent)
{

}
QSize LayoutTreeView::sizeHint() const
{
    return QSize(300, 0);
}