//
// Created by everettjf on 2018/3/29.
//

#ifndef MACHOEXPLORER_LAYOUTTREEVIEW_H
#define MACHOEXPLORER_LAYOUTTREEVIEW_H

#include <QKeyEvent>
#include <QModelIndex>
#include <QTreeView>


class LayoutTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit LayoutTreeView(QWidget *parent = 0);
    QSize sizeHint() const override;

signals:
    void keyboardNavigationActivated(const QModelIndex &index);

protected:
    void keyPressEvent(QKeyEvent *event) override;

};

#endif //MACHOEXPLORER_LAYOUTTREEVIEW_H
