#include <iostream>
#include <boost/asio/ip/address_v4.hpp>
#include "randomizer/IpListParser.h"
#include "netcon/NetCon.h"

void printData(char *data) {
    cout << data << endl;
}

int main(int argc, char **argv) {
    if (argc > 0) {
        IpListParser randomizer(argv[1]);
        NetCon netCon;
        while (randomizer.hasNext()) {
            uint64_t longIp = (unsigned long) randomizer.getNext();
            cout << boost::asio::ip::address_v4(longIp).to_string() << endl;
            netCon.connect(longIp, 7833, &printData);
        }
    }

    return 0;
}