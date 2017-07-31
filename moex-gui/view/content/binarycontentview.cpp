//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "binarycontentview.h"


#include "../../utility/utility.h"
#include <QHBoxLayout>

BinaryContentView::BinaryContentView(QWidget *parent) : ContentViewInterface(parent)
{

}

void BinaryContentView::showViewData(moex::ViewData *data)
{
    moex::BinaryViewData *node = static_cast<moex::BinaryViewData*>(data);
    lazyInitUI();

    QByteArray bytes = QByteArray::fromRawData(node->offset,(int)node->size);
    hexEdit->setData(bytes);
}

void BinaryContentView::lazyInitUI()
{
    if(uiInited)return;
    uiInited = true;

    hexEdit = new QHexEdit(this);
    hexEdit->setOverwriteMode(true);
    hexEdit->setReadOnly(true);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);

    layout->addWidget(hexEdit);
    setLayout(layout);

}

