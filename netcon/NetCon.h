//
// Created by cora32 on 12.11.2016.
//

#ifndef ASYNC_CLIENT_NETCON_H
#define ASYNC_CLIENT_NETCON_H

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace std;
using namespace boost::asio;
using boost::asio::ip::tcp;

class NetCon {
public:
    io_service service;

    void connect(const uint64_t address, unsigned short port, void (*pFunction)(char *));

private:
    void connectCallback(const boost::system::error_code &err, tcp::resolver::iterator endpoint_iterator,
                         tcp::socket *sock, boost::asio::streambuf *requestBuff,
                         boost::asio::streambuf *responseBuff);

    void
    responseCallback(const boost::system::error_code &err, tcp::socket *sock, boost::asio::streambuf *responseBuff);

    void readCallback(const boost::system::error_code &err, tcp::socket *sock, boost::asio::streambuf *responseBuff);
};

#endif //ASYNC_CLIENT_NETCON_H
