//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "utility.h"
#include <QApplication>
#include <QDesktopServices>
#include <QMessageBox>


namespace util {

void quitApp()
{
    QApplication::quit();
}

void openURL(const QString &url)
{
    QDesktopServices::openUrl(QUrl(url));
}

QString qstr(const std::__1::string &str)
{
    return QString::fromStdString(str);
}

void showInfo(QWidget *parent, const QString &info)
{
    QMessageBox::information(parent,"Info",info);
}

void showError(QWidget *parent, const QString &info)
{
    QMessageBox::critical(parent,"Error",info);
}

void showWarn(QWidget *parent, const QString &info)
{
    QMessageBox::warning(parent,"Warning",info);
}

}
