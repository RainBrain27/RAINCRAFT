#include "chunk.h"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"

#include <stdio.h>

#define repeat_i(n) for(int i = 0; i <n;i++)
#define repeat_j(n) for(int j = 0; j <n;j++)

chunk::chunk()

{
    create_alpha_cube();

    init_lists();

    generate_chunk();

    init_buffers();

    fill_buffers();

}

void chunk::change_block(int x, int y, int z, int ID)
{
    int b=16;
    if(Block_list[x][y][z]!=ID){
        Block_list[x][y][z]=ID;

        check_block(x,y,z);
        if(x>0){check_block(x-1,y,z);}
        if(x<b-1){check_block(x+1,y,z);}
        if(y>0){check_block(x,y-1,z);}
        if(y<b-1){check_block(x,y+1,z);}
        if(z>0){check_block(x,y,z-1);}
        if(z<b-1){check_block(x,y,z+1);} //check range

    }
}

void chunk::init_buffers()
{
    vertex_buffer_size=16*16*8*24;//vektoren
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
    printf("FIX 27 %i %i %i %i\n",x,y,z,side);

    if(Quad_list[x][y][z][side]==-1 and Block_list[x][y][z]==-1){
        //printf("fehlaufruf draw quads");
    }
    else if(Quad_list[x][y][z][side]==-1 and Block_list[x][y][z]!=-1){
        printf("    quad == != %i",side);
        int space=get_space();
        write_quad(x,y,z,side,space);
        Quad_list[x][y][z][side]=space;
    }
    else if(Quad_list[x][y][z][side]!=-1 and Block_list[x][y][z]!=-1){
        printf("    quad != != %i",side);
        int space=Quad_list[x][y][z][side];
        write_quad(x,y,z,side,space);
        Quad_list[x][y][z][side]=space;
    }
    else if(Quad_list[x][y][z][side]!=-1 and Block_list[x][y][z]==-1){
        int space=Quad_list[x][y][z][side];
        delete_quad(space);
        give_space(space);
        Quad_list[x][y][z][side]=-1;
    }

}

void chunk::write_quad(int x, int y, int z, int side, int space)
{
    printf("FIX7 %i %i \n",space,side);

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
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferSubData(GL_ARRAY_BUFFER,space*4* sizeof(glm::vec3),4* sizeof(glm::vec3),0);

    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferSubData(GL_ARRAY_BUFFER,space*4* sizeof(glm::vec2),4* sizeof(glm::vec2),0);

    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferSubData(GL_ARRAY_BUFFER,space*4* sizeof(glm::vec3),4* sizeof(glm::vec3),0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,space*6* sizeof(unsigned short),6 * sizeof(unsigned short),0);
}

void chunk::create_alpha_cube()
{
    std::vector<glm::vec3> cube_vertices;
    std::vector<glm::vec2> cube_uvs;
    std::vector<glm::vec3> cube_normals;

    std::vector<unsigned short> cube_indices;
    std::vector<glm::vec3> cube_indexed_vertices;
    std::vector<glm::vec2> cube_indexed_uvs;
    std::vector<glm::vec3> cube_indexed_normals;

    loadOBJ("objects/cube.obj", cube_vertices, cube_uvs, cube_normals);
    indexVBO(cube_vertices, cube_uvs, cube_normals, cube_indices, cube_indexed_vertices, cube_indexed_uvs, cube_indexed_normals);

    repeat_i(6){//for
        repeat_j(6){
            cube_side_indices[i][j]=cube_indices[6*i+j]-i*4;
        }
        repeat_j(4){
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
        printf("stack_top= %i \n",space);
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
        int b=16;

        if(x>0){if(Block_list[x-1][y][z]==-1){add_quad(x,y,z,1);}}
        else{add_quad(x,y,z,1);}

        if(x<b-1){if(Block_list[x+1][y][z]==-1){add_quad(x,y,z,3);}}
        else{add_quad(x,y,z,3);}

        if(z>0){if(Block_list[x][y][z-1]==-1){add_quad(x,y,z,0);}}
        else{add_quad(x,y,z,0);}

        if(z<b-1){if(Block_list[x][y][z+1]==-1){add_quad(x,y,z,2);}}
        else{add_quad(x,y,z,2);}

        if(y>0){if(Block_list[x][y-1][z]==-1){add_quad(x,y,z,5);}}
        else{add_quad(x,y,z,5);}

        if(y<b-1){if(Block_list[x][y+1][z]==-1){add_quad(x,y,z,4);}}
        else{add_quad(x,y,z,4);}
}

void chunk::init_lists()
{
    int b=16;
    for(int x=0;x<b;x++){
        for(int y=0;y<b;y++){
            for(int z=0;z<b;z++){
                Block_list[x][y][z]=-1;
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

void chunk::generate_chunk()
{
    int ID=1;
    int b=16;
    for(int x=0;x<b;x++){
        for(int y=0;y<b;y++){
            for(int z=0;z<b;z++){
                Block_list[x][y][z]=ID;
            }
        }
    }
}
