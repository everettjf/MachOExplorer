//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef UTILITY_H
#define UTILITY_H

#include "Stable.h"


namespace util {

QString qstr(const std::string & str);
void openURL(const QString & url);
void quitApp();
void showInfo(QWidget *parent,const QString & info);
void showError(QWidget *parent,const QString & info);
void showWarn(QWidget *parent,const QString & info);



QString formatBytecount(const long bytecount);
}
#endif // UTILITY_H
