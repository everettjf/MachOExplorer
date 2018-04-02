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
#include "src/base/AppInfo.h"
#include "src/dialog/AboutDialog.h"
#include <QProcess>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui = WS()->ui();

    setMinimumSize(QSize(600,400));

    menu = new MainWindowMenu();
    action = new MainWindowAction();

    setWindowTitle(tr("MachOExplorer"));
    createUI();
    createActions();
    createStatusBar();
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

#define InitDock(dockvar,dockclass,dockaction) \
    dockvar = new dockclass(this); \
    ui->dockWidgets.push_back(dockvar); \
    connect(dockvar,&QDockWidget::visibilityChanged,this,[this](bool checked){\
    dockaction->setChecked(checked);\
    });

void MainWindow::createUI()
{
    // Main Window
    ui->main = new CentralWidget(this);
    setCentralWidget(ui->main);

    setDockNestingEnabled(true);

    // Layout View
    InitDock(ui->layout,LayoutDockWidget,action->showLayoutWindow);
    InitDock(ui->log,LogDockWidget,action->showLogWindow);
    InitDock(ui->hex,HexDockWidget,action->showHexWindow);
    InitDock(ui->information,InformationDockWidget,action->showInformationWindow);
//    InitDock(ui->sourcecode,SourceCodeDockWidget,action->showSourceCodeWindow);

    setCorner(Qt::TopLeftCorner,Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner,Qt::BottomDockWidgetArea);
    setCorner(Qt::TopRightCorner,Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner,Qt::BottomDockWidgetArea);


    // Left(Layout)
    addDockWidget(Qt::LeftDockWidgetArea,ui->layout);
    addDockWidget(Qt::RightDockWidgetArea,ui->hex);

    // Bottom(Log)
    addDockWidget(Qt::BottomDockWidgetArea,ui->log);

    // Log | Information | SourceCode
    splitDockWidget(ui->log,ui->information,Qt::Horizontal);
//    splitDockWidget(ui->information,ui->sourcecode,Qt::Horizontal);
}

#define InitAction(name,actionvar,dockvar) \
    actionvar = new QAction(tr(name)); \
    menu->window->addAction(actionvar);\
    actionvar->setCheckable(true);\
    actionvar->setChecked(true);\
    connect(actionvar,&QAction::triggered,this,[this](bool checked){\
        dockvar->setVisible(checked);\
    });


void MainWindow::createActions()
{
    // File
    menu->file = menuBar()->addMenu(tr("File"));

    action->newWindow = new QAction(tr("&New Window"));
    menu->file->addAction(action->newWindow);
    connect(action->newWindow,&QAction::triggered,this,[this](bool checked){
        QProcess process(this);
        process.setEnvironment(QProcess::systemEnvironment());
        process.startDetached(qApp->applicationFilePath());
    });

    action->closeWindow = new QAction(tr("&Close Window"));
    menu->file->addAction(action->closeWindow);
    connect(action->closeWindow,&QAction::triggered,this,[this](bool checked){
        this->close();
        delete this;
    });

    action->openFile = new QAction(tr("&Open File"));
    menu->file->addAction(action->openFile);
    connect(action->openFile,&QAction::triggered,this,[this](bool checked){
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"",tr("MachO File (*)"));
        if(fileName.length() == 0)
            return;
        WS()->openFile(fileName);
    });

    action->quit = new QAction(tr("&Quit"));
    menu->file->addAction(action->quit);
    connect(action->quit,&QAction::triggered,this,[this](bool checked){
        util::quitApp();
    });


    // Window
    menu->window = menuBar()->addMenu(tr("Window"));

    InitAction("Layout",action->showLayoutWindow,ui->layout);
    InitAction("Hex",action->showHexWindow,ui->hex);
    InitAction("Log",action->showLogWindow,ui->log);
    InitAction("Information",action->showInformationWindow,ui->information);
//    InitAction("SourceCode",action->showSourceCodeWindow,ui->sourcecode);



    // Help
    menu->help = menuBar()->addMenu(tr("Help"));

    action->viewSource = new QAction(tr("View Source"));
    menu->help->addAction(action->viewSource);
    connect(action->viewSource,&QAction::triggered,this,[this](bool checked){
        util::openURL("https://github.com/everettjf/MachOExplorer");
    });

    action->reportIssue = new QAction(tr("Report Issue"));
    menu->help->addAction(action->reportIssue);
    connect(action->reportIssue,&QAction::triggered,this,[this](bool checked){
        util::openURL("https://github.com/everettjf/MachOExplorer/issues");
    });

    action->checkUpdate = new QAction(tr("Check Update"));
    menu->help->addAction(action->checkUpdate);
    connect(action->checkUpdate,&QAction::triggered,this,[this](bool checked){
        CheckUpdateDialog dlg(this);
        dlg.exec();
    });

    action->about = new QAction(tr("About"));
    menu->help->addAction(action->about);
    connect(action->about,&QAction::triggered,this,[this](bool checked){
        AboutDialog dlg(this);
        dlg.exec();
    });
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Welcome"));
}




