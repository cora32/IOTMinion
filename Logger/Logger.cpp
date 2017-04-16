//
// Created by cora32 on 06.01.2017.
//

#include <iostream>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "Logger.h"

std::queue<std::string> Logger::log_queue;
bool Logger::running;

std::mutex Logger::m;
std::mutex Logger::return_mutex;
std::condition_variable Logger::cond_var;
std::condition_variable Logger::return_m_cond_var;

void Logger::print(const std::ostream &out) noexcept
{
    std::stringstream ss;
    ss << out.rdbuf();
    _print(ss.str());
}

void Logger::print(const std::stringstream &ss) noexcept
{
    _print(ss.str());
}

void Logger::print(const char *c_ctr) noexcept
{
    std::stringstream ss(c_ctr);
    ss << std::endl;
    _print(ss.str());
}

void Logger::print(const std::initializer_list<std::string> arg_list) noexcept
{
    std::stringstream ss;
    for (auto elem : arg_list)
        ss << elem;
    _print(ss.str());
}

void Logger::print(const boost::program_options::options_description &od) noexcept
{
    std::stringstream ss;
    ss << od;
    _print(ss.str());
}

void Logger::print(const std::string &string) noexcept
{
    _print(string);
};

void Logger::_print(const std::string &string) noexcept
{
    log_queue.push(string);
    cond_var.notify_one();
};

void Logger::start() noexcept
{
    running = true;
    boost::thread m_thread = boost::thread(&Logger::_start);
}

void Logger::stop() noexcept
{
    running = false;
    cond_var.notify_one();
    std::unique_lock<std::mutex> lock(return_mutex);
    return_m_cond_var.wait(lock, []() { return log_queue.empty(); });
}

void Logger::_start() noexcept
{
    while (running) {
        std::unique_lock<std::mutex> lock(m);
        cond_var.wait(lock, [&]() { return !log_queue.empty(); });
        while (!log_queue.empty()) {
            std::cout << log_queue.front() << std::endl;
            log_queue.pop();
        }
        return_m_cond_var.notify_one();
    }
}
