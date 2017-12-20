//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "view/mainwindow.h"
#include <QApplication>
#include <iostream>
#include "common/appinfo.h"
#include "controller/workspacemanager.h"

using namespace std;

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(MachOExplorer);

    QApplication app(argc, argv);

    app.setApplicationName("MachOExplorer");
    app.setOrganizationName("everettjf");
    app.setOrganizationDomain("everettjf.com");
    app.setApplicationVersion(AppInfo::Instance().GetAppVersion());
    app.setWindowIcon(QIcon(":MachOExplorer.icns"));

    WorkspaceManager::newWorkspace();

    return app.exec();
}
