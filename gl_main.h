#ifndef GL_MAIN_H
#define GL_MAIN_H

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include "shape.h"
#include "baseobject.h"
#include "chunk_obj.h"


class gl_main
{
public:
    gl_main();
    void mainloop();

private:
    void initialize_libs();
    void loadIDs();
    void loadObjects();
    void paint();
    void measure_speed();
    void create_shapes();
    void loadTextures();
    void clean();

private:
     GLuint VertexArrayID;

     std::vector<GLuint> programIDs;
     std::vector<GLint> MatrixIDs;
     std::vector<GLint> ViewMatrixIDs;
     std::vector<GLint> ModelMatrixIDs;
     std::vector<GLint> TextureSamplerIDs;
     std::vector<GLint> LightIDs;


private:

     std::vector<GLuint> Textures;

     std::vector<shape*> shapes;

     std::vector<baseobject*> objects;
     //std::vector< chunk_obj* > chunks;
     //int chunk_sizes[3]={18,18,18};
     //chunk_obj* chunks[18][18][18];
     #define cc 4
     int chunk_sizes[3]={cc,cc,cc};
     int map_border[3]={0,0,0};
     int map_pos[3]={0,0,0};
     chunk_obj* chunks[cc][cc][cc];

     void move_chunks();

     size_t skycubeID;
     glm::vec3 lastCamPos;

private:
     double lastTime;
     int nbFrames;

};

#endif // GL_MAIN_H
