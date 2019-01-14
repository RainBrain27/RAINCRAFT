#ifndef CHUNK_OBJ_H
#define CHUNK_OBJ_H

#include "chunk.h"
#include "baseobject.h"

class chunk_obj : public baseobject
{
public:
    chunk_obj(size_t texID, size_t program,int ObjID,glm::vec3 pos,
              std::vector<unsigned short> cube_indices,
              std::vector<glm::vec3> cube_indexed_vertices,
              std::vector<glm::vec2> cube_indexed_uvs,
              std::vector<glm::vec3> cube_indexed_normals);
    chunk* get_chunk_shape(){return &chunk_shape;}
    void change_block(int x,int y,int z, short ID);

    void refresh_chunk();

    void set_neighbor(chunk_obj* Nneighbor, int i);
    void del_neighbor(int i);

    void replace_to(glm::vec3 pos);

private:
    chunk_obj* neighbour[6]={0,0,0,0,0,0};
    chunk* shape_neighbour[6]={0,0,0,0,0,0};
    //z- x- z+ x+ y+ y- Richtung

    short Block_list[16][16][16];

    chunk chunk_shape;

    void generate_chunk();

};

#endif // CHUNK_OBJ_H
