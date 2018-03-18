//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef CONTENTVIEWINTERFACE_H
#define CONTENTVIEWINTERFACE_H

#include <libmoex/moex-view.h>
#include <QWidget>
#include "src/controller/WindowShareData.h"

class ContentViewInterface : public QWidget, public WindowSharedData{
    Q_OBJECT
public:
    explicit ContentViewInterface(QWidget *parent = 0):QWidget(parent){}
    virtual void showViewData(moex::ViewData *data) = 0;
};

#endif // CONTENTVIEWINTERFACE_H
