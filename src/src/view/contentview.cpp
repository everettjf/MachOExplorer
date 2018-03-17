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
#include <QDebug>

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
    connect(tab,SIGNAL(currentChanged(int)),this,SLOT(currentChanged(int)));

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
    workspace->openFile(filePath);

    displayContentTab();
}

void ContentView::showNode(moex::ViewNode *node)
{
    if(!node)return;

    displayContentTab();
    releaseCurrentTabItems();

    for(auto viewdata : node->GetViewDatas()){
        QString title = QString::fromStdString(viewdata->title());

        if(viewdata->mode() == moex::ViewDataMode::Table){
            // Tab - Table
            table = new TableContentView(this);
            table->workspace = workspace;
            addTabItem(table,title,viewdata.get());
        }else if(viewdata->mode() == moex::ViewDataMode::Binary){
            // Tab - Binary
            binary = new BinaryContentView(this);
            binary->workspace = workspace;
            addTabItem(binary,title,viewdata.get());
        }else{
            // No such mode
        }
    }

    loadCurrentTab();
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
    tabItems.push_back(std::make_pair(view,data));
}

void ContentView::loadCurrentTab()
{
    int index = tab->currentIndex();
    if(index < 0 || index >= tabItems.size())
        return;
    ContentViewInterface *view = tabItems[index].first;
    moex::ViewData *data = tabItems[index].second;

    qDebug()<< view << " - " << data;

    view->showViewData(data);
}

void ContentView::currentChanged(int index)
{
    loadCurrentTab();
}



