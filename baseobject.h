#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <glm/glm.hpp>

#include <stdio.h>

class baseobject
{
public:
    baseobject(size_t shaID, size_t texID, size_t program,int ObjID,glm::vec3 pos);
    void move(glm::vec3 dirvec);
    void scale(float scale);
    void follow(glm::mat4 cam_pos);

    glm::mat4 get_ModelMatrix(){return ModelMatrix;}
    size_t get_TextureID(){return TextureID;}
    size_t get_shapeID(){return shapeID;}
    size_t get_programmID(){return programID;}
    float get_scale(){return scale_size;}
    int get_objID(){return ObjID;}


protected:
    int ObjID;
    glm::mat4 ModelMatrix;
    size_t TextureID;
    size_t shapeID;
    size_t programID;
    float scale_size;
};

#endif // BASEOBJECT_H
