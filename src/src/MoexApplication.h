#ifndef MOEXAPPLICATION_H
#define MOEXAPPLICATION_H

#include <QApplication>
#include "MainWindow.h"



class MoexApplication : public QApplication
{
    Q_OBJECT
public:
    MoexApplication(int argc,char *argv[]);
    ~MoexApplication();

    MainWindow *getMainWindow(){return mainWindow;}

private:
    MainWindow *mainWindow;
};

#endif // MOEXAPPLICATION_H
