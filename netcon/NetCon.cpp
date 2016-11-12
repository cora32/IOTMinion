//
// Created by cora32 on 12.11.2016.
//

#include "NetCon.h"

void NetCon::connect(const uint64_t address, unsigned short port, void (*pFunction)(char *)) {
    tcp::socket sock(service);
    boost::asio::streambuf requestBuff;
    boost::asio::streambuf responseBuff;

    ostream request_stream(&requestBuff);
    boost::asio::ip::address_v4 addressObject(address);
    request_stream << "GET " << "/" << " HTTP/1.0\r\n";
    request_stream << "Host: " << addressObject.to_string() << ":" << port << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    ip::address();
    ip::tcp::endpoint endpoint(addressObject, port);
    tcp::resolver::iterator endpoint_iterator;
    sock.async_connect(endpoint,
                       boost::bind(&NetCon::connectCallback, this, boost::asio::placeholders::error, endpoint_iterator,
                                   &sock, &requestBuff, &responseBuff));
    service.run();
}

void NetCon::connectCallback(const boost::system::error_code &err, tcp::resolver::iterator endpoint_iterator,
                             tcp::socket *sock, boost::asio::streambuf *requestBuff,
                             boost::asio::streambuf *responseBuff) {
    if (!err) {
        boost::asio::async_write(*sock, *requestBuff,
                                 boost::bind(&NetCon::responseCallback, this, boost::asio::placeholders::error,
                                             sock, responseBuff));
    } else if (endpoint_iterator != tcp::resolver::iterator()) {
        sock->close();
        tcp::endpoint endpoint = *endpoint_iterator;
        sock->async_connect(endpoint, boost::bind(&NetCon::connectCallback, this, boost::asio::placeholders::error,
                                                  ++endpoint_iterator, sock, requestBuff, responseBuff));
    } else {
        std::cout << "Error: " << err.message() << "\n";
    }
}

void NetCon::responseCallback(const boost::system::error_code &err, tcp::socket *sock,
                              boost::asio::streambuf *responseBuff) {
    if (!err) {
        boost::asio::async_read(*sock, *responseBuff, boost::asio::transfer_at_least(1),
                                boost::bind(&NetCon::readCallback, this,
                                            boost::asio::placeholders::error,
                                            sock, responseBuff));
    } else {
        std::cout << "Error: " << err.message() << "\n";
    }
}

void NetCon::readCallback(const boost::system::error_code &err, tcp::socket *sock,
                          boost::asio::streambuf *responseBuff) {
    if (!err) {
        std::cout << responseBuff;

        boost::asio::async_read(*sock, *responseBuff,
                                boost::asio::transfer_at_least(1),
                                boost::bind(&NetCon::readCallback, this,
                                            boost::asio::placeholders::error, sock, responseBuff));
    } else if (err != boost::asio::error::eof) {
        std::cout << "Error: " << err << "\n";
    }
}
