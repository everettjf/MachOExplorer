#ifndef HEXDUMPWIDGET_H
#define HEXDUMPWIDGET_H

#include <QWidget>

namespace Ui {
class HexdumpWidget;
}

class HexdumpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HexdumpWidget(QWidget *parent = 0);
    ~HexdumpWidget();

private:
    Ui::HexdumpWidget *ui;
};

#endif // HEXDUMPWIDGET_H
