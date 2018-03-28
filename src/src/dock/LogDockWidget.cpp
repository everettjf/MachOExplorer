//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "LogDockWidget.h"
#include <QHBoxLayout>
#include "src/controller/Workspace.h"

LogDockWidget::LogDockWidget(QWidget *parent) : QDockWidget(parent)
{
    setWindowTitle(tr("Log"));

    textEdit = new QTextEdit(this);
    textEdit->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    textEdit->setReadOnly(true);
    textEdit->setMinimumHeight(150);

    setWidget(textEdit);
}

void LogDockWidget::addLine(const QString &line)
{
    textEdit->insertPlainText("> ");
    textEdit->insertPlainText(line);
    textEdit->insertPlainText("\n");
}
