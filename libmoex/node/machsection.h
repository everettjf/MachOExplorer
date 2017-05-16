//
// Created by everettjf on 2017/4/2.
//

#ifndef MACHOEXPLORER_MACHSECTION_H
#define MACHOEXPLORER_MACHSECTION_H

#include "node.h"

MOEX_NAMESPACE_BEGIN



class MachSection : public NodeOffset<section>{
protected:

public:
    std::string segment_name() const {return std::string(offset_->segname,16).c_str();}
    std::string section_name() const {return std::string(offset_->sectname,16).c_str();}

    std::string GetTypeName() override{ return "section";}
    std::string GetDisplayName() override{
        return this->section_name();
    }
    std::string GetDescription() override{
        return boost::str(boost::format("section(%1%/%2%)")
                          % this->segment_name()
                          % this->section_name()
        );
    }
};
using MachSectionPtr = std::shared_ptr<MachSection>;

class MachSection64 : public NodeOffset<section_64>{
protected:

public:
    std::string segment_name() const {return std::string(offset_->segname,16).c_str();}
    std::string section_name() const {return std::string(offset_->sectname,16).c_str();}

    std::string GetTypeName() override{ return "section_64";}
    std::string GetDisplayName() override{
        return this->section_name();
    }
    std::string GetDescription() override{
        return boost::str(boost::format("%1%/%2%")
                          % this->segment_name()
                          % this->section_name()
        );
    }
};
using MachSection64Ptr = std::shared_ptr<MachSection64>;

MOEX_NAMESPACE_END

#endif //MACHOEXPLORER_MACHSECTION_H
