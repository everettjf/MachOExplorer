#include "OpenFileDialog.h"
#include "ui_OpenFileDialog.h"
#include <QFileDialog>
#include <QtGui>
#include <QMessageBox>
#include <QDir>
#include "utility/Utility.h"
#include "MainWindow.h"


const int OpenFileDialog::MaxRecentFiles;

static QColor getColorFor(const QString& str, int pos)
{
    Q_UNUSED(str);

    QList<QColor> Colors;
    Colors << QColor(29, 188, 156); // Turquoise
    Colors << QColor(52, 152, 219); // Blue
    Colors << QColor(155, 89, 182); // Violet
    Colors << QColor(52, 73, 94);   // Grey
    Colors << QColor(231, 76, 60);  // Red
    Colors << QColor(243, 156, 17); // Orange

    return Colors[pos % 6];

}

static QIcon getIconFor(const QString& str, int pos)
{
    // Add to the icon list
    int w = 64;
    int h = 64;

    QPixmap pixmap(w, h);
    pixmap.fill(Qt::transparent);

    QPainter pixPaint(&pixmap);
    pixPaint.setPen(Qt::NoPen);
    pixPaint.setRenderHint(QPainter::Antialiasing);
    pixPaint.setBrush(QBrush(QBrush(getColorFor(str, pos))));
    pixPaint.drawEllipse(1, 1, w - 2, h - 2);
    pixPaint.setPen(Qt::white);
    pixPaint.setFont(QFont("Verdana", 24, 1));
    pixPaint.drawText(0, 0, w, h - 2, Qt::AlignCenter, QString(str).toUpper().mid(0, 2));
    return QIcon(pixmap);
}

OpenFileDialog::OpenFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenFileDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
    setAcceptDrops(true);

    ui->recentsListWidget->addAction(ui->actionRemove);
    ui->recentsListWidget->addAction(ui->actionclearAll);

    fillRecentFilesList();

    ui->plainTextEditFilePath->setFocus();
}

OpenFileDialog::~OpenFileDialog()
{
    delete ui;
}

void OpenFileDialog::on_pushButtonSelectFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select file"), QDir::homePath());

    if (!fileName.isEmpty())
    {
        ui->plainTextEditFilePath->setPlainText(fileName);
//        ui->loadFileButton->setFocus();
    }
}

void OpenFileDialog::on_buttonBox_accepted()
{
    loadFile(ui->plainTextEditFilePath->toPlainText());
}

void OpenFileDialog::on_buttonBox_rejected()
{
    exit(0);
}

void OpenFileDialog::on_recentsListWidget_itemClicked(QListWidgetItem *item)
{
    QVariant data = item->data(Qt::UserRole);
    QString sitem = data.toString();
    ui->plainTextEditFilePath->setPlainText(sitem);
}

void OpenFileDialog::on_recentsListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    loadFile(item->data(Qt::UserRole).toString());

}

void OpenFileDialog::on_actionRemove_triggered()
{
    // Remove selected item from recents list
    QListWidgetItem *item = ui->recentsListWidget->currentItem();

    QVariant data = item->data(Qt::UserRole);
    QString sitem = data.toString();

    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(sitem);
    settings.setValue("recentFileList", files);

    ui->recentsListWidget->takeItem(ui->recentsListWidget->currentRow());

    ui->plainTextEditFilePath->clear();
}

void OpenFileDialog::on_actionclearAll_triggered()
{
    // Clear recent file list
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.clear();

    ui->recentsListWidget->clear();
    // TODO: if called from main window its ok, otherwise its not
    settings.setValue("recentFileList", files);
    ui->plainTextEditFilePath->clear();
}

bool OpenFileDialog::fillRecentFilesList()
{
    // Fill list with recent opened files
    QSettings settings;

    QStringList files = settings.value("recentFileList").toStringList();

    QMutableListIterator<QString> it(files);
    int i = 0;
    while (it.hasNext())
    {
        const QString &file = it.next();
        // Get stored files

        // Remove all but the file name
        const QString sep = QDir::separator();
        const QStringList name_list = file.split(sep);
        const QString name = name_list.last();

        // Get file info
        QFileInfo info(file);
        if (!info.exists())
        {
            it.remove();
        }
        else
        {
            QListWidgetItem *item = new QListWidgetItem(
                    getIconFor(name, i++),
                    file + "\nCreated: " + info.created().toString() + "\nSize: " + util::formatBytecount(info.size())
            );
            item->setData(Qt::UserRole, file);
            ui->recentsListWidget->addItem(item);
        }
    }

    // Removed files were deleted from the stringlist. Save it again.
    settings.setValue("recentFileList", files);

    return !files.isEmpty();
}

void OpenFileDialog::loadFile(const QString &filename)
{
//    if(!Core()->tryFile(filename, false))
//       {
//           QMessageBox msgBox(this);
//           msgBox.setText(tr("Select a new program or a previous one before continuing."));
//           msgBox.exec();
//           return;
//       }

    // Add file to recent file list
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(filename);
    files.prepend(filename);
    while (files.size() > MaxRecentFiles)
       files.removeLast();

    settings.setValue("recentFileList", files);

    // Close dialog and open MainWindow/OptionsDialog
    MainWindow *main = new MainWindow();
    main->openNewFile(filename);

    close();
}
void OpenFileDialog::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept drag & drop events only if they provide a URL
    if(event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void OpenFileDialog::dropEvent(QDropEvent *event)
{
    // Accept drag & drop events only if they provide a URL
    if(event->mimeData()->urls().count() == 0)
    {
        qWarning() << "No URL in drop event, ignoring it.";
        return;
    }

    event->acceptProposedAction();
    loadFile(event->mimeData()->urls().first().path());
}
