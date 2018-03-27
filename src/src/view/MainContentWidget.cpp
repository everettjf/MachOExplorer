//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "MainContentWidget.h"

#include <QBoxLayout>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "src/controller/Workspace.h"
#include <QDebug>

MainContentWidget::MainContentWidget(QWidget *parent) : QWidget(parent)
{
    node = nullptr;

    // Stack
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);

    // TabWidget
    tab = new QTabWidget(this);
    layout->addWidget(tab);
    connect(tab,SIGNAL(currentChanged(int)),this,SLOT(currentChanged(int)));

    setAcceptDrops(true);
}

void MainContentWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void MainContentWidget::dropEvent(QDropEvent *event)
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

void MainContentWidget::openFile(const QString &filePath)
{
    WS()->openFile(filePath);

}

void MainContentWidget::showNode(moex::ViewNode *node)
{
    if(!node)return;

    releaseCurrentTabItems();

    for(auto viewdata : node->GetViewDatas()){
        QString title = QString::fromStdString(viewdata->title());

        if(viewdata->mode() == moex::ViewDataMode::Table){
            // Tab - Table
            table = new TableContentWidget(this);
            addTabItem(table,title,viewdata.get());
        }else if(viewdata->mode() == moex::ViewDataMode::Binary){
            // Tab - Binary
            WS()->ui()->binary->showViewData(viewdata.get());
        }
    }

    loadCurrentTab();
}


void MainContentWidget::releaseCurrentTabItems()
{
    tab->clear();

    for(auto & item : tabItems){
        delete item.first;
    }
    tabItems.clear();
}

void MainContentWidget::addTabItem(ContentWidgetBase *view, const QString &title, moex::ViewData *data)
{
    tab->addTab(view,title);
    tabItems.push_back(std::make_pair(view,data));
}

void MainContentWidget::loadCurrentTab()
{
    int index = tab->currentIndex();
    if(index < 0 || index >= tabItems.size())
        return;
    ContentWidgetBase *view = tabItems[index].first;
    moex::ViewData *data = tabItems[index].second;

    qDebug()<< view << " - " << data;

    view->showViewData(data);
}

void MainContentWidget::currentChanged(int index)
{
    loadCurrentTab();
}



