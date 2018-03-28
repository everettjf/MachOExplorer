//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef CONTENTVIEW_H
#define CONTENTVIEW_H

#include <QDockWidget>
#include <QTabWidget>
#include <QStackedWidget>

#include "src/widget/TableContentWidget.h"
#include "HexDockWidget.h"
#include <libmoex/moex.h>
#include <libmoex/moex-view.h>
#include <vector>
#include <map>

class InfoDockWidget : public QDockWidget
{
    Q_OBJECT
public:

    QTabWidget *tab;
    TableContentWidget *table;

    moex::Node *node;

    std::vector<std::pair<ContentWidgetBase*,moex::ViewData*>> tabItems;
public:
    explicit InfoDockWidget(QWidget *parent = 0);

    void showNode(moex::ViewNode *node);

private:
    void releaseCurrentTabItems();
    void addTabItem(ContentWidgetBase *view,const QString & title,moex::ViewData* data);
    void loadCurrentTab();
signals:

public slots:
    void currentChanged(int index);
};

#endif // CONTENTVIEW_H
