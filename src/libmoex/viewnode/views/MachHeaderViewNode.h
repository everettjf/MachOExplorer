//
// Created by everettjf on 2017/7/23.
//

#ifndef MOEX_MACHHEADERVIEWNODE_H
#define MOEX_MACHHEADERVIEWNODE_H

#include "../ViewNode.h"
#include "libmoex/node/MachHeader.h"
#include "LoadCommandsViewNode.h"
#include "SectionsViewNode.h"
#include "DataInCodeEntriesViewNode.h"
#include "CodeSignatureViewNode.h"
#include "DynamicLoaderInfoViewNode.h"
#include "DynamicSymbolTable.h"
#include "SymbolTableViewNode.h"
#include "FunctionStartsViewNode.h"
#include "StringTableViewNode.h"
#include "TwoLevelHintsTableViewNode.h"
#include "SegmentSplitInfoViewNode.h"

MOEX_NAMESPACE_BEGIN

class MachHeaderViewNode : public ViewNode{
private:
    MachHeaderPtr d_;
    std::vector<ViewNode*> children_;

    LoadCommandsViewNodePtr load_commands_;
    SectionsViewNodePtr sections_;
    DynamicLoaderInfoViewNodePtr dynamic_loader_info_;
    FunctionStartsViewNodePtr function_starts_;
    SymbolTableViewNodePtr  symbol_table_;
    DynamicSymbolTablePtr dynamic_symbol_table_;
    DataInCodeEntriesViewNodePtr data_in_code_entries_;
    StringTableViewNodePtr string_table_;
    CodeSignatureViewNodePtr  code_signature_;
    TwoLevelHintsTableViewNodePtr twolevel_hints_table_;
    SegmentSplitInfoViewNodePtr segment_split_info_;

public:
    void Init(MachHeaderPtr d);

    std::string GetDisplayName()override;

    void ForEachChild(std::function<void(ViewNode*)> callback)override;
    void InitViewDatas()override;
};
using MachHeaderViewNodePtr = std::shared_ptr<MachHeaderViewNode>;

MOEX_NAMESPACE_END

#endif //MOEX_MACHHEADERVIEWNODE_H
