//
// Created by everettjf on 2018/3/30.
//

#include "SourceCodeDockWidget.h"

SourceCodeDockWidget::SourceCodeDockWidget(QWidget *parent) : QDockWidget(parent)
{
    setWindowTitle(tr("Source Code"));

    textEdit = new QTextEdit(this);
    textEdit->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    textEdit->setReadOnly(true);

    setWidget(textEdit);

    //default message
    this->setContent(QStringLiteral("print \"hello world\""));
}
void SourceCodeDockWidget::setContent(const QString &line)
{
    textEdit->setText(line);
}
