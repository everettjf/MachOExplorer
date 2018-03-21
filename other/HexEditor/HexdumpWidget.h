#ifndef HEXDUMPWIDGET_H
#define HEXDUMPWIDGET_H

#include <QWidget>
#include <array>
#include <memory>

#include <QDebug>
#include <QTextEdit>
#include <QDockWidget>
#include <QMouseEvent>



namespace Ui {
class HexdumpWidget;
}

class HexdumpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HexdumpWidget(QWidget *parent = 0);
    ~HexdumpWidget();

    void setupFont();
    void setupColors();
    void updateHeaders();
    void setupScrollSync();
private slots:

    void selectionChanged();
    void scrollChanged();

protected:
    void showEvent(QShowEvent *event);
private:

    void connectScroll(bool disconnect_);
    void refresh(unsigned long long addr,unsigned long long maxlen);
    std::array<QString, 3> fetchHexdump(unsigned long long addr, int lines);
    void updateWidths();

private:
    int  bufferLines;

    unsigned long long first_loaded_address = -1;
    unsigned long long last_loaded_address = -1;


private:
    Ui::HexdumpWidget *ui;

    bool scroll_disabled = false;

};

#endif // HEXDUMPWIDGET_H
