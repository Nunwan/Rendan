#pragma once

#include "Logger.hpp"
#include <iostream>

inline bool isInDebug()
{
#ifndef NDEBUG
    return true;
#else
    return false;
#endif
}
