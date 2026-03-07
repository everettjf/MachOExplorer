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
#include <QFontDatabase>
#include <QTabWidget>
#include <QPalette>
#include <QSettings>
#include <QEvent>
#include <QInputDialog>
#include <QFileInfo>
#include <climits>

#ifdef Q_OS_MAC
#include <libproc.h>
#endif

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
    setThemeMode(loadThemeMode(), false);
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

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::ApplicationPaletteChange && themeMode == ThemeMode::System) {
        applyVisualRefresh();
    }
    return QMainWindow::event(event);
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
    setDockOptions(dockOptions() | QMainWindow::AllowTabbedDocks | QMainWindow::AnimatedDocks);
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);

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

    action->attachProcess = new QAction(tr("&Attach by PID"));
    menu->file->addAction(action->attachProcess);
    connect(action->attachProcess, &QAction::triggered, this, [this](bool checked){
        Q_UNUSED(checked)
        bool ok = false;
        int pid = QInputDialog::getInt(this, tr("Attach Process"), tr("Process ID:"), 0, 1, INT_MAX, 1, &ok);
        if(!ok) return;

        QString path;
#ifdef Q_OS_MAC
        char exec_path[PROC_PIDPATHINFO_MAXSIZE] = {0};
        const int ret = proc_pidpath(pid, exec_path, sizeof(exec_path));
        if(ret > 0){
            path = QString::fromUtf8(exec_path);
        }
#endif
        if(path.isEmpty()){
            util::showError(this, tr("Unable to resolve process executable path.\nOn macOS this may require extra privileges for target PID."));
            return;
        }
        if(!QFileInfo::exists(path)){
            util::showError(this, tr("Resolved executable path does not exist:\n%1").arg(path));
            return;
        }

        this->showMaximized();
        WS()->openFile(path);
        statusBar()->showMessage(tr("Attached to PID %1: %2").arg(pid).arg(path), 6000);
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
    createThemeActions();



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

void MainWindow::createThemeActions()
{
    QMenu *themeMenu = menu->window->addMenu(tr("Theme"));

    themeActionGroup = new QActionGroup(this);
    themeActionGroup->setExclusive(true);

    action->themeSystem = themeMenu->addAction(tr("Follow System"));
    action->themeSystem->setCheckable(true);
    themeActionGroup->addAction(action->themeSystem);
    connect(action->themeSystem, &QAction::triggered, this, [this](bool checked) {
        if (checked) setThemeMode(ThemeMode::System, true);
    });

    action->themeLight = themeMenu->addAction(tr("Light"));
    action->themeLight->setCheckable(true);
    themeActionGroup->addAction(action->themeLight);
    connect(action->themeLight, &QAction::triggered, this, [this](bool checked) {
        if (checked) setThemeMode(ThemeMode::Light, true);
    });

    action->themeDark = themeMenu->addAction(tr("Dark"));
    action->themeDark->setCheckable(true);
    themeActionGroup->addAction(action->themeDark);
    connect(action->themeDark, &QAction::triggered, this, [this](bool checked) {
        if (checked) setThemeMode(ThemeMode::Dark, true);
    });
}

MainWindow::ThemeMode MainWindow::loadThemeMode() const
{
    QSettings settings;
    const QString saved = settings.value("ui/theme_mode", "system").toString();
    if (saved == "light") return ThemeMode::Light;
    if (saved == "dark") return ThemeMode::Dark;
    return ThemeMode::System;
}

bool MainWindow::isDarkMode() const
{
    if (themeMode == ThemeMode::Dark) return true;
    if (themeMode == ThemeMode::Light) return false;
    return palette().color(QPalette::Window).lightness() < 128;
}

void MainWindow::updateThemeActionChecks()
{
    if (!action->themeSystem || !action->themeLight || !action->themeDark) return;
    action->themeSystem->setChecked(themeMode == ThemeMode::System);
    action->themeLight->setChecked(themeMode == ThemeMode::Light);
    action->themeDark->setChecked(themeMode == ThemeMode::Dark);
}

void MainWindow::setThemeMode(ThemeMode mode, bool persist)
{
    themeMode = mode;
    updateThemeActionChecks();
    applyVisualRefresh();

    if (!persist) return;
    QSettings settings;
    QString value = "system";
    if (mode == ThemeMode::Light) value = "light";
    if (mode == ThemeMode::Dark) value = "dark";
    settings.setValue("ui/theme_mode", value);
}

void MainWindow::applyVisualRefresh()
{
    QFontDatabase::addApplicationFont(":/res/fonts/Inconsolata-Regular.ttf");
    QFontDatabase::addApplicationFont(":/res/fonts/Anonymous Pro.ttf");

    QFont uiFont("Inconsolata", 12);
    if (!uiFont.exactMatch()) {
        uiFont = font();
        uiFont.setPointSize(11);
    }
    setFont(uiFont);

    const QString style = isDarkMode() ? R"(
QMainWindow {
    background: #0b1020;
}
QMenuBar {
    background: #0f172a;
    color: #e2e8f0;
    padding: 4px;
}
QMenuBar::item {
    background: transparent;
    padding: 6px 12px;
    border-radius: 6px;
}
QMenuBar::item:selected {
    background: #1e293b;
}
QMenu {
    background: #111827;
    color: #e5e7eb;
    border: 1px solid #334155;
}
QMenu::item:selected {
    background: #1f2937;
}
QStatusBar {
    background: #020617;
    color: #cbd5e1;
}
QDockWidget {
    titlebar-close-icon: none;
    titlebar-normal-icon: none;
    font-weight: 600;
}
QDockWidget::title {
    text-align: left;
    background: #0f172a;
    color: #e2e8f0;
    padding: 6px 10px;
}
QTableView, QTreeView, QTextEdit {
    background: #0f172a;
    alternate-background-color: #111827;
    color: #e2e8f0;
    gridline-color: #334155;
    selection-background-color: #0284c7;
    selection-color: #ffffff;
    border: 1px solid #334155;
}
QHeaderView::section {
    background: #1e293b;
    color: #e2e8f0;
    border: 0;
    border-right: 1px solid #334155;
    border-bottom: 1px solid #334155;
    padding: 6px;
    font-weight: 600;
}
QScrollBar:vertical, QScrollBar:horizontal {
    background: #0b1220;
}
QTabBar::tab {
    background: #1e293b;
    color: #cbd5e1;
    padding: 6px 12px;
    margin-right: 2px;
}
QTabBar::tab:selected {
    background: #0284c7;
    color: #ffffff;
}
)" : R"(
QMainWindow {
    background: #f4f6f8;
}
QMenuBar {
    background: #111827;
    color: #f8fafc;
    padding: 4px;
}
QMenuBar::item {
    background: transparent;
    padding: 6px 12px;
    border-radius: 6px;
}
QMenuBar::item:selected {
    background: #1f2937;
}
QMenu {
    background: #ffffff;
    color: #111827;
    border: 1px solid #d1d5db;
}
QMenu::item:selected {
    background: #e5e7eb;
}
QStatusBar {
    background: #111827;
    color: #cbd5e1;
}
QDockWidget {
    titlebar-close-icon: none;
    titlebar-normal-icon: none;
    font-weight: 600;
}
QDockWidget::title {
    text-align: left;
    background: #0f172a;
    color: #e2e8f0;
    padding: 6px 10px;
}
QTableView, QTreeView, QTextEdit {
    background: #ffffff;
    alternate-background-color: #f8fafc;
    color: #0f172a;
    gridline-color: #e2e8f0;
    selection-background-color: #0ea5e9;
    selection-color: #ffffff;
    border: 1px solid #dbe3ea;
}
QHeaderView::section {
    background: #e2e8f0;
    color: #0f172a;
    border: 0;
    border-right: 1px solid #cbd5e1;
    border-bottom: 1px solid #cbd5e1;
    padding: 6px;
    font-weight: 600;
}
QScrollBar:vertical, QScrollBar:horizontal {
    background: #f1f5f9;
}
QTabBar::tab {
    background: #cbd5e1;
    color: #0f172a;
    padding: 6px 12px;
    margin-right: 2px;
}
QTabBar::tab:selected {
    background: #0ea5e9;
    color: #ffffff;
}
)";
    setStyleSheet(style);
}
