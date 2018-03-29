//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include "src/dialog/OpenFileDialog.h"

#include "src/controller/Workspace.h"

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

    QAction *showLayoutWindow;
    QAction *showHexWindow;
    QAction *showLogWindow;
    QAction *showInformationWindow;
    QAction *showSourceCodeWindow;

    QAction *reportIssue;
    QAction *viewSource;
    QAction *checkUpdate;
    QAction *about;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    WorkspaceUI *ui;
    MainWindowMenu *menu;
    MainWindowAction *action;

    OpenFileDialog *openFileDialog = nullptr;

public:
    explicit MainWindow(QWidget *parent = 0);

    void displayNewFileDialog();
    void openNewFile(const QString & filePath);

protected:
    void createUI();
    void createActions();
    void createStatusBar();

};

#endif // MAINWINDOW_H
