//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef CONTENTVIEW_H
#define CONTENTVIEW_H

#include <QWidget>
#include <QTabWidget>
#include <QStackedWidget>

#include "src/view/content/TableContentView.h"
#include "src/view/content/BinaryContentView.h"
#include <libmoex/moex.h>
#include <libmoex/moex-view.h>
#include <vector>
#include <map>

class ContentView : public QWidget
{
    Q_OBJECT
public:

    QTabWidget *tab;
    TableContentView *table;
    BinaryContentView *binary;

    moex::Node *node;

    std::vector<std::pair<ContentViewInterface*,moex::ViewData*>> tabItems;
public:
    explicit ContentView(QWidget *parent = 0);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent* event);

    void openFile(const QString & filePath);
    void showNode(moex::ViewNode *node);

private:
    void releaseCurrentTabItems();
    void addTabItem(ContentViewInterface *view,const QString & title,moex::ViewData* data);
    void loadCurrentTab();
signals:

public slots:
    void currentChanged(int index);
};

#endif // CONTENTVIEW_H
