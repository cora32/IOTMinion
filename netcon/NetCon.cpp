//
// Created by cora32 on 12.11.2016.
//

#include <boost/thread/thread.hpp>
#include "NetCon.h"
#include "../Logger/Logger.h"

//void
//NetCon::connect(io_service_ *pService, tcp::socket_ *socket_, uint64_t address, int port, void (*pFunction)(char *)) {
//
////    shared_ptr<tcp::socket> &socket = make_shared<tcp::socket>(*service);
////    socket = make_shared<tcp::socket>(*service);
//    boost::asio::streambuf requestBuff;
//    boost::asio::streambuf responseBuff;
//
//    ostream request_stream(&requestBuff);
//    boost::asio::ip::address_v4 addressObject(address);
//    request_stream << "GET " << "/" << " HTTP/1.0\r\n";
//    request_stream << "Host: " << addressObject.to_string() << ":" << port << "\r\n";
//    request_stream << "Accept: */*\r\n";
//    request_stream << "Connection: close\r\n\r\n";
//
//    ip::address();
//    ip::tcp::endpoint endpoint(addressObject, port);
//    tcp::resolver::iterator endpoint_iterator;
//    socket_->async_connect(endpoint,
//                       boost::bind(&NetCon::connectCallback, this, boost::asio::placeholders::error, endpoint_iterator,
//                                   socket_, &requestBuff, &responseBuff));
//    pService->poll();
//}
//void
//NetCon::connect(io_service_ *pService, uint64_t address, int port, void (*pFunction)(char *)) {
//
//    tcp::socket_ *socket_ = new tcp::socket_(*pService);
////    shared_ptr<tcp::socket> &socket = make_shared<tcp::socket>(*service);
////    socket = make_shared<tcp::socket>(*service);
//    boost::asio::streambuf requestBuff;
//    boost::asio::streambuf responseBuff;
//
//    ostream request_stream(&requestBuff);
//    boost::asio::ip::address_v4 addressObject(address);
//    request_stream << "GET " << "/" << " HTTP/1.0\r\n";
//    request_stream << "Host: " << addressObject.to_string() << ":" << port << "\r\n";
//    request_stream << "Accept: */*\r\n";
//    request_stream << "Connection: close\r\n\r\n";
//
//    ip::address();
//    ip::tcp::endpoint endpoint(addressObject, port);
//    tcp::resolver::iterator endpoint_iterator;
//    socket_->async_connect(endpoint,
//                       boost::bind(&NetCon::connectCallback, this, boost::asio::placeholders::error, endpoint_iterator,
//                                   socket_, &requestBuff, &responseBuff));
//    pService->poll();
//}

//void NetCon::check_deadline()
//{
////    if (stopped_)
////        return;
//
//    // Check whether the deadline has passed. We compare the deadline against
//    // the current time since a new asynchronous operation may have moved the
//    // deadline before this actor had a chance to run.
//    if (deadline_.expires_at() <= deadline_timer::traits_type::now())
//    {
//        // The deadline has passed. The socket_ is closed so that any outstanding
//        // asynchronous operations are cancelled.
//        socket_.close();
//
//        // There is no longer an active deadline. The expiry is set to positive
//        // infinity so that the actor takes no action until a new deadline is set.
//        deadline_.expires_at(boost::posix_time::pos_infin);
//    }
//
//    // Put the actor back to sleep.
//    deadline_.async_wait(boost::bind(&NetCon::check_deadline, this));
//}

void
NetCon::connect(uint64_t longIp, int port, void (*pFunction)(char *))
{
    service_.reset();
    socket_ = tcp::socket(service_);
    boost::asio::ip::address_v4 ipString(longIp);

    ostream request_stream(&requestBuff);
    request_stream << "GET " << "/" << " HTTP/1.0\r\n";
    request_stream << "Host: " << ipString.to_string() << ":" << port << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    socket_.async_connect(ip::tcp::endpoint(ipString, port),
                          boost::bind(&NetCon::handle_connect, this, boost::asio::placeholders::error));
    timer_.async_wait(boost::bind(&NetCon::ConnectTimeout, this, boost::asio::placeholders::error));


    Logger::print({"--", ipString.to_string(), ":", to_string(port), " Started"});
//    cout<<"=="<<ipString.to_string()<<endl;
    service_.run();
}

void NetCon::close()
{
    timer_.expires_from_now(boost::posix_time::pos_infin); // Stop the timer
    socket_.close();
};

void NetCon::ConnectTimeout(const boost::system::error_code &error)
{
    if (!error) {
        close();
    }
}

void NetCon::ReadTimeout(const boost::system::error_code &error)
{
    if (!error) {
        close();
    }
}

void NetCon::handle_connect(const boost::system::error_code &error)
{
    if (!error) {
        resetTimer();
        // Write request normally
        boost::asio::async_write(socket_, requestBuff,
                                 boost::bind(&NetCon::responseCallback, this, boost::asio::placeholders::error));
    } else {
//        std::cout << "TCP Connection failed " << error << std::endl;
//        close();
    }
}

void NetCon::connectCallback(const boost::system::error_code &err, tcp::resolver::iterator endpoint_iterator)
{
//    if (!err) {
//        boost::asio::async_write(socket_, requestBuff,
//                                 boost::bind(&NetCon::responseCallback, this, boost::asio::placeholders::error));
//    } else if (endpoint_iterator != tcp::resolver::iterator()) {
//        socket_.close();
//        tcp::endpoint endpoint = *endpoint_iterator;
//        socket_.async_connect(endpoint, boost::bind(&NetCon::connectCallback, this, boost::asio::placeholders::error,
//                                                    ++endpoint_iterator));
//    } else {
////        std::cout << "Error: connectCallback" << err.message() << "\n";
//    }
}

void NetCon::responseCallback(const boost::system::error_code &err)
{
    if (!err) {
        boost::asio::async_read(socket_, responseBuff, boost::asio::transfer_at_least(1),
                                boost::bind(&NetCon::readCallback, this,
                                            boost::asio::placeholders::error));
    }
}

void NetCon::readCallback(const boost::system::error_code &err)
{
    if (!err) {
        resetTimer();
        const char *str = boost::asio::buffer_cast<const char *>(responseBuff.data());
        Logger::print({"<--", str});

        boost::asio::async_read(socket_, responseBuff,
                                boost::asio::transfer_at_least(1),
                                boost::bind(&NetCon::readCallback, this,
                                            boost::asio::placeholders::error));
    } else if (err != boost::asio::error::eof) {
//        std::cout << "Error: readCallback" << err << "\n";
    }
}

void NetCon::setTimeout(unsigned long timeout)
{
    timeout_ = timeout;
    timer_.expires_from_now(boost::posix_time::milliseconds(timeout_)); //setting timeout
}

void NetCon::resetTimer()
{
    timer_.expires_from_now(boost::posix_time::milliseconds(timeout_)); //reset timer
    timer_.async_wait(boost::bind(&NetCon::ReadTimeout, this, boost::asio::placeholders::error));
}

