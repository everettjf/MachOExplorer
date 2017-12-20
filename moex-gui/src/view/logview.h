//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef LOGVIEW_H
#define LOGVIEW_H

#include <QWidget>
#include <QTextEdit>
#include "../controller/windowshareddata.h"

class LogView : public QWidget , public WindowSharedData
{
    Q_OBJECT
private:
    QTextEdit *textEdit;
public:
    explicit LogView(QWidget *parent = 0);

    void addLine(const QString &line);
    QSize sizeHint() const;

signals:

public slots:
};

#endif // LOGVIEW_H
