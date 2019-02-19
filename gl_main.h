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
    void actions();
    void playercolision();
    void selectBlock();
    void paint();
    void measure_speed();
    void create_shapes();
    void loadTextures();
    void clean();
    void move_chunks();

private:
    GLuint VertexArrayID;

    std::vector<GLuint> programIDs;
    std::vector<GLint> MatrixIDs;
    std::vector<GLint> ViewMatrixIDs;
    std::vector<GLint> ModelMatrixIDs;
    std::vector<GLint> TextureSamplerIDs;
    std::vector<GLint> LightIDs;
    GLint TextureArraySampler;


    GLuint CrossShader;
    GLuint Screenvertices;

    GLuint depthProgramID;
    GLint depthMatrixID;
    GLuint FramebufferName;
    GLuint depthTexture;
    GLint ShadowMapID;
    GLint DepthBiasID;

private:

     std::vector<GLuint> Textures;

     std::vector<shape*> shapes;

     std::vector<baseobject*> objects;
     //std::vector< chunk_obj* > chunks;
     //int chunk_sizes[3]={18,18,18};
     //chunk_obj* chunks[18][18][18];
     #define cc 6
     int chunk_sizes[3]={cc,cc,cc};
     int map_border[3]={0,0,0};
     int map_pos[3]={0,0,0};
     chunk_obj* chunks[cc][cc][cc];



     size_t skycubeID;
     glm::vec3 lastCamPos;

private:
     double lastTime;
     int nbFrames;
     int windowWidth = 1920;//1024;
     int windowHeight = 1080;//768;

private:
     float PlayerSize[3]  = {0.8f,1.8f,0.8f};
     float PlayerEyePos[3]= {0.4f,1.6f,0.4f};
     short getBlockat(int x, int y, int z);
     void setBlockat(int x, int y, int z, short mat);

public:
     GLuint4back get_chunk_buffer();
     void return_chunk_buffer(GLuint4back bufferz);
};

#endif // GL_MAIN_H
