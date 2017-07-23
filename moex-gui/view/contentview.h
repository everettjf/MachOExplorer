//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef CONTENTVIEW_H
#define CONTENTVIEW_H

#include <QWidget>
#include <QTabWidget>
#include <QStackedWidget>

#include "content/tablecontentview.h"
#include "content/binarycontentview.h"
#include "content/blankcontentview.h"
#include <libmoex/node/Binary.h>


class ContentView : public QWidget
{
    Q_OBJECT
public:
    QStackedWidget *stack;
    BlankContentView *blank;

    QTabWidget *tab;
    TableContentView *table;
    BinaryContentView *binary;

    moex::Node *node;

public:
    explicit ContentView(QWidget *parent = 0);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent* event);

    void openFile(const QString & filePath);
    void showNode(moex::Node *node);

    void displayContentTab();
private:

signals:

public slots:
};

#endif // CONTENTVIEW_H
