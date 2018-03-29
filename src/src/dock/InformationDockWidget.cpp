//
// Created by everettjf on 2018/3/28.
//

#include "InformationDockWidget.h"

InformationDockWidget::InformationDockWidget(QWidget *parent) : QDockWidget(parent)
{
    setWindowTitle(tr("Information"));

    textEdit = new QTextEdit(this);
    textEdit->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    textEdit->setReadOnly(true);

    setWidget(textEdit);

    //default message
    this->setContent(QStringLiteral("# Hi, have a nice day !"));
}
void InformationDockWidget::setContent(const QString &line)
{
    textEdit->setText(line);
}

