#ifndef QTGUIDER_H
#define QTGUIDER_H

#include <QtWidgets/QMainWindow>

class QtGuiderImpl;

class FlexWidget;
class DockWidget;

class QtGuider : public QMainWindow
{
    Q_OBJECT
public:
    QtGuider(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~QtGuider();

protected:
    void closeEvent(QCloseEvent* evt);

public:
    void openFile_N(int n);
    void openView_N(int n);

private slots:
    void on_actionEdit_Undo_triggered();
    void on_actionEdit_Redo_triggered();
    void on_actionFile_N_triggered();
    void on_actionView_N_triggered();
    void on_actionTool_N_triggered();

private slots:
    void actionView_X_triggered();
    void actionFile_X_triggered();

private:
    friend class QtGuiderImpl;
private:
    QScopedPointer<QtGuiderImpl> impl;
};

class QtCentral : public QWidget
{
    Q_OBJECT
public:
    QtCentral(QWidget* parent);
    void createOne();
    void createTwo();

private slots:
    void on_flexWidgetCreated(FlexWidget*);
    void on_dockWidgetCreated(DockWidget*);
    void on_flexWidgetDestroying(FlexWidget*);
    void on_dockWidgetDestroying(DockWidget*);

private:
    QWidget* _widget;
};

#endif // QTGUIDER_H
