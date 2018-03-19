#ifndef OPENFILEDIALOG_H
#define OPENFILEDIALOG_H

#include <QDialog>
#include <QListWidgetItem>


namespace Ui {
class OpenFileDialog;
}

class OpenFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenFileDialog(QWidget *parent = 0);
    ~OpenFileDialog();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);


private slots:
    void on_pushButtonSelectFile_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_recentsListWidget_itemClicked(QListWidgetItem *item);

    void on_recentsListWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_actionRemove_triggered();

    void on_actionclearAll_triggered();

private:

    bool fillRecentFilesList();

    void loadFile(const QString &filename);

    static const int MaxRecentFiles = 10;


private:
    Ui::OpenFileDialog *ui;
};

#endif // OPENFILEDIALOG_H
