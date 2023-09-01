#include "converter.h"
#include "lookup/stateToColor.h"

using namespace std;

void Converter::RegionConverter::ColorConverter::getImage(Converter::RegionConverter::ColorConverter::Image &image) {
    for (int tileX = 0; tileX < 8; tileX++) {
        for (int tileZ = 0; tileZ < 8; tileZ++) {
            for (int chunkX = 0; chunkX < 4; chunkX++) {
                for (int chunkZ = 0; chunkZ < 4; chunkZ++) {
                    RegionParser::ChunkParameters *workingChunk = &region[tileX][tileZ]->chunks[chunkX][chunkZ];
                    if (workingChunk->isVoid) {
                        setChunkVoid(image,
                                     (tileX * 4) + chunkX,
                                     (tileZ * 4) + chunkZ);
                        continue; // chunk set to void... can continue to NEXT chunk
                    }
                    // else read chunk's data and compile overlays and data from adjacent chunks
                    for (int x = 0; x < 16; x++) {
                        for (int z = 0; z < 16; z++) {
                            const array<unsigned char*, 4> pixel = image[(tileX * 64) + (chunkX * 16) + x, (tileZ * 64) + (chunkZ * 16) + z];
                            getPixelColor(pixel, (tileX * 64) + (chunkX + 16) + x, (tileZ * 64) + (chunkZ * 16) + z);
                        }
                    }
                }
            }
        }
    }
}

Converter::RegionConverter::ColorConverter::ColorConverter(Converter::RegionConverter::Region &region) {
    this->region = &region;
}

void Converter::RegionConverter::ColorConverter::setChunkVoid(Converter::RegionConverter::ColorConverter::Image &image,
                                                              int chunkX, int chunkZ) {
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            *image[(chunkX * 16) + x, (chunkZ * 16) + z][ALPHA] = (unsigned char) 0;
        }
    }
}

void Converter::RegionConverter::ColorConverter::getPixelColor(const array<unsigned char *, 4> &pixel, int x, int z) {
    RegionParser::Parameters* blockParameters = &parametersFromPixel(x, z);
    if (blockParameters->stateId != AIR_ID) {
        
    }
}

Converter::RegionConverter::RegionParser::Parameters &
Converter::RegionConverter::ColorConverter::parametersFromPixel(int x, int z) {
    return (*region[x % 64][z % 64]->chunks
    [(x - (x % 64) * 64) % 16][(z - (z % 64) * 64) % 16].chunk)
    [x - (x - (x % 64) * 64) % 16, z - (z - (z % 64) * 64) % 16]; // this is disgusting
}

