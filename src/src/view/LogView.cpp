//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "LogView.h"
#include <QHBoxLayout>
#include "src/controller/WorkspaceManager.h"

LogView::LogView(QWidget *parent) : QWidget(parent)
{
    textEdit = new QTextEdit(this);
    textEdit->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    textEdit->setReadOnly(true);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(textEdit);
    setLayout(layout);
}

void LogView::addLine(const QString &line)
{
    textEdit->insertPlainText(line);
    textEdit->insertPlainText("\n");
}

QSize LogView::sizeHint() const
{
    return QSize(0, 100);
}
