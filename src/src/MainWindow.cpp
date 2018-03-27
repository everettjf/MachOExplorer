//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "MainWindow.h"
#include <QDockWidget>
#include <QMenuBar>
#include "src/utility/Utility.h"
#include <QFileDialog>
#include "src/controller/Workspace.h"
#include "src/dialog/CheckUpdateDialog.h"
#include "src/common/AppInfo.h"
#include "src/dialog/AboutDialog.h"
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui = WS()->ui();

    setMinimumSize(QSize(600,400));

    menu = new MainWindowMenu();
    action = new MainWindowAction();

    setWindowTitle(tr("MachOExplorer"));
    createUI();
    createActions();
    createMenus();

}

void MainWindow::displayNewFileDialog()
{
    openFileDialog = new OpenFileDialog();
    openFileDialog->setAttribute(Qt::WA_DeleteOnClose);
    openFileDialog->show();
}

void MainWindow::openNewFile(const QString &filePath)
{
    this->showMaximized();

    WS()->openFile(filePath);
}

void MainWindow::createUI()
{
    // Main Window
    ui->content = new ContentView(this);
    setCentralWidget(ui->content);

    // layout View
    {
        ui->layout = new LayoutView();

        ui->layoutDock = new QDockWidget(tr("Layout"),this);
        ui->layoutDock->setFeatures(QDockWidget::DockWidgetMovable);
        ui->layoutDock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        ui->layoutDock->setWidget(ui->layout);
        addDockWidget(Qt::LeftDockWidgetArea,ui->layoutDock);
    }
    // Log View
    {
        ui->log = new LogView();

        ui->logDock = new QDockWidget(tr("Log"),this);
        ui->logDock->setFeatures(QDockWidget::AllDockWidgetFeatures);
        ui->logDock->setWidget(ui->log);
        connect(ui->logDock,SIGNAL(visibilityChanged(bool)),this,SLOT(dockLogVisibilityChanged(bool)));
        addDockWidget(Qt::BottomDockWidgetArea,ui->logDock);
    }

    setCorner(Qt::TopLeftCorner,Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);
    setCorner(Qt::TopRightCorner,Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner,Qt::RightDockWidgetArea);

}

void MainWindow::createActions()
{
    action->newWindow = new QAction(tr("&New Window"));
    connect(action->newWindow,SIGNAL(triggered(bool)),this,SLOT(newWindow(bool)));

    action->closeWindow = new QAction(tr("&Close Window"));
    connect(action->closeWindow,SIGNAL(triggered(bool)),this,SLOT(closeWindow(bool)));

    action->openFile = new QAction(tr("&Open File"));
    connect(action->openFile,SIGNAL(triggered(bool)),this,SLOT(openFile(bool)));

    action->quit = new QAction(tr("&Quit"));
    connect(action->quit,SIGNAL(triggered(bool)),this,SLOT(quitApp(bool)));

    action->showLogWindow = new QAction(tr("Log Window"));
    action->showLogWindow->setCheckable(true);
    action->showLogWindow->setChecked(true);
    connect(action->showLogWindow,SIGNAL(triggered(bool)),this,SLOT(showLogWindow(bool)));

    action->viewSource = new QAction(tr("View Source"));
    connect(action->viewSource,SIGNAL(triggered(bool)),this,SLOT(viewSource(bool)));

    action->reportIssue = new QAction(tr("Report Issue"));
    connect(action->reportIssue,SIGNAL(triggered(bool)),this,SLOT(reportIssue(bool)));

    action->checkUpdate = new QAction(tr("Check Update"));
    connect(action->checkUpdate,SIGNAL(triggered(bool)),this,SLOT(checkUpdate(bool)));

    action->about = new QAction(tr("About"));
    connect(action->about,SIGNAL(triggered(bool)),this,SLOT(aboutApp(bool)));

}

void MainWindow::createMenus()
{
    menu->file = menuBar()->addMenu(tr("File"));
    menu->file->addAction(action->newWindow);
    menu->file->addAction(action->closeWindow);
    menu->file->addAction(action->openFile);
    menu->file->addAction(action->quit);

    menu->window = menuBar()->addMenu(tr("Window"));
    menu->window->addAction(action->showLogWindow);

    menu->help = menuBar()->addMenu(tr("Help"));
    menu->help->addAction(action->viewSource);
    menu->help->addAction(action->reportIssue);
    menu->help->addAction(action->checkUpdate);
    menu->help->addAction(action->about);
}

void MainWindow::newWindow(bool checked)
{
    QProcess process(this);
    process.setEnvironment(QProcess::systemEnvironment());
    process.startDetached(qApp->applicationFilePath());
}

void MainWindow::closeWindow(bool checked)
{
    this->close();
    delete this;
}

void MainWindow::openFile(bool checked)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                      "",
                                                      tr("MachO File (*)"));

    if(fileName.length() == 0)
        return;

    WS()->openFile(fileName);
}


void MainWindow::quitApp(bool checked)
{
    util::quitApp();
}

void MainWindow::showLogWindow(bool checked)
{
    ui->logDock->setVisible(checked);
}

void MainWindow::reportIssue(bool checked)
{
    util::openURL("https://github.com/everettjf/MachOExplorer/issues");
}

void MainWindow::viewSource(bool checked)
{
    util::openURL("https://github.com/everettjf/MachOExplorer");
}

void MainWindow::checkUpdate(bool checked)
{
    CheckUpdateDialog dlg(this);
    dlg.exec();
}

void MainWindow::aboutApp(bool checked)
{
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::dockLogVisibilityChanged(bool visible)
{
    action->showLogWindow->setChecked(visible);
}

