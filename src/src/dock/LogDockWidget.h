//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef LOGVIEW_H
#define LOGVIEW_H

#include <QDockWidget>
#include <QTextEdit>

class LogDockWidget : public QDockWidget
{
    Q_OBJECT
private:
    QTextEdit *textEdit;
public:
    explicit LogDockWidget(QWidget *parent = 0);

    void addLine(const QString &line);

signals:

public slots:
};

#endif // LOGVIEW_H
