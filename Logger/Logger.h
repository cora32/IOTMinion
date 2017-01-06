//
// Created by cora32 on 06.01.2017.
//

#ifndef IOTMINION_LOGGER_H
#define IOTMINION_LOGGER_H


#include <queue>
#include <string>
#include <boost/asio/io_service.hpp>
#include <mutex>
#include <condition_variable>

class Logger
{
private:
    static std::queue<std::string> log_queue;
    static bool running;
    static std::mutex m;
    static std::condition_variable cond_var;
public:
    static void print(const std::initializer_list<std::string> arg_list);

    static void start();

    static void stop();

private:
    static void start_();
};


#endif //IOTMINION_LOGGER_H
