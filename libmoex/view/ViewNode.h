//
// Created by everettjf on 2017/7/22.
//

#ifndef MOEX_VIEWNODE_H
#define MOEX_VIEWNODE_H

#include "DataView.h"
#include "../node/node.h"


class ViewNode {
public:

    virtual moex::Node * GetNode(){return nullptr;}
    virtual void ForEachDataView(std::function<void(DataView*)>){}
};


class FatBinaryViewNode : public ViewNode{
public:

};
class FatHeaderViewNode : public ViewNode{
public:

};
class ExecutableViewNode : public ViewNode{
public:

};

#endif //MOEX_VIEWNODE_H
