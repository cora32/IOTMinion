#include <iostream>
#include <boost/asio/ip/address_v4.hpp>
#include "randomizer/IpListParser.h"

int main(int argc, char **argv) {
    IpListParser randomizer;
    if (argc > 0) {
        randomizer.init(argv[1]);
        for (int i = 0; i < 5; i++) {
            unsigned long longIp = (unsigned long) randomizer.getNext();
            cout << longIp << "-" << boost::asio::ip::address_v4(longIp).to_string() << endl;
        }
    }

    return 0;
}