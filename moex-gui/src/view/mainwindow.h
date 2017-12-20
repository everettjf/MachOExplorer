//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QMenu>
#include <QAction>

#include "../controller/workspacemanager.h"
#include "../controller/windowshareddata.h"

struct MainWindowMenu{
    QMenu *file;
    QMenu *window;
    QMenu *help;
};

struct MainWindowAction{
    QAction *newWindow;
    QAction *closeWindow;
    QAction *openFile;
    QAction *quit;

    QAction *showLogWindow;

    QAction *reportIssue;
    QAction *viewSource;
    QAction *checkUpdate;
    QAction *about;
};

class MainWindow : public QMainWindow , public WindowSharedData
{
    Q_OBJECT

private:
    WorkspaceUI *ui;
    MainWindowMenu *menu;
    MainWindowAction *action;

public:
    explicit MainWindow(QWidget *parent = 0);

protected:
    void createUI();
    void createActions();
    void createMenus();

signals:

public slots:
    void newWindow(bool checked);
    void closeWindow(bool checked);
    void openFile(bool checked);
    void quitApp(bool checked);

    void showLogWindow(bool checked);

    void reportIssue(bool checked);
    void viewSource(bool checked);
    void checkUpdate(bool checked);
    void aboutApp(bool checked);

    void dockLogVisibilityChanged(bool visible);
};

#endif // MAINWINDOW_H
