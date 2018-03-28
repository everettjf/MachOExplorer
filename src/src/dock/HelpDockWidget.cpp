//
// Created by everettjf on 2018/3/28.
//

#include "HelpDockWidget.h"

HelpDockWidget::HelpDockWidget(QWidget *parent) : QDockWidget(parent)
{
    setWindowTitle(tr("Help"));

    textEdit = new QTextEdit(this);
    textEdit->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    textEdit->setReadOnly(true);

    setWidget(textEdit);

    //default message
    this->setContent(QStringLiteral("# Hi, have a nice day !"));
}
void HelpDockWidget::setContent(const QString &line)
{
    textEdit->setText(line);
}

