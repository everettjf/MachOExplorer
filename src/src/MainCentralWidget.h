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

class MainCentralWidget : public QWidget
{
    Q_OBJECT
public:

    QTabWidget *tab;
    TableInfoWidget *table;

    moex::Node *node;

    std::vector<std::pair<InfoWidgetBase*,moex::ViewData*>> tabItems;
public:
    explicit MainCentralWidget(QWidget *parent = 0);

    void showNode(moex::ViewNode *node);

private:
    void releaseCurrentTabItems();
    void addTabItem(InfoWidgetBase *view,const QString & title,moex::ViewData* data);
    void loadCurrentTab();
signals:

public slots:
    void currentChanged(int index);
};

#endif // CONTENTVIEW_H
