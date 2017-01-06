//
// Created by cora32 on 02.01.2017.
//

#ifndef IOTMINION_LOGGER_H
#define IOTMINION_LOGGER_H

#include <iostream>

#define PrintLog(token) {std::stringstream ss; ss << token; LogPrint(ss);}

template<typename TValue>
static void LogPrint(std::stringstream &s, TValue &&arg) noexcept
{
    s << std::forward<TValue>(arg);
}

static void LogPrint(stringstream &ss) noexcept
{
    std::cout << ss.str() << std::endl;
}

template<typename TValue>
static void LogPrint(TValue &&arg) noexcept
{
    std::cout << arg << std::endl;
}

#endif //IOTMINION_LOGGER_H
