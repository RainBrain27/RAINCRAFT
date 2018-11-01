//
// Created by THE BERND on 24.10.2018.
//

#ifndef RAINCRAFT_TERRAIN_GENERATOR_H
#define RAINCRAFT_TERRAIN_GENERATOR_H

#include "chunk_obj.h"

typedef float (*noisefunc)(long long x, long long z);
typedef std::vector< std::vector< std::vector<int> > > BlockVector;
typedef std::vector< chunk_obj* > ChunkVector;

class terrain_generator {
public:
    static ChunkVector *generateChunks(glm::ivec3 start, glm::ivec3 end,int totalCount = -1);
    static BlockVector *generateChunk(int chunkX, int chunkY, int chunkZ, BlockVector &bl, noisefunc noise = sinusNoise);
    static float simplexNoise(long long x, long long z);
    static float sinusNoise(long long x, long long z);
};

#endif //RAINCRAFT_TERRAIN_GENERATOR_H
