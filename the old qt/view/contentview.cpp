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
    stack = new QStackedWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(stack);
    setLayout(layout);

    // Stack - Blank
    blank = new BlankContentView(this);
    stack->addWidget(blank);

    // Stack - TabWidget
    tab = new QTabWidget(this);
    stack->addWidget(tab);

    // Tab - Table
    table = new TableContentView(this);
    tab->addTab(table,tr("Data"));

    // Tab - Binary
    binary = new BinaryContentView(this);
    tab->addTab(binary,tr("Binary"));

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

void ContentView::showNode(moex::Node *node)
{
    moex::NodeViewArray views = node->GetViews();
    for(moex::NodeViewPtr & view : views){
        switch(view->type()){
        case moex::NodeViewType::Address:{

            break;
        }
        case moex::NodeViewType::Table:{

            break;
        }
        }
    }
}

void ContentView::displayContentTab()
{
    stack->setCurrentIndex(1);
}

void ContentView::showAddressNodeView(moex::AddressNodeView *view)
{

}

void ContentView::showTableNodeView(moex::TableNodeView *view)
{

}
