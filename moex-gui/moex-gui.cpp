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
    Q_INIT_RESOURCE(moex);

    QApplication app(argc, argv);

    app.setApplicationName("MOEX");
    app.setOrganizationName("MOEX");
    app.setWindowIcon(QIcon(":res/moex.ico"));

    MainWindow w;
    w.resize(900,600);
    w.show();

    return app.exec();
}
