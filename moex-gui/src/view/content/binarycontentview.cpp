//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "binarycontentview.h"


#include "../../utility/utility.h"
#include <QHBoxLayout>

BinaryContentView::BinaryContentView(QWidget *parent) : ContentViewInterface(parent)
{
    hexEdit = new QHexEdit(this);
    hexEdit->setOverwriteMode(true);
    hexEdit->setReadOnly(true);
    hexEdit->setAddressWidth(8);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);

    layout->addWidget(hexEdit);
    setLayout(layout);
}

void BinaryContentView::showViewData(moex::ViewData *data)
{
    moex::BinaryViewData *node = static_cast<moex::BinaryViewData*>(data);

    QByteArray bytes = QByteArray::fromRawData(node->offset,(int)node->size);
    hexEdit->setData(bytes);
    hexEdit->setAddressOffset(node->start_value);
}

