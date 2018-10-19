#include "chunk.h"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"

#include <stdio.h>

#define repeat_i(n) for(int i = 0; i <n;i++)
#define repeat_j(n) for(int j = 0; j <n;j++)






chunk::chunk()

{
    //create_alpha_cube();

    init_lists();

    init_buffers();
}



void chunk::change_block(int x, int y, int z, int ID)
{
    int b=16;
    if(Block_list[x][y][z]!=ID){
        Block_list[x][y][z]=ID;

        int dir[6][3]={
        {x  ,y  ,z-1},
        {x-1,y  ,z  },
        {x  ,y  ,z+1},
        {x+1,y  ,z  },
        {x  ,y+1,z  },
        {x  ,y-1,z  }
        };

        bool dec[6]={z>0,x>0,z<b-1,x<b-1,y<b-1,y>0};

        int ext[6][3]={
        {x,y,b-1},
        {b-1,y,z},
        {x,y,0},
        {0,y,z},
        {x,0,z},
        {x,b-1,z}
        };

        check_block(x,y,z);

        for(size_t i=0;i<6;i++){
            if(dec[i]){check_block(dir[i][0],dir[i][1],dir[i][2]);}
            else if(neighbours[i]!=0){
                neighbours[i]->check_block(ext[i][0],ext[i][1],ext[i][2]);
            }
        }
    }
}

void chunk::init_buffers()
{
    this->vertex_buffer_size=16*16*8*24;//vektoren //+1 for 1 empty space for clearing//not neccessary
    element_buffer_size=16*16*8*36;//ecken elemente

    index_size=element_buffer_size; //for render

    //FIX:change values in more complex times

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * sizeof(glm::vec3), 0, GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * sizeof(glm::vec2), 0, GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * sizeof(glm::vec3), 0, GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, element_buffer_size * sizeof(unsigned short), 0, GL_STATIC_DRAW);

}

void chunk::fill_buffers()
{
    int b=16;
    for(int x=0;x<b;x++){
        for(int y=0;y<b;y++){
            for(int z=0;z<b;z++){
                check_block(x, y, z);
            }
        }
    }
}

void chunk::add_quad(int x, int y, int z, int side)
{
    if(Block_list[x][y][z]!=-1){
        if(Quad_list[x][y][z][side]==-1){
                int space=get_space();
                write_quad(x,y,z,side,space);
                Quad_list[x][y][z][side]=space;
            }

        else if(Quad_list[x][y][z][side]!=-1){
            int space = Quad_list[x][y][z][side];
            write_quad(x,y,z,side,space);
            Quad_list[x][y][z][side] = space;
        }
    }
    else{printf("Seltsamer add_quad-Aufruf");}
}

void chunk::remove_quad(int x, int y, int z, int side)
{
    int space=Quad_list[x][y][z][side];
    if(space!=-1){
        delete_quad(space);
        give_space(space);
        Quad_list[x][y][z][side]=-1;
    }
}

void chunk::write_quad(int x, int y, int z, int side, int space)
{

    glm::vec3 pos(x-7.5,y-7.5,z-7.5);
    std::vector<glm::vec3> temp_v;
    repeat_i(4){
        temp_v.push_back(cube_side_vertices[side][i]+pos);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferSubData(GL_ARRAY_BUFFER,space*4*sizeof(glm::vec3),4*sizeof(glm::vec3),&temp_v[0]);

    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferSubData(GL_ARRAY_BUFFER,space*4*sizeof(glm::vec2),4*sizeof(glm::vec2),cube_side_uvs[side]);

    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferSubData(GL_ARRAY_BUFFER,space*4*sizeof(glm::vec3),4*sizeof(glm::vec3),cube_side_normals[side]);


    std::vector<unsigned short> temp_e;
    repeat_i(6){
        temp_e.push_back(cube_side_indices[side][i]+space*4);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,space*6*sizeof(unsigned short),6*sizeof(unsigned short),&temp_e[0]);
}

void chunk::delete_quad(int space)
{
    std::vector<unsigned short> O;
    repeat_i(6){
        //O.push_back(16*16*8*24); //aufpassen bei größenveränderung des Buffer
        O.push_back(1000000000);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,space*6* sizeof(unsigned short),6 * sizeof(unsigned short),&O[0]);

    //AUTSCH::WARNING
    //OPTIMIZE
    //learn to clear data
}

void chunk::clone_alpha_cube(std::vector<unsigned short> cube_indices, std::vector<glm::vec3> cube_indexed_vertices, std::vector<glm::vec2> cube_indexed_uvs, std::vector<glm::vec3> cube_indexed_normals)
{
    for(size_t i=0;i<6;i++){//for
        for(size_t j=0;j<6;j++){
            cube_side_indices[i][j]=cube_indices[6*i+j]-i*4;
        }
        for(size_t j=0;j<4;j++){
            cube_side_vertices[i][j]=cube_indexed_vertices[i*4+j];
            cube_side_uvs[i][j]=cube_indexed_uvs[i*4+j];
            cube_side_normals[i][j]=cube_indexed_normals[i*4+j];
        }
    }
}

int chunk::get_space()
{
    if(not free_stack.empty()){
        int space=free_stack.top();
        free_stack.pop();
        return space;
    }
    else{
        printf("ERROR:FULL_HOUSE");
        return 0;
    }
}

void chunk::give_space(int space)
{
    free_stack.push(space);
}

void chunk::check_block(int x, int y, int z)
{
    //z- x- z+ x+ y+ y-
        int b=16;

        int dir[6][3]={
        {0,0,-1},
        {-1,0,0},
        {0,0,1},
        {1,0,0},
        {0,1,0},
        {0,-1,0}
        };

        bool dec[6]={z>0,x>0,z<b-1,x<b-1,y<b-1,y>0};

        int ext[6][3]={
        {x,y,b-1},
        {b-1,y,z},
        {x,y,0},
        {0,y,z},
        {x,0,z},
        {x,b-1,z}
        };

        if(Block_list[x][y][z]==-1){
            remove_quad(x,y,z,0);
            remove_quad(x,y,z,1);
            remove_quad(x,y,z,2);
            remove_quad(x,y,z,3);
            remove_quad(x,y,z,4);
            remove_quad(x,y,z,5);
        }
        else{
            for(int i=0;i<6;i++){
                if(dec[i]){ //liegt nicht am entsprechendem rand
                    if(Block_list[ x+dir[i][0] ][ y+dir[i][1] ][ z+dir[i][2] ] ==-1){
                        add_quad(x,y,z,i);
                    }
                    else{
                        remove_quad(x,y,z,i);
                    }
                }
                else{
                    if(neighbours[i]!=0){ //nachbarchunk existiert
                        if(neighbours[i]->Block_list[ ext[i][0] ][ ext[i][1] ][ ext[i][2] ] ==-1){
                            add_quad(x,y,z,i);
                        }
                        else{
                            remove_quad(x,y,z,i);
                        }
                    }
                    else{add_quad(x,y,z,i); }
                }

            }
        }
}

void chunk::init_lists()
{
    int b=16;
    for(int x=0;x<b;x++){
        for(int y=0;y<b;y++){
            for(int z=0;z<b;z++){
                for(int side=0;side<6;side++){
                    Quad_list[x][y][z][side]=-1;
                }
            }
        }
    }

    for(int i=0;i<16*16*8*6;i++){ //fur alle quads
        free_stack.push(i); //WARNING: BUG HERE : funktioniert nicht ohne 12287, sollte aber das selbe sein ###FIXED: IS THERE A SUN-GLAS EMOJY?
    }

}
