//
// Created by cora32 on 12.11.2016.
//

#ifndef ASYNC_CLIENT_NETCON_H
#define ASYNC_CLIENT_NETCON_H

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <memory>
#include <condition_variable>
#include "../constants.h"

using namespace std;
using namespace boost::asio;

using boost::asio::ip::tcp;
using boost::lambda::_1;
using boost::lambda::var;

class NetCon
{
private:
    io_service &service_;
    tcp::socket socket_;
    boost::asio::streambuf requestBuff;
    boost::asio::streambuf responseBuff;
    deadline_timer timer_;
    unsigned long timeout_;
public:
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args &&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    NetCon(boost::asio::io_service *service) : service_(*service), socket_(tcp::socket(*service)), timer_(*service),
                                               timeout_(DEFAULT_TIMEOUT)
    {}

    void connect(uint64_t address, int port, void (*pFunction)(char *));

    void ConnectTimeout(const boost::system::error_code &error);

    void ReadTimeout(const boost::system::error_code &error);

    void handle_connect(const boost::system::error_code &error);

    void setTimeout(unsigned long timeout);
private:
    void connectCallback(const boost::system::error_code &err, tcp::resolver::iterator endpoint_iterator);

    void
    responseCallback(const boost::system::error_code &err);

    void readCallback(const boost::system::error_code &err);

    void resetTimer();

    void close();
};

#endif //ASYNC_CLIENT_NETCON_H
