//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "view/mainwindow.h"
#include <QApplication>
#include <iostream>
#include "common/appinfo.h"
#include "controller/workspacemanager.h"

#include "test/Test.h"

using namespace std;

int main(int argc, char *argv[])
{
    Test::go();

    Q_INIT_RESOURCE(MachOExplorer);

    QApplication app(argc, argv);

    app.setApplicationName("MachOExplorer");
    app.setOrganizationName("everettjf");
    app.setOrganizationDomain("everettjf.com");
    app.setApplicationVersion(AppInfo::Instance().GetAppVersion());
    app.setWindowIcon(QIcon(":MachOExplorer.icns"));

    WorkspaceManager::Instance()->newWorkspace();

    return app.exec();
}
