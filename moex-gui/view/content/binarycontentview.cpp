//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "binarycontentview.h"


#include "../../utility/utility.h"
#include <QHBoxLayout>

BinaryContentView::BinaryContentView(QWidget *parent) : QWidget(parent)
{
    hexEdit = new QHexEdit(this);
    hexEdit->setOverwriteMode(true);
    hexEdit->setReadOnly(true);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);

    layout->addWidget(hexEdit);
    setLayout(layout);

}


void BinaryContentView::showNode(moex::BinaryViewData *node)
{
    QByteArray data = QByteArray::fromRawData(node->offset,(int)node->size);
    hexEdit->setData(data);
}

