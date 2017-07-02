//
// Created by everettjf on 2017/7/2.
//

#ifndef MOEX_BEAUTYTEXTPRINT_H
#define MOEX_BEAUTYTEXTPRINT_H

#include <vector>
#include <string>
#include <initializer_list>
#include <memory>
#include <iostream>
#include <numeric>
#include <iomanip>


/*
+--------------------------------+
|       One|       Two|     Three|
+--------------------------------+
|         1|         2|         3|
|         1|         2|      3333|
|         1|         2|         3|
|         1|         2|         3|
+--------------------------------+

auto print = BeautyTextPrinterFactory::CreatePrinter(BeautyTextPrinterFactory::Table);
//auto print = BeautyTextPrinterFactory::CreatePrinter(BeautyTextPrinterFactory::CSV);
print->SetHeaders({"One","Two","Three"});
print->Begin();
print->AddRow({"1","2","3"});
print->AddRow({"1","2","3333"});
print->AddRow({"1","2","3"});
print->AddRow({"1","2","3"});
print->End();

 */

class BeautyTextPrinter{
protected:
    std::vector<std::string> headers_;
    std::vector<int> widths_;

public:
    BeautyTextPrinter(){}
    ~BeautyTextPrinter(){}
    void SetHeaders(std::initializer_list<std::string> l){
        widths_.clear();
        headers_.clear();
        for(auto iter = l.begin(); iter != l.end(); ++iter){
            headers_.push_back(*iter);
            widths_.push_back(10); // default to 10 width
        }
    }

    void SetWidths(std::initializer_list<int> l){
        widths_.clear();
        for(auto iter = l.begin(); iter != l.end(); ++iter){
            widths_.push_back(*iter);
        }
    }

    virtual void Begin(){}
    virtual void End(){}
    virtual void AddRow(std::initializer_list<std::string> l){};

};

class TableTextPrinter : public BeautyTextPrinter{

protected:

    void Line(){
        int extra_width = headers_.size() + 1;
        int content_width = std::accumulate(widths_.begin(),widths_.end(),0);
        int total_width = extra_width + content_width;

        std::cout << '+';
        for(int i = 1; i <= total_width - 2; ++i){
            std::cout << '-';
        }
        std::cout << '+';
        std::cout << std::endl;
    }
public:
    TableTextPrinter(){}
    ~TableTextPrinter(){}

    void Begin() override {
        Line();
        int idx = 0;
        for(auto & name : headers_){
            std::cout<<'|';
            int width = widths_[idx];
            std::cout<<std::setw(width)<<name;

            ++idx;
        }

        std::cout<<'|'<<std::endl;
        Line();
    }

    void End() override {
        Line();
    }

    void AddRow(std::initializer_list<std::string> l) override {
        int idx = 0;
        for(auto iter = l.begin(); iter != l.end(); ++iter){
            std::cout<<'|';
            int width = widths_[idx];
            std::cout << std::setw(width) << std::right << *iter;
            ++idx;
        }
        std::cout<<'|'<<std::endl;
    }

};

class CSVTextPrinter : public BeautyTextPrinter{
protected:

public:

    void Begin() override {
        int idx = 0;
        for(auto & name : headers_){
            std::cout<<name;
            if(idx != headers_.size()-1)
                std::cout<<',';
            ++idx;
        }
        std::cout<<std::endl;
    }

    void End() override {
    }

    void AddRow(std::initializer_list<std::string> l) override {
        int idx = 0;
        for(auto iter = l.begin(); iter != l.end(); ++iter){
            std::cout << *iter;
            if(idx != headers_.size()-1)
                std::cout<<',';
            ++idx;
        }
        std::cout<<std::endl;
    }

};

class BeautyTextPrinterFactory{
public:
    enum PrinterType{
        Table,
        CSV,
    };

    static std::shared_ptr<BeautyTextPrinter> CreatePrinter(PrinterType type = Table){
        if(type == CSV){
            return std::make_shared<CSVTextPrinter>();
        }
        return std::make_shared<TableTextPrinter>();
    }
};

#endif //MOEX_BEAUTYTEXTPRINT_H
