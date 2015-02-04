/************************************************************************
*
* Corbin Hart © (2008-201x)
* All rights reserved.
*
************************************************************************/

#pragma once

#include "API.h"
#include "Mallocator.h"

#include <string>

namespace dit {

    typedef std::basic_string<char, std::char_traits<char>, Mallocator<char>> String;

}