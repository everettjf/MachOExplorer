//
// Created by everettjf on 2017/7/5.
//

#ifndef MOEX_COMMONDISPLAY_H
#define MOEX_COMMONDISPLAY_H

#include <libmoex/moex.h>
#include <libmoex/moex-view.h>
#include "../util/BeautyTextPrint.h"

class CommonDisplay {
private:
    moex::BinaryPtr bin_;
    std::shared_ptr<BeautyTextPrinter> print_;

    std::string arch_;
public:
    bool Init(const std::string & filepath,bool is_csv);

    // Default to all archs if not given
    void set_arch(const std::string & arch){arch_ = arch;}

    // Display
    void IsFat();
    void FatList();
    void HeaderList();
    void ArchList();
    void LoadCommandList();
    void SegmentList();
    void SectionList();
    void SymbolList();
    void StringTable();
    void CryptInfo();
    void UUID();
    void DylibList();
    void Main();
    void Tree();
    void DataInCodeEntries();
    void IndirectSymbols();

    void RebaseOpcodes();
    void InternalBindInfo(moex::LoadCommand_DYLD_INFO::BindNodeType node_type);
    void BindingInfo();
    void WeakBindingInfo();
    void LazyBindingInfo();

private:
    void ForEachHeader(std::function<void(moex::MachHeaderPtr)> callback);
    void DisplayViewNode(moex::ViewNode *node,int & level);
};


#endif //MOEX_COMMONDISPLAY_H
