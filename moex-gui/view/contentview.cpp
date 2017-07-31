//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "contentview.h"

#include <QBoxLayout>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "../controller/workspacemanager.h"

ContentView::ContentView(QWidget *parent) : QWidget(parent)
{
    node = nullptr;

    // Stack
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);

    stack = new QStackedWidget(this);
    layout->addWidget(stack);

    // Stack - Blank
    blank = new BlankContentView(this);
    stack->addWidget(blank);

    // Stack - TabWidget
    tab = new QTabWidget(this);
    stack->addWidget(tab);

    setAcceptDrops(true);
}

void ContentView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void ContentView::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    // check for our needed mime type, here a file or a list of files
    if (mimeData->hasUrls())
    {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();
        if(urlList.length() > 0){
            openFile(urlList.at(0).toLocalFile());
        }
    }
}

void ContentView::openFile(const QString &filePath)
{
    WorkspaceManager::current()->openFile(filePath);

    displayContentTab();
}

void ContentView::showNode(moex::ViewNode *node)
{
    if(!node)return;

    displayContentTab();

    releaseCurrentTabItems();

    for(auto viewdata : node->GetViewDatas()){
        if(viewdata->mode() == moex::ViewDataMode::Table){
            // Tab - Table
            table = new TableContentView(this);
            addTabItem(table,tr("Data"),viewdata.get());
        }else if(viewdata->mode() == moex::ViewDataMode::Binary){
            // Tab - Binary
            binary = new BinaryContentView(this);
            addTabItem(binary,tr("Binary"),viewdata.get());
        }else{
            // No such mode
        }
    }

}

void ContentView::displayContentTab()
{
    stack->setCurrentIndex(1);
}

void ContentView::releaseCurrentTabItems()
{
    tab->clear();

    for(auto & item : tabItems){
        delete item.first;
    }
    tabItems.clear();
}

void ContentView::addTabItem(ContentViewInterface *view, const QString &title, moex::ViewData *data)
{
    tab->addTab(view,title);
    view->showViewData(data);
    tabItems.push_back(std::make_pair(view,data));
}



