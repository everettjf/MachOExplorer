//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "view/mainwindow.h"
#include <QApplication>
#include <iostream>
#include <QDesktopWidget>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.resize(900,600);
    w.show();

    return a.exec();
}
