//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#include "TableInfoWidget.h"
#include "src/utility/Utility.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QHeaderView>
#include <QRegularExpression>
#include <QShortcut>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDateTime>
#include <QProcess>
#include <QProgressDialog>
#include <QTimer>
#include <QApplication>
#include <QClipboard>
#include "../controller/Workspace.h"

namespace {
class TableFilterProxyModel : public QSortFilterProxyModel {
public:
    explicit TableFilterProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override {
        if (filterRegularExpression().pattern().isEmpty()) return true;
        auto *m = sourceModel();
        if (m == nullptr) return true;
        for (int c = 0; c < m->columnCount(source_parent); ++c) {
            const QModelIndex idx = m->index(source_row, c, source_parent);
            const QString value = m->data(idx, Qt::DisplayRole).toString();
            if (value.contains(filterRegularExpression())) return true;
        }
        return false;
    }
};
}

TableInfoWidget::TableInfoWidget(QWidget *parent) : QWidget(parent)
{
    controller = nullptr;
    proxyModel = nullptr;
    filterDebounceTimer = new QTimer(this);
    filterDebounceTimer->setSingleShot(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(4);
    setLayout(layout);

    auto *topBar = new QHBoxLayout();
    topBar->setContentsMargins(4,4,4,0);
    topBar->setSpacing(6);
    filterEdit = new QLineEdit(this);
    filterEdit->setPlaceholderText(tr("Filter rows (all columns)..."));
    filterStatus = new QLabel(this);
    filterStatus->setMinimumWidth(120);
    clearFilterButton = new QPushButton(tr("Clear"), this);
    exportButton = new QPushButton(tr("Export CSV"), this);
    topBar->addWidget(filterEdit, 1);
    topBar->addWidget(filterStatus);
    topBar->addWidget(clearFilterButton);
    topBar->addWidget(exportButton);
    layout->addLayout(topBar);

    tableView = new QTableView(this);
    layout->addWidget(tableView);

    QHeaderView *verticalHeader = tableView->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(24);
    tableView->horizontalHeader()->setStretchLastSection(true);

    tableView->setSelectionBehavior(QTableView::SelectRows);
    tableView->setSelectionMode(QTableView::SingleSelection);
    tableView->setSortingEnabled(true);

    connect(tableView,&QTableView::clicked,this,&TableInfoWidget::clicked);
    connect(tableView, &QTableView::doubleClicked, this, [this](const QModelIndex &index) {
        if (!index.isValid() || proxyModel == nullptr) return;
        const QModelIndex sourceIndex = proxyModel->mapToSource(index);
        if (!sourceIndex.isValid()) return;
        openDyldCacheImageFromRow(sourceIndex);
    });
    connect(filterEdit, &QLineEdit::textChanged, this, [this](const QString &) {
        filterDebounceTimer->start(120);
    });
    connect(filterDebounceTimer, &QTimer::timeout, this, [this]() {
        if (proxyModel == nullptr) return;
        const QString text = filterEdit->text();
        proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        proxyModel->setFilterRegularExpression(QRegularExpression(QRegularExpression::escape(text),
                                                                  QRegularExpression::CaseInsensitiveOption));
        const int visible = proxyModel->rowCount();
        const int total = proxyModel->sourceModel() ? proxyModel->sourceModel()->rowCount() : visible;
        if (text.isEmpty()) {
            filterStatus->setText(QString("%1 rows").arg(total));
        } else {
            filterStatus->setText(QString("%1/%2 filtered").arg(visible).arg(total));
        }
    });

    auto *focusSearchShortcut = new QShortcut(QKeySequence::Find, this);
    connect(focusSearchShortcut, &QShortcut::activated, this, [this]() {
        filterEdit->setFocus();
        filterEdit->selectAll();
    });

    auto *clearFilterShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(clearFilterShortcut, &QShortcut::activated, this, [this]() {
        if (!filterEdit->text().isEmpty()) filterEdit->clear();
    });
    connect(clearFilterButton, &QPushButton::clicked, this, [this]() {
        if (!filterEdit->text().isEmpty()) filterEdit->clear();
    });

    auto *copyRowShortcut = new QShortcut(QKeySequence::Copy, this);
    connect(copyRowShortcut, &QShortcut::activated, this, [this]() {
        if (proxyModel == nullptr) return;
        const QModelIndex idx = tableView->currentIndex();
        if (!idx.isValid()) return;
        if (!proxyModel->mapToSource(idx).isValid()) return;
        QStringList vals;
        const int cols = proxyModel->columnCount();
        for (int c = 0; c < cols; ++c) {
            vals << proxyModel->data(proxyModel->index(idx.row(), c), Qt::DisplayRole).toString();
        }
        QGuiApplication::clipboard()->setText(vals.join('\t'));
    });

    connect(exportButton, &QPushButton::clicked, this, [this]() {
        if (proxyModel == nullptr || proxyModel->sourceModel() == nullptr) return;
        const QString outPath = QFileDialog::getSaveFileName(
                this,
                tr("Export Visible Rows to CSV"),
                QDir::home().absoluteFilePath("MachOExplorer-export.csv"),
                tr("CSV Files (*.csv)"));
        if (outPath.isEmpty()) return;
        QFile f(outPath);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            util::showError(this, tr("Cannot write file: %1").arg(outPath));
            return;
        }
        QTextStream ts(&f);
        const int cols = proxyModel->columnCount();
        const int rows = proxyModel->rowCount();
        auto csv_escape = [](QString s) {
            s.replace("\"", "\"\"");
            return QString("\"%1\"").arg(s);
        };
        QStringList headerVals;
        for (int c = 0; c < cols; ++c) {
            headerVals << csv_escape(proxyModel->headerData(c, Qt::Horizontal, Qt::DisplayRole).toString());
        }
        ts << headerVals.join(",") << "\n";
        for (int r = 0; r < rows; ++r) {
            QStringList rowVals;
            for (int c = 0; c < cols; ++c) {
                rowVals << csv_escape(proxyModel->index(r, c).data(Qt::DisplayRole).toString());
            }
            ts << rowVals.join(",") << "\n";
        }
        f.close();
        util::showInfo(this, tr("Exported %1 rows to %2").arg(rows).arg(outPath));
    });
}

