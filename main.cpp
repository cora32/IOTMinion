#include <iostream>
#include "randomizer/IpListParser.h"

int main(int argc, char **argv) {
    if (argc > 0) {
        IpListParser randomizer(argv[1]);
        while (randomizer.hasNext()) {
            unsigned long longIp = (unsigned long) randomizer.getNext();
//                cout << longIp << "-" << boost::asio::ip::address_v4(longIp).to_string() << endl;
        }
    }

    return 0;
}