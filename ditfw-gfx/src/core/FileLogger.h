/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once
#include "API.h"
#include "Log.h"

#include <cstdio>

namespace dit {

    //-----------------------------------//

    class FileLogger
    {
    public:
        FileLogger(const char* fileName);
        ~FileLogger();

        void Open();
        void Print(Log::Entry* entry);
        void Close();

    public:
        FILE* file = nullptr;
        String fileName;
    };

}