TableInfoWidget::~TableInfoWidget()
{
    if (dyldRowExtractProcess_) {
        disconnect(dyldRowExtractProcess_, nullptr, this, nullptr);
        if (dyldRowExtractProcess_->state() != QProcess::NotRunning) {
            dyldRowExtractProcess_->terminate();
            if (!dyldRowExtractProcess_->waitForFinished(2000)) {
                dyldRowExtractProcess_->kill();
                dyldRowExtractProcess_->waitForFinished(2000);
            }
        }
    }
    dyldRowExtractInProgress_ = false;
    dyldRowExtractProcess_.clear();
}

void TableInfoWidget::showViewData(moex::TableViewData *data)
{
    moex::TableViewData *node = data;

    if(controller) delete controller;
    controller = new TableInfoController();

    qDebug()<<data;
    qDebug()<<node;
    qDebug()<<controller;


    controller->InitModel(node);

    qDebug()<<"after";

    if (proxyModel) delete proxyModel;
    proxyModel = new TableFilterProxyModel(this);
    proxyModel->setSourceModel(controller->model());
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    tableView->setModel(proxyModel);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->sortByColumn(0, Qt::AscendingOrder);
    if (tableView->selectionModel() != nullptr) {
        connect(tableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
                this, [this](const QModelIndex &current, const QModelIndex &) {
            if (current.isValid()) clicked(current);
        });
    }

    for(uint32_t idx = 0; idx < node->widths.size(); ++idx){
        tableView->setColumnWidth(idx,node->widths.at(idx));
    }
    filterEdit->clear();
    filterDebounceTimer->stop();
    filterStatus->setText(QString("%1 rows").arg(node->rows.size()));

    if (proxyModel->rowCount() > 0 && proxyModel->columnCount() > 0) {
        const QModelIndex first = proxyModel->index(0, 0);
        if (first.isValid()) {
            tableView->setCurrentIndex(first);
            clicked(first);
        }
    } else {
        WS()->clearHexSelection();
        WS()->setInformation(QString());
    }


}
void TableInfoWidget::clicked(const QModelIndex &index)
{
    if (!index.isValid() || proxyModel == nullptr) return;
    const QModelIndex sourceIndex = proxyModel->mapToSource(index);
    if (!sourceIndex.isValid()) return;
    qDebug()<< sourceIndex.row();

    auto row = controller->model()->data_ptr()->rows[sourceIndex.row()];
    if(row->data){
        WS()->selectHexRange(row->data,row->size);
    }else{
        WS()->clearHexSelection();
    }

    std::string desc = controller->model()->data_ptr()->GetRowDescription(sourceIndex.row());
    qDebug() << desc.c_str();
    WS()->setInformation(QString::fromStdString(desc));
}

