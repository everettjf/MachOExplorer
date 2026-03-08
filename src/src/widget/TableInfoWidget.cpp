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
    exportButton = new QPushButton(tr("Export CSV"), this);
    topBar->addWidget(filterEdit, 1);
    topBar->addWidget(filterStatus);
    topBar->addWidget(exportButton);
    layout->addLayout(topBar);

    tableView = new QTableView(this);
    layout->addWidget(tableView);

    QHeaderView *verticalHeader = tableView->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(24);

    tableView->setSelectionBehavior(QTableView::SelectRows);
    tableView->setSelectionMode(QTableView::SingleSelection);

    connect(tableView,&QTableView::clicked,this,&TableInfoWidget::clicked);
    connect(filterEdit, &QLineEdit::textChanged, this, [this](const QString &text) {
        if (proxyModel == nullptr) return;
        proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
        proxyModel->setFilterRegularExpression(QRegularExpression(QRegularExpression::escape(text),
                                                                 QRegularExpression::CaseInsensitiveOption));
        const int visible = proxyModel->rowCount();
        const int total = proxyModel->sourceModel() ? proxyModel->sourceModel()->rowCount() : visible;
        filterStatus->setText(QString("%1/%2").arg(visible).arg(total));
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

    for(uint32_t idx = 0; idx < node->widths.size(); ++idx){
        tableView->setColumnWidth(idx,node->widths.at(idx));
    }
    filterEdit->clear();
    filterStatus->setText(QString("%1/%1").arg(node->rows.size()));


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
