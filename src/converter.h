#ifndef XAEROTOJOURNEYMAP_CONVERTER_H
#define XAEROTOJOURNEYMAP_CONVERTER_H

#define REGION_SIZE 512

#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

#include <string>
#include <filesystem>
#include <utility>
#include <vector>
#include <queue>
#include <utility>
#include <thread>
#include <iostream>
#include <array>

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

    struct Image {
        unsigned char image[REGION_SIZE * REGION_SIZE * 4];

        std::array<unsigned char*, 4> operator[] (int x, int z) { // returns array of size 4 representing 1 pixel in the final image
            std::array<unsigned char*, 4> pixel{};
            for (int i = 0; i < 4; i++) {
                pixel[i] = &(image[(x + z * REGION_SIZE) * 4 + i]);
            }
            return pixel;
        }
    };

    class RegionConverter {
    public:
        struct Region;

    private:

        void writeImage();

        std::string regionFile;

        class ColorConverter {

        };
        class RegionParser {
        public:
            struct OverlayParameters {
                bool isNotWater;
                bool hasOpacity;
                bool legacyHasOpacity;
                bool hasCustomColor;
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

            RegionParser(std::string * file);
            void getRegion(Region & region);
        private:
            class ByteParser {
            public:
                ByteParser(std::string * file) {
                    this->file = file;
                }

                void loadInt();
                void loadByte();
                void loadSize(int bytes);

                int peekNextInt();

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

            std::string * file;

            int saveVersion = 0;

            ByteParser bitParser;

            void getNextBlockParameters (Parameters & parameters);

            Coordinate getNextTileCoordinate();
        };
        static void freePixelData(Region & region);
    public:
        struct Region {
            RegionParser::TileChunk region[8][8];

            RegionParser::TileChunk* operator[] (int xIndex) {
                return region[xIndex];
            }
        };

        void loadRegion(std::filesystem::path region);
        void convert(Region & region);
    };
};

#endif //XAEROTOJOURNEYMAP_CONVERTER_H
