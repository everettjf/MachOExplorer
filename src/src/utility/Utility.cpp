//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "Utility.h"
#include <QApplication>
#include <QDesktopServices>
#include <QMessageBox>
#include <cmath>


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

QString formatBytecount(const long bytecount)
{
    if (bytecount == 0)
        return "0";
    const int exp = log(bytecount) / log(1000);
    constexpr char suffixes[] = {' ', 'k', 'M', 'G', 'T', 'P', 'E'};

    QString str;
    QTextStream stream(&str);
    stream << qSetRealNumberPrecision(3) << bytecount / pow(1000, exp)
           << ' ' << suffixes[exp] << 'B';
    return stream.readAll();
}

}
