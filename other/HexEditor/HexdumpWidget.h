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
    void setupSelection();

private slots:

    void selectionChanged();

protected:
    void showEvent(QShowEvent *event);
private:

    void refresh(unsigned long long addr,unsigned long long maxlen);
    std::array<QString, 3> fetchHexdump(unsigned long long addr, int lines);
    void updateWidths();
    unsigned long long hexPositionToAddress(int position);
    int asciiAddressToPosition(unsigned long long address);
    void setTextEditPosition(QTextEdit *textEdit, int position);
    unsigned long long asciiPositionToAddress(int position);
    int hexAddressToPosition(unsigned long long address);

private:

    unsigned long long first_loaded_address = -1;
    unsigned long long last_loaded_address = -1;

    int m_columnCount = 16;

private:
    Ui::HexdumpWidget *ui;
};

#endif // HEXDUMPWIDGET_H
