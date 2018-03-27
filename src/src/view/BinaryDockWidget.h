//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef BINARYCONTENTVIEW_H
#define BINARYCONTENTVIEW_H

#include <QDockWidget>
#include "src/widget/HexdumpWidget.h"
#include <libmoex/moex-view.h>

class BinaryDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit BinaryDockWidget(QWidget *parent = 0);
    void showViewData(moex::ViewData *data);

private:
    HexdumpWidget *hexEdit;
signals:

public slots:
};

#endif // BINARYCONTENTVIEW_H
