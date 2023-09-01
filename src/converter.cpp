#include "converter.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

//#include <ctpl_stl.h> // for future thread stuff

#include <Zip/ZipStream.h>
#include <Zip/ZipArchive.h>
#include <StreamCopier.h>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <bitset>
#include <stack>

using namespace std;
using namespace Poco::Zip;
using namespace filesystem;

Converter::Converter(string inputFolder, string outputFolder) {
    this->inputFolder = filesystem::path(inputFolder);
    this->outputFolder = filesystem::path(outputFolder);
}

void Converter::runConversion() {
    if (!filesystem::exists(outputFolder)) {
        cout << "creating output folders" << endl;
        createSubfolders();
    }

    cout << "converting overworld" << endl;
    auto startTime = chrono::high_resolution_clock::now();
    convertDimension(0);
    cout << "finished in: " << chrono::duration<float, chrono::milliseconds::period>(chrono::high_resolution_clock::now() - startTime) << endl;

//    convertDimension(1);
//    convertDimension(-1);
}

void Converter::createSubfolders() {
    path overworld = outputFolder / "DIM0/day";
    path nether = outputFolder / "DIM-1/7";
    path end = outputFolder / "DIM1/7";
    create_directories(overworld);
    create_directories(nether);
    create_directories(end);
}

void Converter::convertDimension(int dimension) {
    path dimensionFolder = inputFolder;
    if (dimension == 0) { // overworld so need to check whether null or dim0
        if (exists(inputFolder / "DIM0")) {
            dimensionFolder /= "DIM0";
        } else {
            dimensionFolder /= "null";
        }
    } else {
        dimensionFolder /= "DIM" + to_string(dimension);
    }
    dimensionFolder /= "mw$default";

//    Image image{};
//    for (int x = 0; x < 512; x++) {
//        for (int z = 0; z < 512; z++) {
//            *image[x,z][RED] = (unsigned char) 255;
//            *image[x,z][ALPHA] = (x % 2 == 0) && (z % 2 == 0) ? (unsigned char) 255 : (unsigned char) 0;
//        }
//    }
//    stbi_write_png("/home/danducky/Programming/C++/XaerotoJourneyMap/Test/keb.png", REGION_SIZE, REGION_SIZE, 4, &image, REGION_SIZE*4);

    vector<path> xaeroFiles;
    for (const auto& file : directory_iterator(dimensionFolder)) {
        xaeroFiles.push_back(file.path());
        cout << file.path().string() << "\n";
        RegionConverter test{};
        RegionConverter::Region region;
        test.loadRegion(file.path());
        test.convert(region);
    }
}

//constexpr std::map<int, int> Converter::getMap(std::filesystem::path pathToData) {
//}

void Converter::RegionConverter::loadRegion(std::filesystem::path region) { // read from zippington
    ifstream zipFile(region, ios::binary);
    poco_assert(zipFile);
    ZipArchive archive(zipFile);
    auto iterator = archive.findHeader("region.xaero");
    poco_assert(iterator != archive.headerEnd());
    zipFile.clear();
    ZipInputStream zipIn(zipFile, iterator->second);
    ostringstream out(ios::binary);
    Poco::StreamCopier::copyStream(zipIn, out);
    regionFile = out.str();
}

void Converter::RegionConverter::convert(Region& region) {
    RegionParser parser(&regionFile);

    parser.getRegion(region);

    freePixelData(region);

}

void Converter::RegionConverter::freePixelData(Region & region) {
    /*
     * frees all of the optionally allocated heap memory from the in-memory region
     * this is optionally allocated as some chunks are void, which means that they don't need any of the data from Parameters{}
     */
    for (int tileX = 0; tileX < 8; tileX++) {
        for (int tileZ = 0; tileZ < 8; tileZ++) {
            for (int chunkX = 0; chunkX < 4; chunkX++) {
                for (int chunkZ = 0; chunkZ < 4; chunkZ++) {
                    if (!region[tileX][tileZ].chunks[chunkX][chunkZ].isVoid) {
                        for (int blockX = 0; blockX < 16; blockX++) {
                            for (int blockZ = 0; blockZ < 16; blockZ++) {
                                if ((*region[tileX][tileZ].chunks[chunkX][chunkZ].chunk)[blockX, blockZ].numberOfOverlays > 0) {
                                    delete[] (*region[tileX][tileZ].chunks[chunkX][chunkZ].chunk)[blockX, blockZ].overlays;
                                }
                            }
                        }
                        delete[] region[tileX][tileZ].chunks[chunkX][chunkZ].chunk;
                    }
                }
            }
        }
    }

}

Converter::RegionConverter::RegionParser::RegionParser(std::string * file)
        : bitParser(bitParser) {
    this->file = file;

    bitParser = ByteParser(this->file);

    // move over initial file buffer thing
    bitParser.loadByte();
    if (bitParser.getValue() == 255) {
        bitParser.loadInt();
        this->saveVersion = bitParser.getValue();
    } else {
        bitParser.incrementPosition(-1);
    }
}

