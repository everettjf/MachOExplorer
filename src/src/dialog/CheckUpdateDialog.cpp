//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "CheckUpdateDialog.h"
#include "ui_checkupdatedialog.h"
#include <QNetworkReply>
#include "src/base/AppInfo.h"
#include "src/utility/Utility.h"


CheckUpdateDialog::CheckUpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CheckUpdateDialog)
{
    ui->setupUi(this);

    ui->label->setText(QString("Current version : %1").arg(AppInfo::Instance().GetAppVersion()));

    connect(this,SIGNAL(signal_appversion_detected(bool,QString)),this,SLOT(slot_appversion_detected(bool,QString)));

    manager = new QNetworkAccessManager();
    QObject::connect(manager, &QNetworkAccessManager::finished,
        this, [=](QNetworkReply *reply) {
            if (reply->error()) {
                qDebug() << reply->errorString();
                emit signal_appversion_detected(false,"");
                return;
            }

            QString answer = reply->readAll();

            qDebug() << answer;
            emit signal_appversion_detected(true,answer);
        }
    );
    request.setUrl(QUrl("http://everettjf.com/app/moex/latestversion.txt"));
    manager->get(request);


}

CheckUpdateDialog::~CheckUpdateDialog()
{
    delete ui;
}

void CheckUpdateDialog::gotoDownloadPage()
{
    util::openURL("https://github.com/everettjf/MachOExplorer/releases");
}

void CheckUpdateDialog::on_pushButton_clicked()
{
    gotoDownloadPage();

    this->close();
}

void CheckUpdateDialog::on_pushButton_2_clicked()
{
    this->close();
}

void CheckUpdateDialog::slot_appversion_detected(bool ok, QString version)
{
    if(!ok){
        ui->label->setText("Error get latest version from server.");
        return;
    }

    if(version.isEmpty()){
        ui->label->setText("Error get latest version from server.(version empty)");
        return;
    }

    QString curVersion = AppInfo::Instance().GetAppVersion();
    if(curVersion == version){
        ui->label->setText(QString("Already latest version : v%1").arg(curVersion));
        return;
    }

    ui->label->setText(QString("There is a new version.\nCurrent version : v%1 , Latest Version : v%2")
                       .arg(AppInfo::Instance().GetAppVersion())
                       .arg(version)
                       );

}
