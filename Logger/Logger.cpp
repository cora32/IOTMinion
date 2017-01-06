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
std::condition_variable Logger::cond_var;

void Logger::print(const std::initializer_list<std::string> arg_list)
{
    std::stringstream ss;
    for (auto elem : arg_list)
        ss << elem;

    std::unique_lock<std::mutex> lock(m);
    log_queue.push(ss.str());
    cond_var.notify_one();
}

void Logger::start()
{
    running = true;
    boost::thread m_thread = boost::thread(&Logger::start_);
}

void Logger::stop()
{
    running = false;
    cond_var.notify_one();
}

void Logger::start_()
{
    while (running) {
        std::unique_lock<std::mutex> lock(m);
        cond_var.wait(lock, [&]() { return !log_queue.empty(); });
        while (!log_queue.empty()) {
            std::cout << log_queue.front() << std::endl;
            log_queue.pop();
        }
    }
}
