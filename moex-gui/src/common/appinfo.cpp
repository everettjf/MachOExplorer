//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "appinfo.h"

AppInfo::AppInfo()
{

}

AppInfo &AppInfo::Instance()
{
    static AppInfo o;
    return o;
}

QString AppInfo::GetAppVersion()
{
    return "0.3";
}

QString AppInfo::GetAppEdition()
{
    return "alpha";
}