void Converter::RegionConverter::RegionParser::getNextBlockParameters(
        Converter::RegionConverter::RegionParser::Parameters &parameters) {
    bitParser.loadInt();
    parameters.isNotGrass = bitParser.getNextBool(true);
    parameters.hasOverlays = bitParser.getNextBool(true);
    parameters.colorTypeToWrite = bitParser.getNextBits(6, true);
    parameters.light = bitParser.getNextBits(4, true);
    parameters.height = bitParser.getNextBits(8, true);
    parameters.hasBiome = bitParser.getNextBool(true);
    // ah yes... ignoring some properties here B)

    bitParser.skipBits(3);
    parameters.topHeightAndHeightDontMatch = bitParser.getNextBool();

    if (parameters.isNotGrass) {
        bitParser.loadInt();
        parameters.stateId = bitParser.getValue();
    }

    if (parameters.topHeightAndHeightDontMatch && saveVersion >= 4) {
        bitParser.loadByte();
        parameters.topHeight = bitParser.getValue();
    }

    if (parameters.hasOverlays) {
        bitParser.loadByte();
        parameters.numberOfOverlays = bitParser.getValue();
        parameters.generateOverlays();
        for (int i = 0; i < parameters.numberOfOverlays; i++) {
            bitParser.loadInt();
            OverlayParameters* workingOverlay = &parameters.overlays[i];
            workingOverlay->isNotWater = bitParser.getNextBool(true);
            workingOverlay->legacyHasOpacity = bitParser.getNextBool(true);
            workingOverlay->hasCustomColor = bitParser.getNextBool(true);
            workingOverlay->hasOpacity = bitParser.getNextBool(true);
            workingOverlay->light = bitParser.getNextBits(4, true);
            workingOverlay->savedColorType = bitParser.getNextBits(2, true);

            if (workingOverlay->isNotWater) {
                bitParser.loadInt();
                workingOverlay->state = bitParser.getValue();
            } // else stateId = water

            // old opacity check [SKIP]
            if (saveVersion < 1 && workingOverlay->legacyHasOpacity) {
                bitParser.loadInt();
                workingOverlay->opacity = bitParser.getValue();
            }

            if (workingOverlay->savedColorType == 2 || workingOverlay->hasCustomColor) {
                bitParser.loadInt();
                if (bitParser.getValue() == -1) {
                    workingOverlay->savedColorType = 0;
                } else {
                    workingOverlay->savedColorType = 3;
                }

                workingOverlay->customColor = bitParser.getValue();
            }
            if (workingOverlay->hasOpacity) {
                bitParser.loadInt();
                workingOverlay->opacity = bitParser.getValue();
            }
        }
    }

    if (parameters.colorTypeToWrite == 3) {
        bitParser.loadInt();
        parameters.customColor = bitParser.getValue();
    }

    if (parameters.hasBiome) {
        bitParser.loadByte(); // load area that includes biome info
        if (bitParser.getValue() == 255) { // if that byte is 255 it means that the biome id is > 255
            bitParser.loadInt();
            parameters.biome = bitParser.getValue(); // read biome id
        } else { // biome id is < 255
            parameters.biome = bitParser.getValue(); // read biome id
        }
    }
}

Converter::RegionConverter::RegionParser::Coordinate Converter::RegionConverter::RegionParser::getNextTileCoordinate() {
    Coordinate tileCoordinates {};
    if (bitParser.getFilePosition() == file->size()) { // file over YAYYYYY
        tileCoordinates.x = -1;
        tileCoordinates.z = -1;
        return tileCoordinates;
    }
    bitParser.loadByte();
    tileCoordinates.z = bitParser.getNextBits(4, true);
    tileCoordinates.x = bitParser.getNextBits(4);
    return tileCoordinates;
}

void
Converter::RegionConverter::RegionParser::getRegion(Region & region) {
    bool eof = false;
    for (int iterations = 0; iterations < (8*8) && !eof; iterations++) {
        Coordinate currentTile = getNextTileCoordinate();
        if (currentTile.x == -1 || currentTile.z == -1) {
            eof = true;
            continue;
        }
        for (int chunkX = 0; chunkX < 4; chunkX++) { // todo: replace with range based loops :3
            for (int chunkZ = 0; chunkZ < 4; chunkZ++) {
                if (bitParser.peekNextInt() == -1) { // this means ChunkParameters is void
                    // VOID CHUNK
                    bitParser.incrementPosition(4); // skip over void chunk
                    region[currentTile.x][currentTile.z].chunks[chunkX][chunkZ].isVoid = true;
                } else { // actual ChunkParameters :o
                    // LOADED CHUNK
                    ChunkParameters *workingChunk = &region[currentTile.x][currentTile.z].chunks[chunkX][chunkZ];
                    workingChunk->isVoid = false;
                    workingChunk->chunk = new Chunk{};
                    for (int blockX = 0; blockX < 16; blockX++) {
                        for (int blockZ = 0; blockZ < 16; blockZ++) {
                            getNextBlockParameters((*workingChunk->chunk)[blockX, blockZ]);
                        }
                    }
                    bitParser.incrementPosition(1); // this skips chunk version byte
                    if (saveVersion > 4) {
                        bitParser.incrementPosition(5); // skip past weird cave info
                    }
                }
            }
        }
    }
}