void TableInfoWidget::openDyldCacheImageFromRow(const QModelIndex &sourceIndex)
{
    if (dyldRowExtractInProgress_) {
        util::showInfo(this, tr("An extraction task is already running."));
        return;
    }

    if (proxyModel == nullptr || proxyModel->sourceModel() == nullptr) return;
    if (sourceIndex.row() < 0) return;

    const QString cachePath = WS()->currentFilePath();
    if (cachePath.isEmpty() || !QFileInfo(cachePath).fileName().startsWith("dyld_shared_cache")) return;

    const int cols = proxyModel->sourceModel()->columnCount();
    if (cols < 3) return;
    const QString h0 = proxyModel->sourceModel()->headerData(0, Qt::Horizontal, Qt::DisplayRole).toString();
    const QString h1 = proxyModel->sourceModel()->headerData(1, Qt::Horizontal, Qt::DisplayRole).toString();
    const QString h2 = proxyModel->sourceModel()->headerData(2, Qt::Horizontal, Qt::DisplayRole).toString();
    if (h0 != "Address" || h1 != "Path Offset" || h2 != "Path") return;

    const QString imagePath = proxyModel->sourceModel()->index(sourceIndex.row(), 2).data(Qt::DisplayRole).toString().trimmed();
    if (imagePath.isEmpty() || imagePath.startsWith("(")) return;
    if (imagePath.contains('\n') || imagePath.contains('\r')) {
        util::showError(this, tr("Invalid image path: newline is not allowed."));
        return;
    }

    QString tool = QCoreApplication::applicationDirPath() + "/moex-cache-extract";
    if(!QFileInfo::exists(tool)){
        tool = QDir::current().absoluteFilePath("build/moex-cache-extract");
    }
    if(!QFileInfo::exists(tool)){
        util::showError(this, tr("Cannot find moex-cache-extract tool.\nBuild from source first."));
        return;
    }

    const QString outPath = QDir::temp().absoluteFilePath(
            QString("MachOExplorer-cache-row-%1-%2")
                    .arg(QFileInfo(imagePath).fileName())
                    .arg(QDateTime::currentDateTimeUtc().toString("yyyyMMdd-hhmmss-zzz")));

    auto *progress = new QProgressDialog(tr("Extracting selected image..."),
                                         tr("Cancel"), 0, 0, this);
    progress->setWindowModality(Qt::WindowModal);
    progress->setMinimumDuration(0);
    progress->setAutoClose(false);
    progress->setAutoReset(false);
    progress->show();

    auto *proc = new QProcess(this);
    auto *timeout = new QTimer(this);
    timeout->setSingleShot(true);
    dyldRowExtractInProgress_ = true;
    dyldRowExtractProcess_ = proc;
    WS()->addLog(tr("[extract-row] start cache=%1 selector=%2 out=%3")
                         .arg(cachePath, imagePath, outPath));

    connect(progress, &QProgressDialog::canceled, this, [this, proc]() {
        if (proc->state() != QProcess::NotRunning) proc->kill();
        WS()->addLog("[extract-row] canceled by user");
    });

    connect(timeout, &QTimer::timeout, this, [this, proc, progress]() {
        if (proc->state() == QProcess::NotRunning) return;
        progress->cancel();
        proc->kill();
        WS()->addLog("[extract-row] canceled by timeout");
    });

    connect(proc, &QProcess::finished, this,
            [this, proc, progress, timeout, outPath](int exitCode, QProcess::ExitStatus exitStatus) {
        timeout->stop();
        timeout->deleteLater();
        const QString stderrText = QString::fromUtf8(proc->readAllStandardError());
        const QString stdoutText = QString::fromUtf8(proc->readAllStandardOutput());
        const bool canceled = progress->wasCanceled();

        dyldRowExtractInProgress_ = false;
        dyldRowExtractProcess_.clear();
        progress->hide();
        progress->deleteLater();

        if (canceled) {
            QFile::remove(outPath);
            WS()->addLog("[extract-row] finished with canceled state");
            proc->deleteLater();
            return;
        }

        if (exitStatus != QProcess::NormalExit || exitCode != 0) {
            QFile::remove(outPath);
            const QString details = (stderrText + "\n" + stdoutText).trimmed();
            WS()->addLog(tr("[extract-row] failed exit=%1 status=%2 details=%3")
                                 .arg(exitCode)
                                 .arg(exitStatus == QProcess::NormalExit ? "normal" : "crash")
                                 .arg(details.isEmpty() ? "(no process output)" : details));
            util::showError(this, tr("Extraction failed:\n%1").arg(details.isEmpty() ? tr("(no process output)") : details));
            proc->deleteLater();
            return;
        }
        const QFileInfo outInfo(outPath);
        if (!outInfo.exists() || outInfo.size() <= 0) {
            QFile::remove(outPath);
            WS()->addLog(tr("[extract-row] failed output missing/empty: %1").arg(outPath));
            util::showError(this, tr("Extraction completed but output is missing or empty:\n%1").arg(outPath));
            proc->deleteLater();
            return;
        }

        WS()->openFile(outPath);
        WS()->addLog(tr("[extract-row] success out=%1 size=%2").arg(outPath).arg(outInfo.size()));
        util::showInfo(this, tr("Extracted and opened:\n%1").arg(outPath));
        proc->deleteLater();
    });

    connect(proc, &QProcess::errorOccurred, this, [this, proc, progress, timeout](QProcess::ProcessError) {
        if (!dyldRowExtractInProgress_) return;
        const QString err = proc->errorString();
        dyldRowExtractInProgress_ = false;
        dyldRowExtractProcess_.clear();
        timeout->stop();
        timeout->deleteLater();
        progress->hide();
        progress->deleteLater();
        WS()->addLog(tr("[extract-row] process start error: %1").arg(err));
        util::showError(this, tr("Failed to start extraction:\n%1").arg(err));
        proc->deleteLater();
    });

    proc->start(tool, {"--compact", cachePath, imagePath, outPath});
    timeout->start(600000);
}
