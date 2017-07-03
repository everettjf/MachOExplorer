//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "mainwindow.h"
#include <QDockWidget>
#include <QMenuBar>
#include "../utility/utility.h"
#include <QFileDialog>
#include "../controller/workspacemanager.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui = new WorkspaceUI();
    menu = new MainWindowMenu();
    action = new MainWindowAction();

    setWindowTitle(tr("MachOExplorer"));
    createUI();
    createActions();
    createMenus();

    WorkspaceManager::current()->set_ui(ui);

    // TEST
#ifndef NDEBUG
    WorkspaceManager::current()->openFile("/Applications/SizeOptDemo");
#endif
}

void MainWindow::createUI()
{
    // Main Window
    ui->content = new ContentView(this);
    setCentralWidget(ui->content);

    // Skeleton View
    {
        ui->skeleton = new SkeletonView();

        ui->skeletonDock = new QDockWidget(tr("Skeleton Window"),this);
        ui->skeletonDock->setFeatures(QDockWidget::DockWidgetMovable);
        ui->skeletonDock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        ui->skeletonDock->setWidget(ui->skeleton);
        addDockWidget(Qt::LeftDockWidgetArea,ui->skeletonDock);
    }
    // Log View
    {
        ui->log = new LogView();

        ui->logDock = new QDockWidget(tr("Log Window"),this);
        ui->logDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
        ui->logDock->setWidget(ui->log);
        connect(ui->logDock,SIGNAL(visibilityChanged(bool)),this,SLOT(dockLogVisibilityChanged(bool)));
        addDockWidget(Qt::BottomDockWidgetArea,ui->logDock);
    }
    // Document View
    {
        ui->document = new DocumentView();

        ui->documentDock = new QDockWidget(tr("Document Window"),this);
        ui->documentDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
        ui->documentDock->setWidget(ui->document);
        connect(ui->documentDock,SIGNAL(visibilityChanged(bool)),this,SLOT(dockDocumentVisibilityChanged(bool)));
        addDockWidget(Qt::RightDockWidgetArea,ui->documentDock);
    }

    setCorner(Qt::TopLeftCorner,Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner,Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner,Qt::RightDockWidgetArea);
}

void MainWindow::createActions()
{
    action->openFile = new QAction(tr("&Open File"));
    connect(action->openFile,SIGNAL(triggered(bool)),this,SLOT(openFile(bool)));

    action->quit = new QAction(tr("&Quit"));
    connect(action->quit,SIGNAL(triggered(bool)),this,SLOT(quitApp(bool)));

    action->showLogWindow = new QAction(tr("Log Window"));
    action->showLogWindow->setCheckable(true);
    action->showLogWindow->setChecked(true);
    connect(action->showLogWindow,SIGNAL(triggered(bool)),this,SLOT(showLogWindow(bool)));

    action->showDocumentWindow = new QAction(tr("Document Window"));
    action->showDocumentWindow->setCheckable(true);
    action->showDocumentWindow->setChecked(true);
    connect(action->showDocumentWindow,SIGNAL(triggered(bool)),this,SLOT(showDocumentWindow(bool)));

    action->viewSource = new QAction(tr("View Source"));
    connect(action->viewSource,SIGNAL(triggered(bool)),this,SLOT(viewSource(bool)));

    action->reportIssue = new QAction(tr("Report Issue"));
    connect(action->reportIssue,SIGNAL(triggered(bool)),this,SLOT(reportIssue(bool)));

    action->about = new QAction(tr("About"));
    connect(action->about,SIGNAL(triggered(bool)),this,SLOT(aboutApp(bool)));

}

void MainWindow::createMenus()
{
    menu->file = menuBar()->addMenu(tr("File"));
    menu->file->addAction(action->openFile);
    menu->file->addAction(action->quit);

    menu->window = menuBar()->addMenu(tr("Window"));
    menu->window->addAction(action->showLogWindow);
    menu->window->addAction(action->showDocumentWindow);

    menu->help = menuBar()->addMenu(tr("Help"));
    menu->help->addAction(action->viewSource);
    menu->help->addAction(action->reportIssue);
    menu->help->addAction(action->about);
}

void MainWindow::openFile(bool checked)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                      "",
                                                      tr("MachO File (*)"));

    if(fileName.length() == 0)
        return;

    WorkspaceManager::current()->openFile(fileName);
}

void MainWindow::quitApp(bool checked)
{
    util::quitApp();
}

void MainWindow::showLogWindow(bool checked)
{
    ui->logDock->setVisible(checked);
}

void MainWindow::showDocumentWindow(bool checked)
{
    ui->documentDock->setVisible(checked);
}

void MainWindow::reportIssue(bool checked)
{
    util::openURL("https://github.com/everettjf/MachOExplorer/issues");
}

void MainWindow::viewSource(bool checked)
{
    util::openURL("https://github.com/everettjf/MachOExplorer");

}

void MainWindow::aboutApp(bool checked)
{
    util::showInfo(this,"MachOExplorer v0.1 by everettjf");
}

void MainWindow::dockLogVisibilityChanged(bool visible)
{
    action->showLogWindow->setChecked(visible);
}

void MainWindow::dockDocumentVisibilityChanged(bool visible)
{
    action->showDocumentWindow->setChecked(visible);
}

