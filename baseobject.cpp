#include "baseobject.h"

#include <glm/gtc/matrix_transform.hpp>

baseobject::baseobject(size_t shape, size_t texture,size_t program,int NewObjID)
{
    ModelMatrix = glm::mat4(1.0);
    scale_size =1.0f;
    shapeID=shape;
    TextureID=texture;
    programID=program;
    ObjID=NewObjID;
}

baseobject::baseobject(size_t shape, size_t texture,size_t program,int NewObjID, glm::vec3 pos)
{
    ModelMatrix = glm::mat4(1.0);
    scale_size =1.0f;
    shapeID=shape;
    TextureID=texture;
    programID=program;
    ObjID=NewObjID;
    move(pos);
}

void baseobject::move(glm::vec3 dirvec)
{
    ModelMatrix=glm::translate(ModelMatrix,dirvec);
}

void baseobject::scale(float scale)
{
    scale_size*=scale;
    ModelMatrix=glm::scale(ModelMatrix,glm::vec3(scale,scale,scale));
}
