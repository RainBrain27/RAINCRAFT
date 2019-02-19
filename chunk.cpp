#include "chunk.h"
#include "gl_main.h"

#include "common/objloader.hpp"
#include "common/vboindexer.hpp"

#include <stdio.h>
#include <string.h>
#include <vector>
#include <glm/glm.hpp>

#define repeat_i(n) for(int i = 0; i <n;i++)
#define repeat_j(n) for(int j = 0; j <n;j++)



chunk::chunk()
{
    init_lists();

    init_buffers();
}

chunk::~chunk()
{
    size_t s=vertexbuffers.size();
    for(size_t i=0;i<s;i++){
        remove_buffer();
    }
}

void chunk::change_block(int x, int y, int z, short ID)
{
    int b=length; //b weg machen
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
    max_buffer_size=16*16*2*6; // ein  achtel der maximalen flachen zur speicheroptimierung
}

void chunk::fill_buffers()
{
    int b=16;
    size_t buffer_number = vertexbuffers.size();
    for(size_t i=0; i<buffer_number; i++){
        remove_buffer();
    }

    free_short=0;
    //DANGER| DONT'COPY
    memset(Quad_list,-1,16*16*16*6*2); // fix: bei veranderung quad_list //notwendig???
    //warum zur hölle das *2 (ohne wird scheinbar nur die Hälfte neu platziert)
    //memset schreibt default immer 8 bit bei -1 FF, zwei -1 wird zu FFFF, funktioniert nur für -1
    /*
    for(int x=0;x<b;x++){ for(int y=0;y<b;y++){ for(int z=0;z<b;z++){ for(int side=0;side<6;side++){
                    Quad_list[x][y][z][side]=-1; } } } }
    */
    new_vertices.clear();
    new_uvs.clear();
    new_normals.clear();
    new_indices.clear();
    //size=0 -> auf max.größe bringen

    new_size=0; //stellt Anzahl der Elemente im Buffer da(entspricht .size mit pushback-konstrukt)
    size_t new_max_size=size_t(max_buffer_size);
    new_vertices.resize(new_max_size*4);
    new_uvs.resize(new_max_size*4);
    new_normals.resize(new_max_size*4);
    new_indices.resize(new_max_size*6);

    //blockcheck:
    for(int x=0;x<b;x++){
        for(int y=0;y<b;y++){
            for(int z=0;z<b;z++){
                check_block2(x, y, z);
            }
        }
    }
    if(new_indices.size()>0){
        if(free_short%max_buffer_size==0){
            paste_in_buffer(free_short/max_buffer_size-1);
        }
        else{
            paste_in_buffer(free_short/max_buffer_size);
        }

    }
    new_vertices.clear();
    new_uvs.clear();
    new_normals.clear();
    new_indices.clear();
}

//die -2 funktionen fullen nur listen und schreiben nicht auf die GPU

void chunk::check_block2(int x, int y, int z)
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
        if(not(Block_list[x][y][z]==-1)){
            for(int i=0;i<6;i++){
                if(dec[i]){ //liegt nicht am entsprechendem rand
                    if(Block_list[ x+dir[i][0] ][ y+dir[i][1] ][ z+dir[i][2] ] ==-1){
                        add_quad2(x,y,z,i);
                    }
                    /*
                    else{
                        remove_quad(x,y,z,i);
                    }*/
                }
                else{
                    if(neighbours[i]!=0){ //nachbarchunk existiert
                        if(neighbours[i]->Block_list[ ext[i][0] ][ ext[i][1] ][ ext[i][2] ] ==-1){
                            add_quad2(x,y,z,i);
                        }
                        /*
                        else{
                            remove_quad(x,y,z,i);
                        }*/
                    }
                    else{add_quad2(x,y,z,i); }
                }

            }
      }
}

void chunk::add_quad2(int x, int y, int z, int side)
{
    if(Block_list[x][y][z]!=-1){
        if(Quad_list[x][y][z][side]==-1){
            short space=get_space2();
            write_quad2(x,y,z,side,space);
            Quad_list[x][y][z][side]=space;
        }

        else if(Quad_list[x][y][z][side]!=-1){
            short space = Quad_list[x][y][z][side];
            write_quad2(x,y,z,side,space);
            Quad_list[x][y][z][side] = space;
        }
    }
    else{printf("Seltsamer add_quad2-Aufruf");}


}

void chunk::write_quad2(int x, int y, int z, int side, short space)
{
    //glm::vec3 pos(x-7.5,y-7.5,z-7.5);
    glm::vec3 pos(x+0.5,y+0.5,z+0.5);

    //size_t buffer_number = space/max_buffer_size;
    short buffer_space = space%max_buffer_size;

<<<<<<< HEAD
    for(size_t i=0;i<4;i++){
        new_vertices[new_size*4+i]=(cube_side_vertices[side][i]+pos);
        new_uvs[new_size*4+i]=(glm::vec3(cube_side_uvs[side][i],Block_list[x][y][z]));
        new_normals[new_size*4+i]=(cube_side_normals[side][i]);
    }
    for(size_t i=0;i<6;i++){
        new_indices[new_size*6+i]=(cube_side_indices[side][i]+buffer_space*4); // nicht korrekt, funktioniert nur wenn nachher unterteilt
    }
    new_size+=1;
}

short chunk::get_space2()
{
    short space=free_short;
    if(free_short%max_buffer_size==0){
        if(free_short>0){paste_in_buffer(free_short/max_buffer_size-1);}
        add_buffer();
        new_size=0;
    }
    free_short+=1;
    return space;
}

