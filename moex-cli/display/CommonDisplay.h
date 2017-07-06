//
// Created by everettjf on 2017/7/5.
//

#ifndef MOEX_COMMONDISPLAY_H
#define MOEX_COMMONDISPLAY_H

#include <libmoex/moex.h>
#include "../util/BeautyTextPrint.h"

class CommonDisplay {
private:
    moex::BinaryPtr bin_;
    std::shared_ptr<BeautyTextPrinter> print_;
public:
    bool Init(const std::string & filepath,bool is_csv);

    // Display
    void IsFat();
    void FatList();
    void HeaderList();
    void SegmentList();
    void SymbolList();

};


#endif //MOEX_COMMONDISPLAY_H
