//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "InfoDockWidget.h"

#include <QBoxLayout>
#include <QLabel>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "src/controller/Workspace.h"
#include <QDebug>

InfoDockWidget::InfoDockWidget(QWidget *parent) : QDockWidget(parent)
{
    setWindowTitle("Info");

    node = nullptr;

    // TabWidget
    tab = new QTabWidget(this);
    tab->setTabPosition(QTabWidget::West);
    tab->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    setWidget(tab);
    connect(tab,SIGNAL(currentChanged(int)),this,SLOT(currentChanged(int)));

}


void InfoDockWidget::showNode(moex::ViewNode *node)
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


void InfoDockWidget::releaseCurrentTabItems()
{
    tab->clear();

    for(auto & item : tabItems){
        delete item.first;
    }
    tabItems.clear();
}

void InfoDockWidget::addTabItem(InfoWidgetBase *view, const QString &title, moex::ViewData *data)
{
    tab->addTab(view,title);
    tabItems.push_back(std::make_pair(view,data));
}

void InfoDockWidget::loadCurrentTab()
{
    int index = tab->currentIndex();
    if(index < 0 || index >= tabItems.size())
        return;
    InfoWidgetBase *view = tabItems[index].first;
    moex::ViewData *data = tabItems[index].second;

    qDebug()<< view << " - " << data;

    view->showViewData(data);
}

void InfoDockWidget::currentChanged(int index)
{
    loadCurrentTab();
}



