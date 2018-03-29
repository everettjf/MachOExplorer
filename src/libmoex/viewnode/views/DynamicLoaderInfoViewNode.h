//
// Created by everettjf on 2017/7/24.
//

#ifndef MOEX_DYNAMICLOADERINFOVIEWNODE_H
#define MOEX_DYNAMICLOADERINFOVIEWNODE_H

#include "../ViewNode.h"
#include "../../node/loadcmd/LoadCommand_DYLD_INFO.h"

MOEX_NAMESPACE_BEGIN

class DyldInfoViewNodeBase : public ViewNode{
protected:
    moex::LoadCommand_DYLD_INFO *info_ = nullptr;
public:
    void Init(LoadCommand_DYLD_INFO *info);

    void InitBindInfo(moex::LoadCommand_DYLD_INFO::BindNodeType node_type);
};

class RebaseInfoViewNode : public DyldInfoViewNodeBase {
public:
    std::string GetDisplayName()override { return "Rebase Info";}
    void InitViewDatas()override;
};

class BindingInfoViewNode : public DyldInfoViewNodeBase  {
public:
    std::string GetDisplayName()override { return "Binding Info";}
    void InitViewDatas()override;
};

class WeakBindingInfoViewNode : public DyldInfoViewNodeBase {
public:
    std::string GetDisplayName()override { return "Weak Binding Info";}
    void InitViewDatas()override;
};

class LazyBindingInfoViewNode : public DyldInfoViewNodeBase {
public:
    std::string GetDisplayName()override { return "Lazy Binding Info";}
    void InitViewDatas()override;
};

class ExportInfoViewNode : public DyldInfoViewNodeBase {
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
