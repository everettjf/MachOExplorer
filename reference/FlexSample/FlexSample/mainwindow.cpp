//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "mainwindow.h"
#include <QApplication>

#include "QtFlex5/QtFlexWidget.h"
#include "QtFlex5/QtFlexHelper.h"
#include "QtFlex5/QtDockGuider.h"
#include "QtFlex5/QtDockWidget.h"
#include "QtFlex5/QtFlexManager.h"


#include <QtCore/QSettings>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QPushButton>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setObjectName("MainWindow");
    qApp->setProperty("window", QVariant::fromValue<QObject*>(this));

    connect(FlexManager::instance(), &FlexManager::dockWidgetCreated, this, &MainWindow::onDockWidgetCreated);

    center = FlexManager::instance()->createFlexWidget(Flex::HybridView, this, Flex::widgetFlags(), "RootFlex");
    setCentralWidget(center);

    setGeometry(QApplication::desktop()->availableGeometry().adjusted(100, 100, -100, -100));

    activeOrAddDockWidget(Flex::ToolView,"Tree",Flex::L0,0,center);
    activeOrAddDockWidget(Flex::ToolView,"Hex",Flex::M,0,center);
    activeOrAddDockWidget(Flex::FileView,"Content",Flex::R0,0,center);
    activeOrAddDockWidget(Flex::ToolView,"Log",Flex::B1,2,center);

//    activeOrAddDockWidget(Flex::ToolView,"寄存器",Flex::R0,0,center);
//    activeOrAddDockWidget(Flex::ToolView,"栈",Flex::B0,0,center);
//    activeOrAddDockWidget(Flex::ToolView,"断点",Flex::B1,0,center);
//    activeOrAddDockWidget(Flex::ToolView,"输出",Flex::B2,0,center);
}

MainWindow::~MainWindow()
{

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    FlexManager::instance()->close();
}

void MainWindow::onDockWidgetCreated(DockWidget *widget)
{
    auto const& title = widget->windowTitle();
    if(title == "Tree"){
        auto view = new QWidget(widget);
        view->setMinimumWidth(300);
        widget->attachWidget(view);
    }else if(title == "Hex"){

        auto view = new QWidget(widget);
        widget->attachWidget(view);
        widget->setSizePolicy(QSizePolicy::Policy::Preferred,QSizePolicy::Policy::Preferred);

    }else if(title == "Content"){

        auto view = new QWidget(widget);
        view->setMinimumWidth(500);
        widget->attachWidget(view);
    }else{
        // None
    }
}


DockWidget *MainWindow::findDockWidget(const QString& name)
{
    return FlexManager::instance()->dockWidget(name);
}


DockWidget *MainWindow::addDockWidget(Flex::ViewMode mode,
                                      const QString& name, Flex::DockArea area, int siteIndex,
                                      FlexWidget* parent)
{
    DockWidget* dockWidget = FlexManager::instance()->createDockWidget(mode, parent, Flex::widgetFlags(), name);
    dockWidget->setViewMode(mode);
    dockWidget->setWindowTitle(name);

    if (parent)
    {
        parent->addDockWidget(dockWidget,area,siteIndex);
    }
    return dockWidget;
}

DockWidget *MainWindow::activeOrAddDockWidget(
        Flex::ViewMode mode, const QString& name,
        Flex::DockArea area, int siteIndex, FlexWidget* parent)
{
    auto widget = findDockWidget(name);
    if (widget)
    {
        widget->activate();
        return widget;
    }

    return addDockWidget(mode, name, area, siteIndex, parent);
}
