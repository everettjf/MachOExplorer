//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef APPINFO_H
#define APPINFO_H


#include <QString>


class AppInfo
{
public:
    AppInfo();
    static AppInfo & Instance();

    QString GetAppVersion();
    QString GetAppEdition();
};

#endif // APPINFO_H
