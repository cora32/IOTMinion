//
// Created by cora32 on 04.09.2016.
//

#include <c++/fstream>
#include <c++/algorithm>
#include "IpListParser.h"
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <c++/regex>

/**
 * Loads initial vector and removes comments
 * @param filename
 * @return
 */
const vector<pair<uint64_t, uint64_t> > IpListParser::loadVector(const char *filename)
{
    Logger::print({"Loading ", filename});
    ifstream inputFile(filename);
    vector<string> ipStringVector;
    string line;

    while (getline(inputFile, line)) {
        boost::trim(line);
        if (!line.empty()) {
            if (line.at(0) != '#' && line.at(0) != '/')
                ipStringVector.push_back(line);
            else
                Logger::print({"Comment: ", line});
        }
    }

    const vector<pair<uint64_t, uint64_t> > &ipPairVector = parseIPVector(ipStringVector);

    return ipPairVector;
}

/**
 * Deletes duplicates and nested ranges
 *
 * @param ipVector
 * @return
 */
vector<pair<uint64_t, uint64_t> *> IpListParser::trimVector(vector<pair<uint64_t, uint64_t> > &ipVector)
{
    vector<pair<uint64_t, uint64_t> *> result;
    if (ipVector.size() > 0) {
        sort(ipVector.begin(), ipVector.end());

        pair<uint64_t, uint64_t> pair1 = ipVector[0];
        pair<uint64_t, uint64_t> *newPair = new pair<uint64_t, uint64_t>(pair1);
        result.push_back(newPair);
        size_t sz = ipVector.size();
        for (int i = 0; i < sz; i++) {
            pair1 = ipVector[i];
            size_t sz2 = result.size();
            for (size_t j = 0; j < sz2; j++) {
                pair<uint64_t, uint64_t> *pair2 = result[j];
                if (pair1.first == pair2->first) {
                    if (pair1.second >= pair2->second)
                        pair2->second = pair1.second;
                    break;
                } else {
                    size_t sz3 = result.size();
                    for (size_t k = j; k < sz3; k++) {
                        pair<uint64_t, uint64_t> *pair3 = result[k];
                        if (pair1.first <= pair3->second) {
                            if (pair1.second > pair3->second)
                                pair3->second = pair1.second;
                            goto haters_gonna_hate;
                        } else if (pair1.first == pair3->second + 1) {
                            pair3->second = pair1.second;
                            goto haters_gonna_hate;
                        }
                    }
                    pair<uint64_t, uint64_t> *newPair = new pair<uint64_t, uint64_t>(pair1);
                    result.push_back(newPair);
                }
            }
            haters_gonna_hate:;
        }
    }
    return result;
}

/**
 * Parses range strings and creates intermediate vector<pair<uint64_t, uint64_t> >
 * @param ipVector
 * @return
 */
const vector<pair<uint64_t, uint64_t> > IpListParser::parseIPVector(vector<string> ipVector)
{
    vector<pair<uint64_t, uint64_t> > ipPairVector;

    std::regex patternIPv4("(\\d{1,3}(\\.\\d{1,3}){3})");
    size_t sz = ipVector.size();
    for (int i = 0; i < sz; i++) {
        string ipStr = ipVector[i];
        /**
         * It is an ordinary range string ip-ip
         */
        if (ipStr.find("-") != string::npos) {
            vector<string> result;
            boost::split(result, ipStr, boost::is_any_of("-"));

            std::smatch match;
            if (!std::regex_search(result[0], match, patternIPv4) ||
                !std::regex_search(result[1], match, patternIPv4)) {
                Logger::print(std::stringstream() << " [-]Skipping " + ipStr + " Line " << i + 1 << endl);
                continue;
            }

            //IPv4
            uint64_t longAddress1 = boost::asio::ip::address_v4::from_string(result[0]).to_ulong();
            uint64_t longAddress2 = boost::asio::ip::address_v4::from_string(result[1]).to_ulong();

            if (longAddress1 > longAddress2) {
                Logger::print(std::stringstream() << " [-]Skipping " + ipStr + " Line " << i + 1 << endl);
            } else {
                pair<uint64_t, uint64_t> ipPair(longAddress1, longAddress2);
                ipPairVector.push_back(ipPair);
            }
        }
            /**
             * It is rnge in CIDR notation
             */
        else if (ipStr.find("/") != string::npos) {
            vector<string> result;
            boost::split(result, ipStr, boost::is_any_of("/"));

            std::smatch match;
            if (!std::regex_search(result[0], match, patternIPv4)) {
                Logger::print(std::stringstream() << " [-]Skipping " + ipStr + " Line " << i + 1 << endl);
                continue;
            }
            int cidrRange = stoi(result[1]);
            if (cidrRange < 1 || cidrRange > 32) {
                Logger::print(std::stringstream() << " [-]Skipping " + ipStr + " Line " << i + 1 << endl);
                continue;
            }

            uint64_t maxIPv4 = ULONG_MAX;
            uint64_t netmask = maxIPv4 << (32 - cidrRange);
            uint64_t wildcard = maxIPv4 >> cidrRange;

            uint64_t longAddress1 = boost::asio::ip::address_v4::from_string(result[0]).to_ulong() & netmask;
            uint64_t longAddress2 = longAddress1 | wildcard;

            pair<uint64_t, uint64_t> ipPair(longAddress1, longAddress2);
            ipPairVector.push_back(ipPair);
        }
            /**
             * It is a single ip string
             */
        else {
            uint64_t longAddress = boost::asio::ip::address_v4::from_string(ipStr).to_ulong();
            pair<uint64_t, uint64_t> ipPair(longAddress, longAddress);
            ipPairVector.push_back(ipPair);
        }
    }

    return ipPairVector;
}

