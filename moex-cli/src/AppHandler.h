//
// Created by everettjf on 2017/7/5.
//

#ifndef MOEX_APPHANDLER_H
#define MOEX_APPHANDLER_H

#include "util/ArgvParser.h"
#include "impl/CommonDisplay.h"

class AppHandler {
private:
    std::shared_ptr<ArgvParser> argv_;
    std::string file_path_;
    CommonDisplay display_;

    std::vector<
            std::tuple<
                    std::string,
                    std::string,
                    std::function<void()>
            >
    > actions_;
private:
    bool Prepare();
    void SetupOptions();

    void GoDisplayMode();
public:
    int Run(int argc, char* argv[]);

};


#endif //MOEX_APPHANDLER_H
