#ifndef XAEROTOJOURNEYMAP_CONVERTER_H
#define XAEROTOJOURNEYMAP_CONVERTER_H

#include <string>
#include <filesystem>
#include <utility>
#include <vector>
#include <queue>
#include <utility>
#include <thread>
#include <iostream>

class Converter {
public:
    Converter (std::string inputFolder, std::string outputFolder);

    void runConversion();

    ~Converter() {
    }
private:
    void createSubfolders();
    void convertDimension(int dimension);

    std::filesystem::path inputFolder;
    std::filesystem::path outputFolder;

    class RegionConverter;

    class RegionConverter {
    public:
        void loadRegion(std::filesystem::path region);
        void convert();
    private:
        std::filesystem::path regionFile;
        class RegionParser {
        public:
            struct OverlayParameters {
                bool isWater;
                bool hasOpacity;
                bool legacyHasOpacity;
                int light;
                int savedColorType;

                //not a "param" but should be fetched in same context

                int state;
                int customColor;
                int opacity;
            };
            struct Parameters {
                bool isNotGrass;
                bool hasOverlays;
                int colorTypeToWrite;
                int light;
                int height;
                bool hasBiome;
                bool topHeightAndHeightDontMatch;

                //not a "param" but should be fetched in same context
                int customColor;
                int topHeight;
                int biome;
                int state;
                int numberOfOverlays;

                OverlayParameters* overlays;
                void generateOverlays() {
                    overlays = new OverlayParameters[numberOfOverlays];
                }
            };
            struct Chunk {
                Parameters parameters[16 * 16];

                Parameters& operator[] (int x, int z) {
                    return parameters[x + 16 * z];
                }
            };
            struct ChunkParameters {
                bool isVoid = true;
                Chunk* chunk;
                //16 chunks with 16*16 blocks inside each
            };
            struct TileChunk {
                ChunkParameters chunks[4][4];
            };
            struct Coordinate {
                int x;
                int z;
            };

            RegionParser(std::string file);
            void getRegion(TileChunk (&region)[][8]);
        private:
            class ByteParser {
            public:
                ByteParser(std::string& file) {
                    this->file = &file;
                }

                void loadInt();
                void loadByte();
                void loadSize(int bytes);

                int getValue() const;
                void incrementPosition(int bytes);
                void skipBits(int bits);

                bool getNextBool();
                int getNextBits(int numberOfBits);
                bool getNextBool(bool advancePosition);
                int getNextBits(int numberOfBits, bool advancePosition);

                // DEBUG
                int getPosition() const;
                int getFilePosition() const;
            private:
                int filePosition = 0;

                int position;
                int value;

                std::string* file;

                void loadNextByte();
                void loadNextOfSize(int size);
            };

            std::string file;

            int saveVersion = 0;

            ByteParser bitParser;

            void getNextBlockParameters (Parameters & parameters);

            Coordinate getNextTileCoordinate();
        };
        static void freePixelData(RegionParser::TileChunk (&region)[][8]);
    };
};

#endif //XAEROTOJOURNEYMAP_CONVERTER_H
