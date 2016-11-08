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
#include <c++/ctime>
#include <algorithm>
#include <random>
#include <c++/chrono>

using namespace std;

class IpListParser {
private:
    static constexpr const char *TEMP_FILENAME = ".iptemp";
    static constexpr const unsigned int INITIAL_CAPACITY = 50000;
    default_random_engine engine;
    int lastVectorIndex;
    int lastVectorOffset;
    atomic_int atomicIndex;
    vector<uint64_t> ipList;
    vector<pair<uint64_t, uint64_t> *> _vector;
public:
    IpListParser(char *path) : atomicIndex(0), lastVectorIndex(0), lastVectorOffset(0),
                               engine(std::default_random_engine{
                                       std::chrono::system_clock::now().time_since_epoch().count()}) {
        ipList.reserve(INITIAL_CAPACITY);

        const vector<pair<uint64_t, uint64_t> > &ipPairVector = loadVector(path);
        _vector = trimVector((vector<pair<uint64_t, uint64_t> > &) ipPairVector);
        fillVectorAndRandomize(_vector);
    };

    const vector<pair<uint64_t, uint64_t> > loadVector(const char *filename);

    vector<pair<uint64_t, uint64_t> *> trimVector(vector<pair<uint64_t, uint64_t> > &ipVector);

    const vector<pair<uint64_t, uint64_t> > parseIPVector(vector<string> _vector);

    const uint64_t getNext();

    void fillInnerVector(vector<pair<uint64_t, uint64_t> *> _vector, int startIndex = 0, int initialOffset = 0);

    void fillVectorAndRandomize(vector<pair<uint64_t, uint64_t> *> _vector, int startIndex = 0, int initialOffset = 0);

    bool hasNext();

    void saveRestToFile(const vector<pair<uint64_t, uint64_t> *> _vector, int vectorIndex, uint64_t ipFirst,
                        uint64_t ipSecond, int initOffset);
};

#endif //RANDOMIZER_IPLISTPARSER_H
