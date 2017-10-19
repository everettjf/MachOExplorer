//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "mytreeview.h"

MyTreeView::MyTreeView(QWidget *parent) : QWidget(parent)
{

}

QSize MyTreeView::sizeHint() const
{
    return QSize(300,0);
}
