//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef LAYOUTFILTERPROXYMODEL_H
#define LAYOUTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QString>
#include <cctype>
#include <string>
#include <libmoex/moex.h>
#include <libmoex/moex-view.h>

// Filters the layout tree by node display name and, for nodes whose view data
// has already been built, by their table cell contents. Already-parsed nodes
// are searched without forcing every node to be parsed, so lazy loading is
// preserved. Recursive filtering keeps the ancestors of any match.
class LayoutFilterProxyModel : public QSortFilterProxyModel {
public:
    explicit LayoutFilterProxyModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent) {
        setRecursiveFilteringEnabled(true);
        setFilterCaseSensitivity(Qt::CaseInsensitive);
        setFilterKeyColumn(0);
    }

    void setPattern(const QString &pattern) {
        if (pattern_ == pattern)
            return;
        pattern_ = pattern;
        invalidateFilter();
    }

    bool hasPattern() const { return !pattern_.isEmpty(); }

    // True when the node at sourceIndex is a direct match (by name, or by the
    // contents of its already-built table) — i.e. not merely a kept ancestor.
    bool nodeMatches(const QModelIndex &sourceIndex) const {
        if (pattern_.isEmpty() || !sourceIndex.isValid())
            return false;

        const QString name = sourceIndex.data(Qt::DisplayRole).toString();
        if (name.contains(pattern_, Qt::CaseInsensitive))
            return true;

        const QVariant v = sourceIndex.data(Qt::UserRole + 1);
        auto *node = reinterpret_cast<moex::ViewNode *>(v.value<void *>());
        if (node != nullptr && node->inited() && node->table()) {
            const std::string needle = pattern_.toLower().toStdString();
            const moex::TableViewData *table = node->table().get();
            for (const auto &row : table->rows) {
                for (const auto &item : row->items) {
                    if (ContainsLower(item->data, needle))
                        return true;
                }
            }
        }
        return false;
    }

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override {
        if (pattern_.isEmpty())
            return true;

        QAbstractItemModel *src = sourceModel();
        if (!src)
            return true;

        const QModelIndex idx = src->index(source_row, 0, source_parent);
        return nodeMatches(idx);
    }

private:
    static bool ContainsLower(const std::string &haystack, const std::string &needle_lower) {
        if (needle_lower.empty())
            return true;
        std::string lower;
        lower.resize(haystack.size());
        for (std::size_t i = 0; i < haystack.size(); ++i) {
            lower[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(haystack[i])));
        }
        return lower.find(needle_lower) != std::string::npos;
    }

    QString pattern_;
};

#endif // LAYOUTFILTERPROXYMODEL_H
