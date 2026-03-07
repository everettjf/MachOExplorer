//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QActionGroup>
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
    QAction *attachProcessPath;
    QAction *attachProcessSnapshot;
    QAction *extractDyldImage;
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

    QAction *themeSystem;
    QAction *themeLight;
    QAction *themeDark;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum class ThemeMode {
        System,
        Light,
        Dark
    };

private:
    WorkspaceUI *ui;
    MainWindowMenu *menu;
    MainWindowAction *action;
    QActionGroup *themeActionGroup = nullptr;
    ThemeMode themeMode = ThemeMode::System;

    OpenFileDialog *openFileDialog = nullptr;

public:
    explicit MainWindow(QWidget *parent = 0);

    void displayNewFileDialog();
    void openNewFile(const QString & filePath);

protected:
    bool event(QEvent *event) override;
    void createUI();
    void createActions();
    void createStatusBar();
    void createThemeActions();
    void applyVisualRefresh();
    void setThemeMode(ThemeMode mode, bool persist);
    ThemeMode loadThemeMode() const;
    bool isDarkMode() const;
    void updateThemeActionChecks();

};

#endif // MAINWINDOW_H
