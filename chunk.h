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
    int* get_Block_list(int x,int y,int z){return &Block_list[x][y][z];}
    void set_Block_list(int x,int y,int z, int ID){Block_list[x][y][z]=ID;}
    void check_all_Blocks(){fill_buffers();}

    void set_neighbours(chunk* neighbours[6]){
        this->neighbours=neighbours;
    }
    void set_Block_list(int Block_list[16][16][16]){
        this->Block_list=Block_list;
    }

    void clone_alpha_cube(std::vector<unsigned short> cube_indices,
                          std::vector<glm::vec3> cube_indexed_vertices,
                          std::vector<glm::vec2> cube_indexed_uvs,
                          std::vector<glm::vec3> cube_indexed_normals);

    chunk** neighbours;
protected:

    void init_buffers();
    void fill_buffers();

protected:


    unsigned short cube_side_indices[6][6];
    glm::vec3 cube_side_vertices[6][4];
    glm::vec2 cube_side_uvs[6][4];
    glm::vec3 cube_side_normals[6][4];

private:

    int (*Block_list)[16][16];

    void init_lists();

    //void create_alpha_cube(); //alpha-cube


    int vertex_buffer_size;
    int element_buffer_size;
    int Quad_list[16][16][16][6];
    std::stack<int> free_stack;

    int get_space();
    void give_space(int space);
    void check_block(int x,int y,int z);
    void add_quad(int x,int y, int z, int side);   //decide where to write
    void remove_quad(int x,int y, int z, int side);
    void write_quad(int x,int y, int z, int side,int space); //writes in buffer
    void delete_quad(int space);



};

#endif // CHUNK_H