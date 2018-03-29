//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef CONTENTVIEW_H
#define CONTENTVIEW_H

#include <QDockWidget>
#include <QTabWidget>
#include <QStackedWidget>

#include "src/widget/TableInfoWidget.h"
#include "src/dock/HexDockWidget.h"
#include <libmoex/moex.h>
#include <libmoex/moex-view.h>
#include <vector>
#include <map>

class CentralWidget : public QWidget
{
    Q_OBJECT
public:
    TableInfoWidget *table;
public:
    explicit CentralWidget(QWidget *parent = 0);

    void showTableViewData(moex::TableViewData *data);

};

#endif // CONTENTVIEW_H
