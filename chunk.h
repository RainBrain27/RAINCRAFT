#ifndef CHUNK_H
#define CHUNK_H

#include "shape.h"
#include <vector>
#include <stack>

class chunk : public shape //16**3 blocke
{
public:
    chunk();
    void change_block(int x,int y,int z, int ID);

protected:
    void init_buffers();
    void fill_buffers();

protected:
    void add_quad(int x,int y, int z, int side);   //decide where to write
    void write_quad(int x,int y, int z, int side,int space); //writes in buffer
    void delete_quad(int space);


    int Block_list[16][16][16];
    int Quad_list[16][16][16][6];
    std::stack<int> free_stack;

    unsigned short cube_side_indices[6][6];
    glm::vec3 cube_side_vertices[6][4];
    glm::vec2 cube_side_uvs[6][4];
    glm::vec3 cube_side_normals[6][4];

private:
    void create_alpha_cube(); //alpha-cube
    int vertex_buffer_size;
    int element_buffer_size;

    int get_space();
    void give_space(int space);

    void check_block(int x,int y,int z);

    void init_lists();
    void generate_chunk();

};

#endif // CHUNK_H
