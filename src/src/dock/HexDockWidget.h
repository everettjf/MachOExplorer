//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef BINARYCONTENTVIEW_H
#define BINARYCONTENTVIEW_H

#include <QDockWidget>
#include "src/widget/HexdumpWidget.h"
#include <libmoex/moex-view.h>

class HexDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit HexDockWidget(QWidget *parent = 0);
    void showViewData(moex::BinaryViewData *data);

    void selectRange(uint64_t offset,uint64_t size);
    void clearSelection();

private:
    HexdumpWidget *hexEdit;
signals:

public slots:
};

#endif // BINARYCONTENTVIEW_H
