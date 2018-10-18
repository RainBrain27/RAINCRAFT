#ifndef CHUNK_OBJ_H
#define CHUNK_OBJ_H

#include "chunk.h"
#include "baseobject.h"

class chunk_obj : public baseobject
{
public:
    chunk_obj(size_t texID, size_t program,int ObjID,glm::vec3 pos);
    chunk* get_chunk_shape(){return &chunk_shape;}
private:
    chunk chunk_shape;
};

#endif // CHUNK_OBJ_H
