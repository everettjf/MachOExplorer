//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "AppInfo.h"
#include "../../libmoex/ver.h"

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
    return LIBMOEX_VERSION;
}

QString AppInfo::GetAppEdition()
{
    return "renovated";
}
