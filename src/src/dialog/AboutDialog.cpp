//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "AboutDialog.h"
#include "ui_aboutdialog.h"
#include "src/base/AppInfo.h"
#include "src/utility/Utility.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QPixmap img(":res/machoexplorer.png");
    img.scaled(QSize(128,128),Qt::KeepAspectRatio);
    ui->label->setPixmap(img);


    QString info = QString("MachOExplorer\n\n"
                           "v%1\n\n"
                           "App is written by everettjf\n\n"
                           "Icon is designed by wantline")
            .arg(AppInfo::Instance().GetAppVersion());

    ui->label_info->setText(info);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_pushButton_clicked()
{
    this->close();
}

void AboutDialog::on_pushButton_everettjf_clicked()
{
    util::openURL("https://weibo.com/everettjf");
}

void AboutDialog::on_pushButton_wantline_clicked()
{
    util::openURL("https://weibo.com/wantline");
}
