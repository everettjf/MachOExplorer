//
// Created by everettjf on 2018/3/28.
//

#ifndef MACHOEXPLORER_HELPDOCKWIDGET_H
#define MACHOEXPLORER_HELPDOCKWIDGET_H


#include <QDockWidget>
#include <QTextEdit>

class InformationDockWidget : public QDockWidget
{
    Q_OBJECT
private:
    QTextEdit *textEdit;
public:
    explicit InformationDockWidget(QWidget *parent = 0);

    void setContent(const QString &line);

signals:

public slots:
};

#endif //MACHOEXPLORER_HELPDOCKWIDGET_H
