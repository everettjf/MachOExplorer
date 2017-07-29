//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef BINARYCONTENTVIEW_H
#define BINARYCONTENTVIEW_H

#include <QWidget>
#include <libmoex/moex-view.h>
#include "../qhexedit/qhexedit.h"


class BinaryContentView : public QWidget
{
    Q_OBJECT
public:
    explicit BinaryContentView(QWidget *parent = 0);
    void showNode(moex::BinaryViewData *node);
private:
    QHexEdit *hexEdit;
signals:

public slots:
};

#endif // BINARYCONTENTVIEW_H
