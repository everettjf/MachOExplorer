//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_everettjf_clicked();

    void on_pushButton_wantline_clicked();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
