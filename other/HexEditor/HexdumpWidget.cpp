#include "HexdumpWidget.h"
#include "ui_HexdumpWidget.h"

HexdumpWidget::HexdumpWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HexdumpWidget)
{
    ui->setupUi(this);
}

HexdumpWidget::~HexdumpWidget()
{
    delete ui;
}
