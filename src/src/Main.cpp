//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include <iostream>
#include "controller/Workspace.h"
#include "MoexApplication.h"

using namespace std;

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(MachOExplorer);

    MoexApplication app(argc, argv);

    return app.exec();
}
