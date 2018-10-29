//
// Created by THE BERND on 24.10.2018.
//

#ifndef RAINCRAFT_TERRAIN_GENERATOR_H
#define RAINCRAFT_TERRAIN_GENERATOR_H

#include "chunk_obj.h"

typedef float (*noisefunc)(int x, int z);
typedef std::vector< std::vector< std::vector<int> > > BlockVector;

float sinusNoise(int x, int z);

class terrain_generator {
public:
    terrain_generator(unsigned long long seed);
    static std::vector< std::vector< std::vector<int> > > *generateChunk(int chunkX, int chunkY, int chunkZ, BlockVector &bl, noisefunc noise = sinusNoise);
};


#endif //RAINCRAFT_TERRAIN_GENERATOR_H
