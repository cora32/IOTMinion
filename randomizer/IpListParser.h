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
    static constexpr const char *TEMP_FILENAME = ".iptemp";
    static constexpr const unsigned int INITIAL_CAPACITY = 5;
    int lastVectorIndex;
    int lastVectorOffset;
    atomic_int atomicIndex;
    vector<uint64_t> ipList;
    vector<pair<uint64_t, uint64_t> *> _vector;
public:
    IpListParser(char *path) : atomicIndex(0), lastVectorIndex(0), lastVectorOffset(0) {
        ipList.reserve(INITIAL_CAPACITY);

        const vector<pair<uint64_t, uint64_t> > &ipPairVector = loadVector(path);
        _vector = trimVector((vector<pair<uint64_t, uint64_t> > &) ipPairVector);
        fillInnerVector(_vector);
    };

    const vector<pair<uint64_t, uint64_t> > loadVector(const char *filename);

    vector<pair<uint64_t, uint64_t> *> trimVector(vector<pair<uint64_t, uint64_t> > &ipVector);

    const vector<pair<uint64_t, uint64_t> > parseIPVector(vector<string> vector);

    const uint64_t getNext();

    void fillInnerVector(vector<pair<uint64_t, uint64_t> *> vector, int startIndex = 0, int initialOffset = 0);

    bool hasNext();

    void saveRestToFile(const vector<pair<uint64_t, uint64_t> *> vector, int vectorIndex, uint64_t ipFirst,
                        uint64_t ipSecond, int initOffset);
};

#endif //RANDOMIZER_IPLISTPARSER_H
