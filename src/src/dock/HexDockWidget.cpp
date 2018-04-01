//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "HexDockWidget.h"


#include "src/utility/Utility.h"

HexDockWidget::HexDockWidget(QWidget *parent) : QDockWidget(parent)
{
    setWindowTitle(tr("Hex"));

    hexEdit = new HexdumpWidget(this);
    hexEdit->setMinimumWidth(200);
    hexEdit->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);

    setWidget(hexEdit);
}

void HexDockWidget::showViewData(moex::BinaryViewData *data)
{
    if(!data){
        hexEdit->clearContent();
        return;
    }

    hexEdit->setAddressAs64Bit(true);
    hexEdit->loadAddress(data->start_value,(unsigned long long)data->offset,data->size);
}
void HexDockWidget::selectRange(uint64_t offset,uint64_t size)
{
    hexEdit->selectRange(offset,size);
}

void HexDockWidget::clearSelection()
{
    hexEdit->clearSelection();
}