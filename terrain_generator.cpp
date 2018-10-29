//
// Created by THE BERND on 24.10.2018.
//

#include <memory.h>
#include <cmath>
#include "terrain_generator.h"

#define CHUNK_SIZE 16

float sinusNoise(int x, int z){
    return (float)((std::sin(((double)x/CHUNK_SIZE)*360*(M_PI/180))+1)/2);
}

terrain_generator::terrain_generator(unsigned long long seed) {srand(seed);}

BlockVector * terrain_generator::generateChunk(int chunkX, int chunkY, int chunkZ, BlockVector &bl,noisefunc noise) {
    if(chunkY < 1){
        bl.resize(CHUNK_SIZE);
        for(std::vector<std::vector<int>> &v:bl) {
            v.resize(CHUNK_SIZE);
            for(std::vector<int> &vi:v){
                vi.resize(CHUNK_SIZE,1);
            }
        }
        return &bl;
    }else if(chunkY > 1){
        bl.resize(CHUNK_SIZE);
        for(std::vector<std::vector<int>> &v:bl) {
            v.resize(CHUNK_SIZE);
            for(std::vector<int> &vi:v){
                vi.resize(CHUNK_SIZE,-1);
            }
        }
        return &bl;
    }

    bl.resize(CHUNK_SIZE);
    for(std::vector< std::vector<int >> &v:bl){
        v.resize(CHUNK_SIZE);
        for(std::vector<int> &vi:v){
            vi.resize(CHUNK_SIZE);
        }
    }
    for(int x = 0; x < CHUNK_SIZE; x++){
        for(int y = 0; y < CHUNK_SIZE; y++){
            for(int z = 0; z < CHUNK_SIZE; z++) {
                int p = CHUNK_SIZE*noise(x,z);
                bl[x][y][z] = p>=y?1:-1;
            }
        }
    }
    return &bl;
}

#undef CHUNK_SIZE