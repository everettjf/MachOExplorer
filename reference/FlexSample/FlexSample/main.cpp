//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    return a.exec();
}
