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
#include "BinaryDockWidget.h"
#include <libmoex/moex.h>
#include <libmoex/moex-view.h>
#include <vector>
#include <map>

class MainContentWidget : public QWidget
{
    Q_OBJECT
public:

    QTabWidget *tab;
    TableContentWidget *table;

    moex::Node *node;

    std::vector<std::pair<ContentWidgetBase*,moex::ViewData*>> tabItems;
public:
    explicit MainContentWidget(QWidget *parent = 0);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent* event);

    void openFile(const QString & filePath);
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
