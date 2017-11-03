//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "view/mainwindow.h"
#include <QApplication>
#include <iostream>
#include <QDesktopWidget>
#include "common/appinfo.h"

using namespace std;

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(moex);

    QApplication app(argc, argv);

    app.setApplicationName("MachOExplorer");
    app.setOrganizationName("everettjf");
    app.setOrganizationDomain("everettjf.com");
    app.setApplicationVersion(AppInfo::Instance().GetAppVersion());
    app.setWindowIcon(QIcon(":res/moex.ico"));

    MainWindow w;
    w.setGeometry(QApplication::desktop()->availableGeometry().adjusted(200, 100, -200, -100));
    w.show();

    return app.exec();
}
