//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "BinaryContentView.h"


#include "src/utility/Utility.h"
#include <QHBoxLayout>

BinaryContentView::BinaryContentView(QWidget *parent) : ContentViewInterface(parent)
{
    hexEdit = new HexdumpWidget(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);

    layout->addWidget(hexEdit);
    setLayout(layout);
}

void BinaryContentView::showViewData(moex::ViewData *data)
{
    moex::BinaryViewData *node = static_cast<moex::BinaryViewData*>(data);

    hexEdit->setAddressAs64Bit(true);
    hexEdit->loadAddress(node->start_value,(unsigned long long)node->offset,node->size);
}

