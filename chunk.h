#ifndef CHUNK_H
#define CHUNK_H

//#include "shape.h"
#include <vector>
#include <stack>
#include <GL/glew.h>
#include <glm/glm.hpp>

class chunk //: public shape //16**3 blocke
{
public:
    chunk();
    ~chunk();

    GLint get_index_size(size_t buffer_number){
        if(buffer_number<vertexbuffers.size()-1){return max_buffer_size*6;}
        else{return (free_short-max_buffer_size*buffer_number)*6;}
    }

    std::vector<GLuint> get_vertexbuffers(){return vertexbuffers;}
    std::vector<GLuint> get_uvbuffers(){return uvbuffers;}
    std::vector<GLuint> get_normalbuffers(){return normalbuffers;}
    std::vector<GLuint> get_elementbuffers(){return elementbuffers;}


    void change_block(int x,int y,int z, short ID);
    short* get_Block_list(int x,int y,int z){return &Block_list[x][y][z];}
    void set_Block_list(int x,int y,int z, short ID){Block_list[x][y][z]=ID;}
    void refresh_chunk(){fill_buffers();}

    void set_neighbours(chunk* neighbours[6]){this->neighbours=neighbours;}
    void set_Block_list(short Block_list[16][16][16]){this->Block_list=Block_list;}

    void clone_alpha_cube(std::vector<unsigned short> cube_indices,
                          std::vector<glm::vec3> cube_indexed_vertices,
                          std::vector<glm::vec2> cube_indexed_uvs,
                          std::vector<glm::vec3> cube_indexed_normals);

    chunk** neighbours;
private:

    void init_buffers();
    void fill_buffers();
    std::vector<glm::vec3> new_vertices;
    std::vector<glm::vec2> new_uvs;
    std::vector<glm::vec3> new_normals;
    std::vector<unsigned short> new_indices;
    void check_block2(int x,int y,int z);
    void add_quad2(int x,int y, int z, int side);
    void write_quad2(int x,int y, int z, int side,short space);
    short get_space2();
    void paste_in_buffer(size_t buffer_number);

    void init_lists();

    unsigned short cube_side_indices[6][6];
    glm::vec3 cube_side_vertices[6][4];
    glm::vec2 cube_side_uvs[6][4];
    glm::vec3 cube_side_normals[6][4];

private:

    short (*Block_list)[16][16];

    //
    int length=16;
    int max_buffer_size;

    std::vector<GLuint> vertexbuffers;
    std::vector<GLuint> uvbuffers;
    std::vector<GLuint> normalbuffers;
    std::vector<GLuint> elementbuffers;

    void add_buffer();
    void remove_buffer();


private:
    short Quad_list[16][16][16][6];
    //gibt Block speicher-short für alle Seiten des Blocks an
    short free_short=0;
    //bool writing = true;

    short get_space();
    void give_space(short space);
    void check_block(int x,int y,int z);
    void add_quad(int x,int y, int z, int side);   //decide where to write
    void remove_quad(int x,int y, int z, int side);
    void write_quad(int x,int y, int z, int side,short space); //writes in buffer



};

#endif // CHUNK_H
