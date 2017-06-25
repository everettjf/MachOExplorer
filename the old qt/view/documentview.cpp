//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "documentview.h"

DocumentView::DocumentView(QWidget *parent) : QWidget(parent)
{
    QTextEdit *te = new QTextEdit(this);
    te->setText(tr("Document Window"));
    te->setAlignment(Qt::AlignCenter);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(te);
    setLayout(layout);
}
