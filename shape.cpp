#include "shape.h"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"

#include <stdio.h>


shape::shape(const char * path)//DONT USE THIS ---NEW : USE THIS!!!
{
    loadOBJ(path, vertices, uvs, normals);
    indexVBO_slow(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals); //gibt 28 vertex für cube zurück
    index_size=indices.size();

    init_buffers();

}

shape::~shape()
{
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &elementbuffer);
}

void shape::init_buffers()
{
    printf("size%i\n",sizeof(glm::vec3));
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), indexed_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), indexed_uvs.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), indexed_normals.data(), GL_STATIC_DRAW);



    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data() , GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
    //glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0,ndices.size() * sizeof(unsigned short),&indices[0]);

    //FIX INDEXING :: NOT NEEDED/BAD FOR CHUNKS
    printf("ID:FIX %i\n",indices.size());
    printf("ID:FIX %i\n",indexed_vertices.size());
}


