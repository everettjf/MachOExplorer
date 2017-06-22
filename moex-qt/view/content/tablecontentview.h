//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef TABLECONTENTVIEW_H
#define TABLECONTENTVIEW_H

#include <QWidget>
#include <libmoex/binary.h>

class TableContentView : public QWidget
{
    Q_OBJECT
public:
    explicit TableContentView(QWidget *parent = 0);

    void set_view(moex::TableNodeView* view){view_ = view;}

private:
    moex::TableNodeView *view_;

signals:

public slots:
};

#endif // TABLECONTENTVIEW_H
