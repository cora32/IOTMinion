#include <iostream>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>
#include <c++/mutex>
#include <c++/condition_variable>
#include "randomizer/IpListParser.h"
#include "netcon/NetCon.h"
#include <boost/program_options.hpp>
#include <boost/utility/in_place_factory.hpp>
#include <future>

io_service io_service_;
io_service networkService;

/**
 * Test callback
 * @param data
 */
void printData(char *data)
{
    cout << data << endl;
}

/**
 * Called asynchronously inside thread.
 * @param longIp
 * @param port
 */
void async_callback(uint64_t longIp, const vector<unsigned short> &portVector, unsigned long timeout)
{
    NetCon netCon(&networkService);
    netCon.setTimeout(timeout);
    size_t sz = portVector.size();
    for (int iPort = 0; iPort < sz; iPort++) {
        netCon.connect(longIp, portVector[iPort], &printData);
    }
}

/**
 * Initial param structure.
 */
struct Params
{
    string ip;
    string port;
    string file;
    int threadNum;
    unsigned long timeoutMillisec;
    bool enableShuffle;
};

/**
 * Converting port string to vector of unsigned short.
 * @param port
 * @return
 */
vector<unsigned short> parsePorts(const string &port)
{
    vector<unsigned short> result;
    vector<string> portVector;
    boost::split(portVector, port, boost::is_any_of(","));

    int sz = (int) portVector.size();
    for (int i = 0; i < sz; i++) {
        if (portVector[i].find("-") != string::npos) {
            vector<string> tempVector;
            boost::split(tempVector, portVector[i], boost::is_any_of("-"));

            unsigned short startPort = (unsigned short) stoi(tempVector[0]);
            unsigned short endPort = (unsigned short) stoi(tempVector[1]);

            for (unsigned short j = startPort; j <= endPort; j++) {
                result.push_back(j);
            }
        } else {
            result.push_back((unsigned short &&) stoi(portVector[i]));
        }
    }

    return result;
}

/* Auxiliary functions for checking input for validity. */

/* Function used to check that 'opt1' and 'opt2' are not specified
   at the same time. */
void conflicting_options(const boost::program_options::variables_map &vm,
                         const char *opt1, const char *opt2)
{
    if (vm.count(opt1) && !vm[opt1].defaulted()
        && vm.count(opt2) && !vm[opt2].defaulted())
        throw logic_error(string("Conflicting options '") + opt1 + "' and '" + opt2 + "'.");
}

/* Function used to check that of 'for_what' is specified, then
   'required_option' is specified too. */
void option_dependency(const boost::program_options::variables_map &vm,
                       const char *for_what, const char *required_option)
{
    if (vm.count(for_what) && !vm[for_what].defaulted())
        if (vm.count(required_option) == 0 || vm[required_option].defaulted())
            throw logic_error(string("Option '") + for_what + "' requires option '" + required_option + "'.");
}

bool parseParams(int argc, char **argv, Params &params)
{
    string ip;
    std::string port;
    string file;
    int threads;
    unsigned long timeoutMillisec;
    bool enableShuffle;

    boost::program_options::options_description desc("Options");
    desc.add_options()
            (OPT_HELP_FULL"," OPT_HELP_SHORT, "Print this")
            (OPT_IP_FULL"," OPT_IP_SHORT, boost::program_options::value(&ip)->default_value(""), "ip address")
            (OPT_PORT_FULL"," OPT_PORT_SHORT, boost::program_options::value<std::string>(&port),
             "port(s) (80,8080,6667-6669)")
            (OPT_FILE_FULL"," OPT_FILE_SHORT, boost::program_options::value(&file)->default_value(""),
             "path to file with ip list")
            (OPT_THREADS_FULL"," OPT_THREADS_SHORT,
             boost::program_options::value(&threads)->default_value(DEFAULT_THREACOUNT), "amount of threads")
            (OPT_TIMEOUT_FULL"," OPT_TIMEOUT_SHORT,
             boost::program_options::value(&timeoutMillisec)->default_value(DEFAULT_TIMEOUT),
             "connect and receive timeout in millisec")
            (OPT_SHUFFLE_FULL"," OPT_SHUFFLE_SHORT,
             boost::program_options::bool_switch()->default_value(DEFAULT_DISABLE_SHUFFLE),
             "disable ip list shuffling");
    try {
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);

        if (vm.count(OPT_HELP_FULL)) {
            cout << desc << endl;
            return false;
        }

        conflicting_options(vm, OPT_FILE_FULL, OPT_IP_FULL);

        option_dependency(vm, OPT_IP_FULL, OPT_PORT_FULL);

        if (!vm.count(OPT_PORT_FULL)) {
            cout << "Option 'port' is required" << endl;
            return false;
        }

        if (!vm.count(OPT_THREADS_FULL)) {
            cout << "Option 'threads' is required" << endl;
            return false;
        }

        if (!vm.count(OPT_FILE_FULL)) {
            if (!vm.count(OPT_IP_FULL)) {
                cout << "Option 'ip' or 'file' is required" << endl;
                return false;
            }
        }

        cout << "ip:" << vm[OPT_IP_FULL].as<string>() << endl;
        cout << "port:" << vm[OPT_PORT_FULL].as<string>() << endl;
        cout << "threads:" << vm[OPT_THREADS_FULL].as<int>() << endl;
        cout << "shuffle disabled:" << vm[OPT_SHUFFLE_FULL].as<bool>() << endl;

        params.ip = vm[OPT_IP_FULL].as<string>();
        params.port = vm[OPT_PORT_FULL].as<string>();
        params.file = vm[OPT_FILE_FULL].as<string>();
        params.threadNum = vm[OPT_THREADS_FULL].as<int>();
        params.timeoutMillisec = vm[OPT_TIMEOUT_FULL].as < unsigned
        long > ();
        params.enableShuffle = vm[OPT_SHUFFLE_FULL].as<bool>();
    }
    catch (exception &e) {
        cerr << e.what() << endl;
//        Logger::print({USAGE});
        cout << desc << endl;
        return false;
    }

    return true;
}

int main(int argc, char **argv)
{
    /**
     * Start logger thread.
     */
    Logger::start();

    Params params;
    if (parseParams(argc, argv, params)) {
        boost::thread_group threadGroup;
        const vector<unsigned short> &portVector = parsePorts(params.port);
        /**
         * If no file specified use singe-ip mode
         */
        if (params.file.empty()) {
            uint64_t longIp = boost::asio::ip::address_v4::from_string(params.ip).to_ulong();
            io_service_.post(boost::bind(async_callback, longIp, portVector, params.timeoutMillisec));
            threadGroup.create_thread(boost::bind(&boost::asio::io_service::run, &io_service_));
        }
            /**
             * Else load ip ranges from file
             */
        else {
            IpListParser randomizer(params.file.c_str(), params.enableShuffle);
            if (!randomizer.isEmpty()) {
                while (randomizer.hasNext())
                    io_service_.post(
                            boost::bind(async_callback, randomizer.getNext(), portVector, params.timeoutMillisec));

                for (int i = 0; i < params.threadNum; ++i)
                    threadGroup.create_thread(boost::bind(&boost::asio::io_service::run, &io_service_));
            }
        }
        threadGroup.join_all();
    }

    Logger::stop();

    return 0;
}
