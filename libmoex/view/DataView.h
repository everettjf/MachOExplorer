//
// Created by everettjf on 2017/7/22.
//

#ifndef MOEX_DATAVIEW_H
#define MOEX_DATAVIEW_H

#include "../node/common.h"

class DataView {
public:


};
class BinaryDataView : public DataView{
public:
    char * offset;
    uint64_t length;

};
class TableDataView : public DataView{
public:
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;

};


#endif //MOEX_DATAVIEW_H
