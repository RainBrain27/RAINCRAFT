#include "chunk_obj.h"
#include "terrain_generator.h"

chunk_obj::chunk_obj(size_t texture, size_t program, int NewObjID, glm::vec3 pos,
                     std::vector<unsigned short> cube_indices,
                     std::vector<glm::vec3> cube_indexed_vertices,
                     std::vector<glm::vec2> cube_indexed_uvs,
                     std::vector<glm::vec3> cube_indexed_normals,
                     glm::ivec3 coordinates
                     )

    :baseobject(0, texture,program,NewObjID, pos)
{
    chunk_shape.clone_alpha_cube(cube_indices,
                                 cube_indexed_vertices,
                                 cube_indexed_uvs,
                                 cube_indexed_normals);

    chunk_shape.set_neighbours(shape_neighbour);
/*
    int b=16;
    for(int x=0;x<b;x++){
        for(int y=0;y<b;y++){
            for(int z=0;z<b;z++){
                Block_list[x][y][z]=-1;
            }
        }
    }
    //chunk_shape.set_Block_list(Block_list);
*/
    generate_chunk(coordinates);

    //refresh_chunk();
}

chunk_obj::~chunk_obj() {
}

#define CHUNK_SIZE 16
void chunk_obj::generate_chunk(glm::ivec3 coord)
{
    chunk_shape.set_Block_list(terrain_generator::generateChunk(coord.x,coord.y,coord.z, Block_list,terrain_generator::simplexNoise));
    /*
    int ID=1;
    int b=16;
    for(int x=0;x<b;x++){
        for(int y=0;y<b;y++){
            for(int z=0;z<b;z++){
                chunk_shape.set_Block_list(x,y,z,ID);
            }
        }
    }*/
    //gegebenenfalls -1sen hineinschreiben
}

void chunk_obj::refresh_chunk()
{
    chunk_shape.check_all_Blocks();
}

void chunk_obj::change_block(int x, int y, int z, int ID)
{
    chunk_shape.change_block(x,y,z,ID);
}

void chunk_obj::set_neighbor(chunk_obj *Nneighbor, int i)
{
    neighbour[i]=Nneighbor;
    shape_neighbour[i]=Nneighbor->get_chunk_shape();
}

#undef CHUNK_SIZE