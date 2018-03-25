#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    std::string str;
//    str = "hello world";
//    str = "hello world hello world";
    for(int i = 0; i < 1000; i++){
        str += "12345678";
    }


    unsigned long long addr = (unsigned long long)(void*)str.c_str();
    unsigned long long len = str.length();

    ui->widget->setAddressAs64Bit(false);
    ui->widget->loadAddress(addr,addr, len);
    ui->widget->selectRange(addr,6);
}