/**
 * Fills internal ipList with actual ip in decimal presentation and then shuffles ipList
 * @param _vector
 * @param startIndex
 * @param initialOffset
 */
void
IpListParser::fillVectorAndRandomize(vector<pair<uint64_t, uint64_t> *> _vector, size_t startIndex, int initialOffset)
{
    fillInnerVector(_vector, startIndex, initialOffset);
    if (!disableShuffle)
        std::shuffle(std::begin(ipList), std::end(ipList), engine);
}

/**
 * Fills inner ipList with first @INITIAL_CAPACITY items. Then using @initialOffset gets next chunk of data.
 * Unused data chunks are stored in @TEMP_FILENAME
 * @param _vector
 * @param startIndex
 * @param initialOffset
 */
void IpListParser::fillInnerVector(vector<pair<uint64_t, uint64_t> *> _vector, size_t startIndex, int initialOffset)
{
    atomicIndex = 0;
    ipList.clear();
    if (_vector.size() > 0 && startIndex < _vector.size()) {
        const pair<uint64_t, uint64_t> *pair = _vector[startIndex];
        int initOffset = initialOffset;
        for (uint64_t j = pair->first + initialOffset; j <= pair->second; ++j, ++initOffset) {
            if (ipList.size() == INITIAL_CAPACITY) {
                saveRestToFile(_vector, startIndex, j, pair->second, initOffset);
                return;
            }
            ipList.push_back(j);
        }
        initOffset = 0;
        size_t sz = _vector.size();
        for (size_t i = startIndex + 1; i < sz; ++i) {
            pair = _vector[i];
            for (uint64_t j = pair->first; j <= pair->second; ++j, ++initOffset) {
                if (ipList.size() == INITIAL_CAPACITY) {
                    saveRestToFile(_vector, i, j, pair->second, initOffset);
                    return;
                }
                ipList.push_back(j);
            }
            initOffset = 0;
        }
        lastVectorIndex = sz;
        lastVectorOffset = initOffset;
        boost::filesystem::remove(boost::filesystem::absolute(std::string(TEMP_FILENAME)));
    }
}

bool IpListParser::isEmpty()
{
    return ipList.empty();
}

/**
 * Checks if internal ipList has data.
 * If ipList is depleted it is refilled with @INITIAL_CAPACITY items from @TEMP_FILENAME
 * @return
 */
bool IpListParser::hasNext()
{
    if (atomicIndex != ipList.size()) {
        return true;
    }
    size_t sz = _vector.size() - 1;
    if (lastVectorIndex < sz) {
        fillVectorAndRandomize(_vector, lastVectorIndex, lastVectorOffset);
        return true;
    }
    if (lastVectorIndex == sz) {
        pair<uint64_t, uint64_t> *pair = _vector[lastVectorIndex];
        int range = (int) (pair->second - pair->first);
        if (lastVectorOffset <= range) {
            fillVectorAndRandomize(_vector, lastVectorIndex, lastVectorOffset);
            return true;
        } else return false;
    }

    return false;
}

const uint64_t IpListParser::getNext()
{
    return ipList[atomicIndex++];
}

/**
 * Stores unused chunks to @TEMP_FILENAME
 * @param _vector
 * @param vectorIndex
 * @param ipFirst
 * @param ipSecond
 * @param initOffset
 */
void
IpListParser::saveRestToFile(const vector<pair<uint64_t, uint64_t> *> _vector, size_t vectorIndex, uint64_t ipFirst,
                             uint64_t ipSecond, int initOffset)
{
    std::ofstream output(TEMP_FILENAME);

    lastVectorIndex = vectorIndex;
    lastVectorOffset = initOffset;

    if (!output.fail()) {
        output << boost::asio::ip::address_v4((unsigned long) ipFirst).to_string() << "-"
               << boost::asio::ip::address_v4((unsigned long) ipSecond).to_string() << endl;
    }

    size_t sz = _vector.size();
    for (size_t i = vectorIndex + 1; i < sz; ++i) {
        const pair<uint64_t, uint64_t> *pair = _vector[i];
        if (!output.fail()) {
            output << boost::asio::ip::address_v4((unsigned long) pair->first).to_string() << "-"
                   << boost::asio::ip::address_v4((unsigned long) pair->second).to_string() << endl;
        }
    }
}




