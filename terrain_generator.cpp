//
// Created by THE BERND on 24.10.2018.
//

#include <memory.h>
#include <cmath>
#include <c++/iostream>
#include "terrain_generator.h"
#include <Simplex.h>
#include <limits>

#define CHUNK_SIZE 16
#define SPX_FREQUENCY 0.2f
#define SPX_AMPLITUDE 0.5f
#define SPX_LACUNARITY 1.6f
#define SPX_PERSISTENCE 0.1f
#define SPX_OCTAVES 32
#define SPX_GAIN 0.2f
#define STRECH_CONSTANT 64.0f

float terrain_generator::sinusNoise(long long x, long long z){
    return (float)((std::sin(((double)x/CHUNK_SIZE)*360*(M_PI/180))+1)/2) * (float)((std::sin(((double)z/CHUNK_SIZE)*360*(M_PI/180))+1)/2);
}

float terrain_generator::simplexNoise(long long x, long long z) {
    float y = 0.0f;
    auto fX = x/(STRECH_CONSTANT);
    auto fZ = z/(STRECH_CONSTANT);
    glm::vec2 coords = glm::vec2(fX,fZ);
    //y=Simplex::noise(coords);
    y = Simplex::fBm(coords,SPX_OCTAVES,SPX_LACUNARITY,SPX_GAIN);
    //y = Simplex::ridgedNoise(coords);
    //y = Simplex::flowNoise(coords,1);
    return y;
}

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
    }else if(chunkY > 2){
        bl.resize(CHUNK_SIZE);
        for(std::vector<std::vector<int>> &v:bl) {
            v.resize(CHUNK_SIZE);
            for(std::vector<int> &vi:v) {
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
    long long bx,by,bz;
    bx = chunkX*CHUNK_SIZE;
    by = chunkY*CHUNK_SIZE;
    bz = chunkZ*CHUNK_SIZE;
    for(int x = 0; x < CHUNK_SIZE; x++, bx++){
        bz = chunkZ*CHUNK_SIZE;
        for(int z = 0; z < CHUNK_SIZE; z++, bz++)
        {
            by = chunkY*CHUNK_SIZE;
            short p = (short)(CHUNK_SIZE*noise(bx,bz));
            for(int y = 0; y < CHUNK_SIZE; y++, by++){
                bl[x][y][z] = p >= y?1:-1;
            }
        }
    }
    return &bl;
}

ChunkVector *terrain_generator::generateChunks(glm::ivec3 start, glm::ivec3 end, int totalCount) {
    if(totalCount == -1){
        ChunkVector *vec = new ChunkVector((unsigned)(std::abs(start.x-end.x)*std::abs(start.y-end.y)*std::abs(start.z-end.z)));
        int i = 0;
        for(int x = start.x; x < end.x; x++){
            for(int y = start.y; y < end.y;y++){
                for(int z = start.z; z<end.z;z++){

                }
            }
        }
    }
}
#undef CHUNK_SIZE
#undef SPX_FREQUENCY
#undef SPX_AMPLITUDE
#undef SPX_LACUNARITY
#undef SPX_PERSISTENCE
#undef SPX_OCTAVES
#undef SPX_GAIN
#undef STRECH_CONSTANT