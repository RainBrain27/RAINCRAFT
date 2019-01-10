#include "chunk_obj.h"

chunk_obj::chunk_obj(size_t texture, size_t program, int NewObjID, glm::vec3 pos,
                     std::vector<unsigned short> cube_indices,
                     std::vector<glm::vec3> cube_indexed_vertices,
                     std::vector<glm::vec2> cube_indexed_uvs,
                     std::vector<glm::vec3> cube_indexed_normals
                     )

    :baseobject(0, texture,program,NewObjID, pos)
{
    chunk_shape.clone_alpha_cube(cube_indices,
                                 cube_indexed_vertices,
                                 cube_indexed_uvs,
                                 cube_indexed_normals);

    chunk_shape.set_neighbours(shape_neighbour);

    int b=16;
    for(int x=0;x<b;x++){
        for(int y=0;y<b;y++){
            for(int z=0;z<b;z++){
                Block_list[x][y][z]=-1;
            }
        }
    }

    chunk_shape.set_Block_list(Block_list);

    generate_chunk();

    //refresh_chunk(); //->fur alle zusammen aufgerufen
}

void chunk_obj::generate_chunk()
{
    // gibt dem chunk den Inhalt
    short ID=1;
    int b=16;
    for(int x=0;x<b;x++){
        for(int y=0;y<b;y++){
            for(int z=0;z<b;z++){
                if(y<4){
                    //if( (( (x-8)*(x-8)+(z-8)*(z-8) )*0.03f+(y))<4 ){
                    if((x-8)*(x-8)*(z-8)<20){
                    //if(x+y+z==8){
                        chunk_shape.set_Block_list(x,y,z,ID);
                    }
                }
            }
        }
    }

}

void chunk_obj::refresh_chunk()
{
    chunk_shape.refresh_chunk();
}

void chunk_obj::change_block(int x, int y, int z, short ID)
{
    chunk_shape.change_block(x,y,z,ID);
}

void chunk_obj::set_neighbor(chunk_obj *Nneighbor, int i)
{
    neighbour[i]=Nneighbor;
    shape_neighbour[i]=Nneighbor->get_chunk_shape();
}
