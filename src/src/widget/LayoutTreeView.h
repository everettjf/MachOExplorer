//
// Created by everettjf on 2018/3/29.
//

#ifndef MACHOEXPLORER_LAYOUTTREEVIEW_H
#define MACHOEXPLORER_LAYOUTTREEVIEW_H

#include <QTreeView>


class LayoutTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit LayoutTreeView(QWidget *parent = 0);
    QSize sizeHint() const;

};

#endif //MACHOEXPLORER_LAYOUTTREEVIEW_H
