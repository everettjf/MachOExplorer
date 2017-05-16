//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//

#include "nodeview.h"

MOEX_NAMESPACE_BEGIN

void TableNodeView::SetHeaders(const std::vector<std::string> &headers)
{
    header_.clear();
    for(const auto & title : headers){
        HeaderCellPtr h = std::make_shared<HeaderCell>();
        h->title = title;
        header_.push_back(h);
    }
}

TableNodeView::Row::Row(std::initializer_list<std::string> l)
{
    cells.clear();
    for(auto iter = l.begin(); iter != l.end(); ++iter){
        CellPtr cell = std::make_shared<Cell>();
        cell->value = *iter;
        cells.push_back(cell);
    }
}

TableNodeView::Row::Row(const std::vector<std::string> &l)
{
    cells.clear();
    for(auto iter = l.begin(); iter != l.end(); ++iter){
        CellPtr cell = std::make_shared<Cell>();
        cell->value = *iter;
        cells.push_back(cell);
    }
}

void TableNodeView::SetTable(std::initializer_list<std::vector<std::string> > l)
{
    table_.clear();
    for(auto row = l.begin(); row != l.end(); ++row){
        RowPtr r(new Row(*row));
        table_.push_back(r);
    }
}

MOEX_NAMESPACE_END
