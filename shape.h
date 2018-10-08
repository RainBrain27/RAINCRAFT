#ifndef SHAPE_H
#define SHAPE_H

#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>

#include <stdio.h>

class shape
{
public:
    shape(){}
    shape(const char * path);
    ~shape();
    void reinit(const char * path);
    void init_buffers();
    GLuint get_vertexbuffer(){return vertexbuffer;}
    GLuint get_uvbuffer(){return uvbuffer;}
    GLuint get_normalbuffer(){return normalbuffer;}
    GLuint get_elementbuffer(){return elementbuffer;}
    std::vector<unsigned short> get_indices(){return indices;}
    std::vector<glm::vec3> get_indexed_vertices(){return indexed_vertices;}
    std::vector<glm::vec2> get_indexed_uvs(){return indexed_uvs;}
    std::vector<glm::vec3> get_indexed_normals(){return indexed_normals;}

    size_t get_index_size(){return index_size;}

public:
    GLint index_size;

    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    GLuint elementbuffer;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
};

#endif // SHAPE_H
