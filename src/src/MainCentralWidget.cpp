//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "MainCentralWidget.h"

#include <QBoxLayout>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "src/controller/Workspace.h"
#include <QDebug>

MainCentralWidget::MainCentralWidget(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Info");

    node = nullptr;

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    this->setLayout(layout);

    // TabWidget
    tab = new QTabWidget(this);
    tab->setTabPosition(QTabWidget::South);
    tab->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    layout->addWidget(tab);

    connect(tab,SIGNAL(currentChanged(int)),this,SLOT(currentChanged(int)));

}


void MainCentralWidget::showNode(moex::ViewNode *node)
{
    if(!node)return;

    releaseCurrentTabItems();

    for(auto viewdata : node->GetViewDatas()){
        QString title = QString::fromStdString(viewdata->title());

        if(viewdata->mode() == moex::ViewDataMode::Table){
            // Tab - Table
            table = new TableInfoWidget(this);
            addTabItem(table,title,viewdata.get());
        }else if(viewdata->mode() == moex::ViewDataMode::Binary){
            // Tab - Binary
            WS()->ui()->hex->showViewData(viewdata.get());
        }
    }

    loadCurrentTab();
}


void MainCentralWidget::releaseCurrentTabItems()
{
    tab->clear();

    for(auto & item : tabItems){
        delete item.first;
    }
    tabItems.clear();
}

void MainCentralWidget::addTabItem(InfoWidgetBase *view, const QString &title, moex::ViewData *data)
{
    tab->addTab(view,title);
    tabItems.push_back(std::make_pair(view,data));
}

void MainCentralWidget::loadCurrentTab()
{
    int index = tab->currentIndex();
    if(index < 0 || index >= tabItems.size())
        return;
    InfoWidgetBase *view = tabItems[index].first;
    moex::ViewData *data = tabItems[index].second;

    qDebug()<< view << " - " << data;

    view->showViewData(data);
}

void MainCentralWidget::currentChanged(int index)
{
    loadCurrentTab();
}



