/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#include "API.h"
#include "Core.h"
#include "FileLogger.h"

namespace dit {

    //-----------------------------------//

    FileLogger::FileLogger(const char* fn)
        : file(nullptr)
        , fileName(fn)
    {}

    //-----------------------------------//

    FileLogger::~FileLogger()
    {
        Close();
    }

    //-----------------------------------//

    void FileLogger::Open()
    {
        file = fopen(fileName.c_str(), "w");
        if (file == nullptr)
            log("Unable to open log file \"%0\".", fileName.c_str());

        auto& logger = core()->logger;
        auto printFn = [=](Log::Entry* entry) {
            Print(entry);
        };

        logger.debugHandlers.push_back(printFn);
        logger.errorHandlers.push_back(printFn);
        logger.infoHandlers.push_back(printFn);
    }

    //-----------------------------------//

    void FileLogger::Print(Log::Entry* entry)
    {
        fputs(entry->msgLine.c_str(), file);
    }
    
    //-----------------------------------//

    void FileLogger::Close()
    {
        if (file == nullptr)
            return;

        fclose(file);
        file = nullptr;
    }
}