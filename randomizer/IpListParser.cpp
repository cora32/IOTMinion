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

const vector<pair<uint64_t, uint64_t> > IpListParser::loadVector(const char *filename) {
    cout << "Loading " << filename << endl;
    ifstream inputFile(filename);
    vector<string> ipStringVector;
    string line;

    while (getline(inputFile, line)) {
        boost::trim(line);
        if (!line.empty()) {
            if (line.at(0) != '#' && line.at(0) != '/')
                ipStringVector.push_back(line);
            else
                cout << "Comment: " << line << endl;
        }
    }

    const vector<pair<uint64_t, uint64_t> > &ipPairVector = parseIPVector(ipStringVector);

    return ipPairVector;
}

vector<pair<uint64_t, uint64_t> *> IpListParser::trimVector(vector<pair<uint64_t, uint64_t> > &ipVector) {
    vector<pair<uint64_t, uint64_t> *> result;
    if (ipVector.size() > 0) {
        sort(ipVector.begin(), ipVector.end());

        result.push_back(new pair<uint64_t, uint64_t>(ipVector[0]));
        int sz = ipVector.size();
        for (int i = 0; i < sz; i++) {
            pair<uint64_t, uint64_t> pair1 = ipVector[i];
            int sz2 = result.size();
            for (vector<pair<uint64_t, uint64_t> >::size_type j = 0; j < sz2; j++) {
                pair<uint64_t, uint64_t> *pair2 = result[j];
                if (pair1.first == pair2->first) {
                    if (pair1.second >= pair2->second)
                        pair2->second = pair1.second;
                    break;
                } else {
                    int sz3 = result.size();
                    for (vector<pair<uint64_t, uint64_t> >::size_type k = j; k < sz3; k++) {
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

const vector<pair<uint64_t, uint64_t> > IpListParser::parseIPVector(vector<string> ipVector) {
    vector<pair<uint64_t, uint64_t> > ipPairVector;

    std::regex patternIPv4("(\\d{1,3}(\\.\\d{1,3}){3})");
    int sz = ipVector.size();
    for (int i = 0; i < sz; i++) {
        string ipStr = ipVector[i];
        if (ipStr.find("-") != string::npos) {
            vector<string> result;
            boost::split(result, ipStr, boost::is_any_of("-"));

            std::smatch match;
            if (!std::regex_search(result[0], match, patternIPv4) ||
                !std::regex_search(result[1], match, patternIPv4)) {
                cout << " [-]Skipping " + ipStr + " Line " << i + 1 << endl;
                continue;
            }

            //IPv4
            uint64_t longAddress1 = boost::asio::ip::address_v4::from_string(result[0]).to_ulong();
            uint64_t longAddress2 = boost::asio::ip::address_v4::from_string(result[1]).to_ulong();

            if (longAddress1 > longAddress2) {
//            std::cout << colorSet( h, DARKTEAL )<<" I am coloured!!" << colorSet( h, GRAY ) <<  std::endl;
                cout << " [-]Skipping " + ipStr + " Line " << i + 1 << endl;
            } else {
                pair<uint64_t, uint64_t> ipPair(longAddress1, longAddress2);
                ipPairVector.push_back(ipPair);
            }
        } else if (ipStr.find("/") != string::npos) {
            vector<string> result;
            boost::split(result, ipStr, boost::is_any_of("/"));

            std::smatch match;
            if (!std::regex_search(result[0], match, patternIPv4)) {
                cout << " [-]Skipping " + ipStr + " Line " << i + 1 << endl;
                continue;
            }
            int cidrRange = stoi(result[1]);
            if (cidrRange < 1 || cidrRange > 32) {
                cout << " [-]Skipping " + ipStr + " Line " << i + 1 << endl;
                continue;
            }

            uint64_t maxIPv4 = ULONG_MAX;
            uint64_t netmask = maxIPv4 << (32 - cidrRange);
            uint64_t wildcard = maxIPv4 >> cidrRange;

            uint64_t longAddress1 = boost::asio::ip::address_v4::from_string(result[0]).to_ulong() & netmask;
            uint64_t longAddress2 = longAddress1 | wildcard;

            pair<uint64_t, uint64_t> ipPair(longAddress1, longAddress2);
            ipPairVector.push_back(ipPair);
        } else {
            uint64_t longAddress = boost::asio::ip::address_v4::from_string(ipStr).to_ulong();
            pair<uint64_t, uint64_t> ipPair(longAddress, longAddress);
            ipPairVector.push_back(ipPair);
        }
    }

    return ipPairVector;
}

void
IpListParser::fillVectorAndRandomize(vector<pair<uint64_t, uint64_t> *> _vector, int startIndex, int initialOffset) {
    fillInnerVector(_vector, startIndex, initialOffset);
    std::shuffle(std::begin(ipList), std::end(ipList), engine);
}

void IpListParser::fillInnerVector(vector<pair<uint64_t, uint64_t> *> _vector, int startIndex, int initialOffset) {
    atomicIndex = 0;
    ipList.clear();
    int sz = _vector.size();
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
        for (int i = startIndex + 1; i < sz; ++i) {
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

bool IpListParser::hasNext() {
    if (atomicIndex != ipList.size()) {
        return true;
    }
    int sz = _vector.size() - 1;
    if (lastVectorIndex < sz) {
        fillVectorAndRandomize(_vector, lastVectorIndex, lastVectorOffset);
        return true;
    }
    if (lastVectorIndex == sz) {
        pair<uint64_t, uint64_t> *pair = _vector[lastVectorIndex];
        int range = pair->second - pair->first;
        if (lastVectorOffset <= range) {
            fillVectorAndRandomize(_vector, lastVectorIndex, lastVectorOffset);
            return true;
        } else return false;
    }

    return false;
}

const uint64_t IpListParser::getNext() {
    return ipList[atomicIndex++];
}

void IpListParser::saveRestToFile(const vector<pair<uint64_t, uint64_t> *> _vector, int vectorIndex, uint64_t ipFirst,
                                  uint64_t ipSecond, int initOffset) {
    std::ofstream output(TEMP_FILENAME);

    lastVectorIndex = vectorIndex;
    lastVectorOffset = initOffset;

    if (!output.fail()) {
        output << boost::asio::ip::address_v4(ipFirst).to_string() << "-"
               << boost::asio::ip::address_v4(ipSecond).to_string() << endl;
    }

    int sz = _vector.size();
    for (int i = vectorIndex + 1; i < sz; ++i) {
        const pair<uint64_t, uint64_t> *pair = _vector[i];
        if (!output.fail()) {
            output << boost::asio::ip::address_v4(pair->first).to_string() << "-"
                   << boost::asio::ip::address_v4(pair->second).to_string() << endl;
        }
    }
}