void chunk::paste_in_buffer(size_t buffer_number)
{

    int size=new_vertices.size()/4;
    //printf("hey %i %i\n",new_vertices.size()/4,size);
    if(new_vertices.size()!=0){
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[buffer_number]);
    glBufferSubData(GL_ARRAY_BUFFER,0,size*4*sizeof(glm::vec3),&new_vertices[0]);

    glBindBuffer(GL_ARRAY_BUFFER, uvbuffers[buffer_number]);
    glBufferSubData(GL_ARRAY_BUFFER,0,size*4*sizeof(glm::vec3),&new_uvs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, normalbuffers[buffer_number]);
    glBufferSubData(GL_ARRAY_BUFFER,0,size*4*sizeof(glm::vec3),&new_normals[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffers[buffer_number]);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0,size*6*sizeof(unsigned short),&new_indices[0]);
    }
    else{
        printf("nothing to draw!\n");
    }
    new_vertices.clear();
    new_indices.clear();
    new_normals.clear();
    new_uvs.clear();
}



void chunk::add_quad(int x, int y, int z, int side)
{
    if(Block_list[x][y][z]!=-1){
        if(Quad_list[x][y][z][side]==-1){
            short space=get_space();
            write_quad(x,y,z,side,space);
            Quad_list[x][y][z][side]=space;
        }

        else if(Quad_list[x][y][z][side]!=-1){
            short space = Quad_list[x][y][z][side];
            write_quad(x,y,z,side,space);
            Quad_list[x][y][z][side] = space;
        }
    }
    else{printf("Seltsamer add_quad-Aufruf");}
}

void chunk::remove_quad(int x, int y, int z, int side)
{
    short space=Quad_list[x][y][z][side];
    if(space!=-1){
        give_space(space);
        Quad_list[x][y][z][side]=-1;
    }
}

void chunk::write_quad(int x, int y, int z, int side, short space)
{
    glm::vec3 pos(x+0.5,y+0.5,z+0.5);
    //glm::vec3 pos(x-7.5,y-7.5,z-7.5);

    size_t buffer_number = space/max_buffer_size;
    short buffer_space = space%max_buffer_size;

    std::vector<glm::vec3> temp_v;
    std::vector<glm::vec3> temp_uv;
    repeat_i(4){
        temp_v.push_back(cube_side_vertices[side][i]+pos);
        temp_uv.push_back(glm::vec3(cube_side_uvs[side][i],Block_list[x][y][z]));
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[buffer_number]);
    glBufferSubData(GL_ARRAY_BUFFER,buffer_space*4*sizeof(glm::vec3),4*sizeof(glm::vec3),&temp_v[0]);

    glBindBuffer(GL_ARRAY_BUFFER, uvbuffers[buffer_number]);
    glBufferSubData(GL_ARRAY_BUFFER,buffer_space*4*sizeof(glm::vec3),4*sizeof(glm::vec3),&temp_uv[0]);

    glBindBuffer(GL_ARRAY_BUFFER, normalbuffers[buffer_number]);
    glBufferSubData(GL_ARRAY_BUFFER,buffer_space*4*sizeof(glm::vec3),4*sizeof(glm::vec3),cube_side_normals[side]);


    std::vector<unsigned short> temp_e;
    repeat_i(6){
        temp_e.push_back(cube_side_indices[side][i]+buffer_space*4);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffers[buffer_number]);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,buffer_space*6*sizeof(unsigned short),6*sizeof(unsigned short),&temp_e[0]);

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

short chunk::get_space()
{
    short space=free_short;
    if(free_short%max_buffer_size==0){
        add_buffer();
    }
    free_short+=1;
    return space;

}

void chunk::give_space(short space)
{
    if(free_short==space+1){
        free_short-=1;
        if(free_short%max_buffer_size==max_buffer_size-1){
            remove_buffer();
        }
    }
    else{
        int b=16;
        for(int x=0;x<b;x++){
            for(int y=0;y<b;y++){
                for(int z=0;z<b;z++){                       //ridicules
                    for(int side=0;side<6;side++){
                        if(Quad_list[x][y][z][side]==free_short-1){
                            Quad_list[x][y][z][side]=space;
                            write_quad(x,y,z,side,space);

                            free_short-=1;
                            if(free_short%max_buffer_size==0){// FIX : funktioniert immer?
                                remove_buffer();
                            }

                            return;
                        }
                    }
                }
            }
        }
    }
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

    free_short=0;
}

void chunk::add_buffer()
{
    int vertex_buffer_size=max_buffer_size*4;//vektoren
    int element_buffer_size=max_buffer_size*6;//ecken elemente

    size_t buffer_number = vertexbuffers.size();
    //size changes here
    vertexbuffers.push_back(0);
    uvbuffers.push_back(0);
    normalbuffers.push_back(0);
    elementbuffers.push_back(0);

    glGenBuffers(1, &vertexbuffers[buffer_number]);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[buffer_number]);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * sizeof(glm::vec3), 0, GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffers[buffer_number]);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffers[buffer_number]);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * sizeof(glm::vec3), 0, GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffers[buffer_number]);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffers[buffer_number]);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * sizeof(glm::vec3), 0, GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffers[buffer_number]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffers[buffer_number]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, element_buffer_size * sizeof(unsigned short), 0, GL_STATIC_DRAW);

}

void chunk::remove_buffer()
{
    size_t buffer_number = vertexbuffers.size()-1;
    if(buffer_number>=0){
        glDeleteBuffers(1, &vertexbuffers[buffer_number]);
        glDeleteBuffers(1, &uvbuffers[buffer_number]);
        glDeleteBuffers(1, &normalbuffers[buffer_number]);
        glDeleteBuffers(1, &elementbuffers[buffer_number]);

        vertexbuffers.pop_back();
        uvbuffers.pop_back();
        normalbuffers.pop_back();
        elementbuffers.pop_back();
    }
    else{
        printf("removing not existing buffer ?!\n");
    }
}
