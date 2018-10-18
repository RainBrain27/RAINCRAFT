#include "chunk_obj.h"

chunk_obj::chunk_obj(size_t texture, size_t program, int NewObjID, glm::vec3 pos)
    :baseobject(0, texture,program,NewObjID, pos)
{

}
