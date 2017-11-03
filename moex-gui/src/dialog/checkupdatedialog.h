//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef CHECKUPDATEDIALOG_H
#define CHECKUPDATEDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>


namespace Ui {
class CheckUpdateDialog;
}

class CheckUpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CheckUpdateDialog(QWidget *parent = 0);
    ~CheckUpdateDialog();
    void gotoDownloadPage();

private slots:
    void on_pushButton_clicked();
    void slot_appversion_detected(bool,QString);

    void on_pushButton_2_clicked();

signals:
    void signal_appversion_detected(bool,QString);

private:
    Ui::CheckUpdateDialog *ui;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
};

#endif // CHECKUPDATEDIALOG_H
