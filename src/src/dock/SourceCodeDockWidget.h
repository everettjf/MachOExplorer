//
// Created by everettjf on 2018/3/30.
//

#ifndef MACHOEXPLORER_SOURCEDOCKWIDGET_H
#define MACHOEXPLORER_SOURCEDOCKWIDGET_H


#include <QDockWidget>
#include <QTextEdit>

class SourceCodeDockWidget : public QDockWidget
{
Q_OBJECT
private:
    QTextEdit *textEdit;
public:
    explicit SourceCodeDockWidget(QWidget *parent = 0);

    void setContent(const QString &line);

signals:

public slots:
};
#endif //MACHOEXPLORER_SOURCEDOCKWIDGET_H
