//
//  Created by everettjf
//  Copyright © 2017 everettjf. All rights reserved.
//
#ifndef VIEW_H
#define VIEW_H


#include "common.h"

MOEX_NAMESPACE_BEGIN

enum class NodeViewType{
    Address,
    Table,
};

class NodeView{
protected:
    std::string title_;
    NodeViewType type_;
public:
    const std::string & title()const{return title_;}
    const NodeViewType & type()const{return type_;}
};
using NodeViewPtr = std::shared_ptr<NodeView>;
using NodeViewArray = std::vector<NodeViewPtr>;


class AddressNodeView : public NodeView{
protected:
    void *offset_;
    std::size_t length_;

public:
    AddressNodeView(void *offset,std::size_t length)
        :offset_(offset),length_(length){
        type_ = NodeViewType::Address;
    }

    void *offset(){return offset_;}
    std::size_t length()const{return length_;}

};
using AddressNodeViewPtr = std::shared_ptr<AddressNodeView>;

class TableNodeView : public NodeView{
public:
    class HeaderCell{
    public:
        std::string title;
    };
    using HeaderCellPtr = std::shared_ptr<HeaderCell>;
    using Header = std::vector<HeaderCellPtr>;

    class Cell{
    public:
        std::string value;
    };
    using CellPtr = std::shared_ptr<Cell>;

    class Row{
    public:
        std::vector<CellPtr> cells;
        Row(){}
        Row(std::initializer_list<std::string> l);
        Row(const std::vector<std::string> &l);
    };
    using RowPtr = std::shared_ptr<Row>;

    using Table = std::vector<RowPtr>;

protected:
    Table table_;
    Header header_;

public:
    TableNodeView(){
        type_ = NodeViewType::Table;
    }

    const Table & table()const{return table_;}
    Table & table_ref(){return table_;}
    // helper
    void SetTable(std::initializer_list<std::vector<std::string>> l);

    const Header & header()const{return header_;}
    Header & header_ref(){return header_;}
    void set_header(const Header &header){header_ = header;}

    void SetHeaders(const std::vector<std::string> & headers);
};
using TableNodeViewPtr = std::shared_ptr<TableNodeView>;


MOEX_NAMESPACE_END

#endif // VIEW_H
