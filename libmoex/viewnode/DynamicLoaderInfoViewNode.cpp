//
// Created by everettjf on 2017/7/24.
//

#include "DynamicLoaderInfoViewNode.h"

MOEX_NAMESPACE_BEGIN




void DynamicLoaderInfoViewNode::InitViewDatas()
{
    {
        auto x = CreateTableViewDataPtr();
        x->AddRow("//todo","","","");
        AddViewData(x);
    }
}

MOEX_NAMESPACE_END
