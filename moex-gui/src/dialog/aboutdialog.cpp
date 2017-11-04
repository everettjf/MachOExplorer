//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "../common/appinfo.h"
#include "../utility/utility.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QPixmap img(":res/machoexplorer.png");
    img.scaled(QSize(128,128),Qt::KeepAspectRatio);
    ui->label->setPixmap(img);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_pushButton_clicked()
{
    QString info = QString("MachOExplorer v%1 by everettjf")
            .arg(AppInfo::Instance().GetAppVersion());
    util::showInfo(this,info);
}
