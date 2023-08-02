#include "converter.h"
#include <string>
#include <cmath>

using namespace std;

void Converter::RegionConverter::RegionParser::ByteParser::loadNextByte() {
    value = (unsigned char) (*file)[filePosition] & 255;
//    value = (unsigned char) file->substr(filePosition, 1)[0] & 255;
    filePosition++;
    position = 0;
}

void Converter::RegionConverter::RegionParser::ByteParser::loadNextOfSize(int size) {
    const string substring = file->substr(filePosition, size);
    int constructedInt = 0;
    for (int i = 0; i < size; i++) {
        constructedInt = constructedInt << 8 | static_cast<unsigned char>(substring[i]);
    }
    value = constructedInt;
    filePosition += size;
    position = 0;
}

bool Converter::RegionConverter::RegionParser::ByteParser::getNextBool() {
    return ((value >> position) & 1) == 1;
}

int Converter::RegionConverter::RegionParser::ByteParser::getNextBits(int numberOfBits) {
    return ((value >> position) & ((int) pow(2, numberOfBits) - 1));
}

bool Converter::RegionConverter::RegionParser::ByteParser::getNextBool(bool advancePosition) {
    if (advancePosition) {
        bool nextBool = getNextBool();
        position += 1;
        return nextBool;
    } else {
        return getNextBool();
    }
}

int Converter::RegionConverter::RegionParser::ByteParser::getNextBits(int numberOfBits, bool advancePosition) {
    if (advancePosition) {
        int nextBits = getNextBits(numberOfBits);
        position += numberOfBits;
        return nextBits;
    } else {
        return getNextBits(numberOfBits);
    }
}

void Converter::RegionConverter::RegionParser::ByteParser::incrementPosition(int bytes) {
    filePosition += bytes;
}

void Converter::RegionConverter::RegionParser::ByteParser::loadSize(int bytes) {
    loadNextOfSize(bytes);
}

void Converter::RegionConverter::RegionParser::ByteParser::loadByte() {
    loadNextByte();
}

void Converter::RegionConverter::RegionParser::ByteParser::loadInt() {
    loadNextOfSize(4);
}

int Converter::RegionConverter::RegionParser::ByteParser::getPosition() const {
    return position;
}

int Converter::RegionConverter::RegionParser::ByteParser::getValue() const {
    return value;
}

void Converter::RegionConverter::RegionParser::ByteParser::skipBits(int bits) {
    position += bits;
}

int Converter::RegionConverter::RegionParser::ByteParser::getFilePosition() const {
    return filePosition;
}

int Converter::RegionConverter::RegionParser::ByteParser::peekNextInt() {
    const string substring = file->substr(filePosition, 4);
    int constructedInt = 0;
    for (int i = 0; i < 4; i++) {
        constructedInt = constructedInt << 8 | static_cast<unsigned char>(substring[i]);
    }
    return constructedInt;
}
