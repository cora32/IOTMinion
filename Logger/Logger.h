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
#include <boost/program_options/options_description.hpp>

class Logger
{
private:
    static std::queue<std::string> log_queue;
    static bool running;
    static std::mutex m;
    static std::mutex return_mutex;
    static std::condition_variable cond_var;
    static std::condition_variable return_m_cond_var;
public:
    static void print(const std::ostream &out) noexcept;

    static void print(const std::stringstream &ss) noexcept;

    static void print(const char *c_ctr) noexcept;

    static void print(const std::string &string) noexcept;

    static void print(const std::initializer_list<std::string> arg_list) noexcept;

    static void print(const boost::program_options::options_description &od) noexcept;

    static void start() noexcept;

    static void stop() noexcept;

private:
    static void _start() noexcept;

    static void _print(const std::string &string) noexcept;
};


#endif //IOTMINION_LOGGER_H
