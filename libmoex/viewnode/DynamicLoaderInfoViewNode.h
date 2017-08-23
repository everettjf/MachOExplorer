//
// Created by everettjf on 2017/7/24.
//

#ifndef MOEX_DYNAMICLOADERINFOVIEWNODE_H
#define MOEX_DYNAMICLOADERINFOVIEWNODE_H

#include "ViewNode.h"

MOEX_NAMESPACE_BEGIN

class RebaseInfoViewNode : public ViewNode {
public:
    std::string GetDisplayName()override { return "Rebase Info";}
    void InitViewDatas()override;
};

class BindingInfoViewNode : public ViewNode {
public:
    std::string GetDisplayName()override { return "Binding Info";}
    void InitViewDatas()override;
};

class WeakBindingInfoViewNode : public ViewNode {
public:
    std::string GetDisplayName()override { return "Weak Binding Info";}
    void InitViewDatas()override;
};

class LazyBindingInfoViewNode : public ViewNode {
public:
    std::string GetDisplayName()override { return "Lazy Binding Info";}
    void InitViewDatas()override;
};

class ExportInfoViewNode : public ViewNode {
public:
    std::string GetDisplayName()override { return "Export Info";}
    void InitViewDatas()override;
};

class DynamicLoaderInfoViewNode : public ViewNode{
private:
    MachHeaderPtr mh_;

    std::shared_ptr<RebaseInfoViewNode> rebase_;
    std::shared_ptr<BindingInfoViewNode> binding_;
    std::shared_ptr<WeakBindingInfoViewNode> weak_binding_;
    std::shared_ptr<LazyBindingInfoViewNode> lazy_binding_;
    std::shared_ptr<ExportInfoViewNode> export_;
public:
    void Init(MachHeaderPtr mh);
public:
    std::string GetDisplayName()override { return "Dynamic Loader Info";}
    void InitViewDatas()override;
    void ForEachChild(std::function<void(ViewNode*)> callback) override ;
};
using DynamicLoaderInfoViewNodePtr = std::shared_ptr<DynamicLoaderInfoViewNode>;

MOEX_NAMESPACE_END

#endif //MOEX_DYNAMICLOADERINFOVIEWNODE_H
