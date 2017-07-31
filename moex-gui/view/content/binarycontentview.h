//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef BINARYCONTENTVIEW_H
#define BINARYCONTENTVIEW_H

#include <QWidget>
#include "contentviewinterface.h"
#include "../qhexedit/qhexedit.h"


class BinaryContentView : public ContentViewInterface
{
    Q_OBJECT
public:
    explicit BinaryContentView(QWidget *parent = 0);
    void showViewData(moex::ViewData *data)override;

private:
    void lazyInitUI();
private:
    QHexEdit *hexEdit;
    bool uiInited = false;
signals:

public slots:
};

#endif // BINARYCONTENTVIEW_H
