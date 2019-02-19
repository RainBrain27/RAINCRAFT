#include "chunk_obj.h"

chunk_obj::chunk_obj(size_t texture, size_t program, int NewObjID, glm::vec3 pos,
                     std::vector<unsigned short> cube_indices,
                     std::vector<glm::vec3> cube_indexed_vertices,
                     std::vector<glm::vec2> cube_indexed_uvs,
                     std::vector<glm::vec3> cube_indexed_normals,
                     gl_main* LINUX
                     )

    :baseobject(0, texture,program,NewObjID, pos)
{
    chunk_shape.set_gl_main(LINUX);

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
                //if(y<4){
                    if( (( (x-8)*(x-8)+(z-8)*(z-8) )*0.03f+(y))<4 ){
                    //if((x-8)*(x-8)*(z-8)<20){
                    //if(x+y+z==8){
                    //if((x+y+z)%2==0){
                    //if(((x-7.5)*(x-7.5)*(z-7.5)*(z-7.5)*(y-7.5)*(y-7.5)<20)){
<<<<<<< HEAD
                    if(y>=10){
                    //if(false){

                        //chunk_shape.set_Block_list(x,y,z,  1  ); //PARTY HARD in 30s //DONE
                        if(x==0 or x==15 or z==0 or z==15){
                            if(10-std::abs(z-7.5)>y-9 and 10-std::abs(x-7.5)>y-9){
                                chunk_shape.set_Block_list(x,y,z,  0  );
                            }
                        }
                        else if((5<x and x<10) and (5<z and z<10)){
                            if(10-std::abs(z-7.5)>y-8 and 10-std::abs(x-7.5)>y-8){
                                chunk_shape.set_Block_list(x,y,z,  -1  );
                            }
                        }
                        else{
                            if(10-std::abs(z-7.5)>y-8 and 10-std::abs(x-7.5)>y-8){
                                chunk_shape.set_Block_list(x,y,z,  1  );
                            }

                        }
=======
                        chunk_shape.set_Block_list(x,y,z,ID);
>>>>>>> parent of 9cc1eb9... mehrere texturen in chunks
                    }
                //}
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

void chunk_obj::del_neighbor(int i)
{
    neighbour[i]=0;
    shape_neighbour[i]=0;
}

void chunk_obj::replace_to(glm::vec3 pos)
{
    //printf("FIX r1\n");
    move_to(pos);
    //printf("FIX r2\n");
    //add blockloading here for replacing
}
