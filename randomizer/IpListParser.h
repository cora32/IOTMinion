//
// Created by cora32 on 04.09.2016.
//

#ifndef RANDOMIZER_IPLISTPARSER_H
#define RANDOMIZER_IPLISTPARSER_H

#include <iostream>
#include <list>
#include <c++/vector>
#include <stdint-gcc.h>
#include <c++/queue>
#include <c++/atomic>

using namespace std;

class IpListParser {
private:
    atomic_int index;
    vector<uint64_t> ipList;
public:
    const vector<pair<uint64_t, uint64_t> > loadVector(const char *filename);

    vector<pair<uint64_t, uint64_t> *> &trimVector(vector<pair<uint64_t, uint64_t> > &ipVector);

    const vector<pair<uint64_t, uint64_t> > parseIPVector(vector<string> vector);

    const uint64_t getNext();

    void init(char *path);

    void fillInnerVector(const vector<pair<uint64_t, uint64_t> *> &vector);
};

#endif //RANDOMIZER_IPLISTPARSER_H
