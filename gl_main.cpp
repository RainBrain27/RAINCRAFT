#include "gl_main.h"


// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "objloader.hpp"
#include "vboindexer.hpp"

#include "chunk.h"
#
void APIENTRY DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam){

    printf("OpenGL Debug Output message : ");

    if(source == GL_DEBUG_SOURCE_API_ARB)					printf("Source : API; ");
    else if(source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)	printf("Source : WINDOW_SYSTEM; ");
    else if(source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)	printf("Source : SHADER_COMPILER; ");
    else if(source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)		printf("Source : THIRD_PARTY; ");
    else if(source == GL_DEBUG_SOURCE_APPLICATION_ARB)		printf("Source : APPLICATION; ");
    else if(source == GL_DEBUG_SOURCE_OTHER_ARB)			printf("Source : OTHER; ");

    if(type == GL_DEBUG_TYPE_ERROR_ARB)						printf("Type : ERROR; ");
    else if(type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)	printf("Type : DEPRECATED_BEHAVIOR; ");
    else if(type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)	printf("Type : UNDEFINED_BEHAVIOR; ");
    else if(type == GL_DEBUG_TYPE_PORTABILITY_ARB)			printf("Type : PORTABILITY; ");
    else if(type == GL_DEBUG_TYPE_PERFORMANCE_ARB)			printf("Type : PERFORMANCE; ");
    else if(type == GL_DEBUG_TYPE_OTHER_ARB)				printf("Type : OTHER; ");

    if(severity == GL_DEBUG_SEVERITY_HIGH_ARB)				printf("Severity : HIGH; ");
    else if(severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)		printf("Severity : MEDIUM; ");
    else if(severity == GL_DEBUG_SEVERITY_LOW_ARB)			printf("Severity : LOW; ");

    // You can set a breakpoint here ! Your debugger will stop the program,
    // and the callstack will immediately show you the offending call.
    printf("Message : %s\n", message);
}

gl_main::gl_main()
{
    initialize_libs();
    loadIDs();
    loadTextures();
    create_shapes();
    loadObjects();
}

void gl_main::initialize_libs()
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        //return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // ARB_debug_output is a bit special,
    // it requires creating the OpenGL context
    // with paticular flags.
    // GLFW exposes it this way; if you use SDL, SFML, freeGLUT
    // or other, check the documentation.
    // If you use custom code, read the spec :
    // http://www.opengl.org/registry/specs/ARB/debug_output.txt
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( windowWidth, windowHeight, "RAINCRAFT", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        //return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        //return -1;
    }



    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, windowWidth/2, windowHeight/2);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    glEnable(GL_DEPTH_TEST); // SHOULD BE GL_DEPTH_TEST ! WILL TRIGGER AN ERROR MESSAGE !
    glDepthFunc(GL_LESS);  // SHOULD BE GL_LESS ! WILL TRIGGER AN ERROR MESSAGE !
    glEnable(GL_CULL_FACE); // SHOULD BE GL_CULL_FACE ! WILL TRIGGER AN ERROR MESSAGE !
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void gl_main::loadIDs()
{


    std::vector<const char *> vertexpaths;
    std::vector<const char *> fragmentpaths;
    vertexpaths.push_back("shaders/StandardShading.vertexshader");fragmentpaths.push_back("shaders/StandardShading.fragmentshader");
    vertexpaths.push_back("shaders/StandardShading.vertexshader");fragmentpaths.push_back("shaders/SkycubeShading.fragmentshader");

    for(size_t i=0;i<vertexpaths.size();i++){
        programIDs.push_back(LoadShaders( vertexpaths[i], fragmentpaths[i] ));

        MatrixIDs.push_back(glGetUniformLocation(programIDs[i], "P")); //change name
        ViewMatrixIDs.push_back(glGetUniformLocation(programIDs[i], "V"));
        ModelMatrixIDs.push_back(glGetUniformLocation(programIDs[i], "M"));
        TextureSamplerIDs.push_back(glGetUniformLocation(programIDs[i], "myTextureSampler"));
        LightIDs.push_back(glGetUniformLocation(programIDs[i], "LightPosition_worldspace"));
    }
    vertexpaths.push_back("shaders/ChunkShading.vertexshader");fragmentpaths.push_back("shaders/ChunkShading.fragmentshader");//fur chunks
    size_t i=vertexpaths.size()-1;
    programIDs.push_back(LoadShaders( vertexpaths[i], fragmentpaths[i] ));
    MatrixIDs.push_back(glGetUniformLocation(programIDs[i], "P")); //change name
    ViewMatrixIDs.push_back(glGetUniformLocation(programIDs[i], "V"));
    ModelMatrixIDs.push_back(glGetUniformLocation(programIDs[i], "M"));
    TextureArraySampler = glGetUniformLocation(programIDs[i], "myTextureArraySampler");
    LightIDs.push_back(glGetUniformLocation(programIDs[i], "LightPosition_worldspace"));
    ShadowMapID = glGetUniformLocation(programIDs[i], "shadowMap");
    DepthBiasID = glGetUniformLocation(programIDs[i], "DepthBiasMVP");

    //vertexpaths.push_back("shaders/Cross.vertexshader");fragmentpaths.push_back("shaders/Cross.fragmentshader");//fur chunks
    CrossShader=LoadShaders("shaders/Cross.vertexshader", "shaders/Cross.fragmentshader");

    depthProgramID=LoadShaders("shaders/DepthRTT.vertexshader", "shaders/DepthRTT.fragmentshader" );
    depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");


    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("depth framebuffer failed");
}

void gl_main::create_shapes()
{
    std::vector<const char *> paths;
    paths.push_back("objects/skycube.obj");
    paths.push_back("objects/cube.obj");
    //paths.push_back("objects/suzanne.obj");
    //paths.push_back("objects/rathalos.obj");



    //in this order it works ... (?)
    for(size_t i=0;i<paths.size();i++){
        shapes.push_back(new shape(paths[i]));
    }
    //shapes.push_back(new chunk());


}

void gl_main::loadTextures()
{

    Textures.push_back(loadDDS("textures/uvmap.DDS",false));
    Textures.push_back(loadDDS("textures/rathalos1.DDS",false));
    Textures.push_back(loadDDS("textures/skycube.DDS",true));
    Textures.push_back(loadDDS("textures/crafting_table.DDS",true));
    Textures.push_back(loadBMP_custom("textures/grass_block.bmp"));

    std::vector<const char *> imgpaths;
    imgpaths.push_back("textures/crafting_table.bmp");
    imgpaths.push_back("textures/grass_block.bmp");
    imgpaths.push_back("textures/skywood.bmp");
    Textures.push_back(loadBMP_array(imgpaths));

}

void gl_main::loadObjects()
{
    std::vector<glm::vec3> cube_vertices;
    std::vector<glm::vec2> cube_uvs;
    std::vector<glm::vec3> cube_normals;

    std::vector<unsigned short> cube_indices;
    std::vector<glm::vec3> cube_indexed_vertices;
    std::vector<glm::vec2> cube_indexed_uvs;
    std::vector<glm::vec3> cube_indexed_normals;

    loadOBJ("objects/cube.obj", cube_vertices, cube_uvs, cube_normals);
    indexVBO(cube_vertices, cube_uvs, cube_normals, cube_indices, cube_indexed_vertices, cube_indexed_uvs, cube_indexed_normals);


    objects.push_back(new baseobject(0,2,1,0,glm::vec3(0,0,0))); //skycube
    objects[0]->scale(256.0);
    skycubeID=0;

    //objects.push_back(new baseobject(0,0,0,0,glm::vec3(0,-10,0)));

    //objects.push_back(new baseobject(1,1,0,1,glm::vec3(0,10,0)));

    //objects.push_back(new baseobject(1,2,0,2,glm::vec3(0,0,0)));

    int b=chunk_sizes[0];
    int h=chunk_sizes[1];
    int l=chunk_sizes[2];


    /*
    int bufferCount=b*h*l* 2 ; //vergrossern um mehr platz zu haben, lastet VRAM stark aus (max statt 2 -> 8)
    //! doesnot  risize atomatcly in clean()
    //!
    int max_buffer_size=16*16*2*6; // ein  achtel der maximalen flachen zur speicheroptimierung
    int vertex_buffer_size=max_buffer_size*4;//vektoren
    int element_buffer_size=max_buffer_size*6;//ecken elemente
    //allocate

    chunk_vertex_buffer_stack.resize(bufferCount);
    chunk_uv_buffer_stack.resize(bufferCount);
    chunk_normal_buffer_stack.resize(bufferCount);
    chunk_element_buffer_stack.resize(bufferCount);

    glGenBuffers(bufferCount, &chunk_vertex_buffer_stack[0]);
    glGenBuffers(bufferCount, &chunk_uv_buffer_stack[0]);
    glGenBuffers(bufferCount, &chunk_normal_buffer_stack[0]);
    glGenBuffers(bufferCount, &chunk_element_buffer_stack[0]);

    for(size_t buffer_i=0;buffer_i<bufferCount;buffer_i++){
        glBindBuffer(GL_ARRAY_BUFFER, chunk_vertex_buffer_stack[buffer_i]);
        glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * sizeof(glm::vec3), 0, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, chunk_uv_buffer_stack[buffer_i]);
        glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * sizeof(glm::vec3), 0, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, chunk_normal_buffer_stack[buffer_i]);
        glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size * sizeof(glm::vec3), 0, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk_element_buffer_stack[buffer_i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, element_buffer_size * sizeof(unsigned short), 0, GL_DYNAMIC_DRAW);
    }
    */

    float d=b/2.0f;
    float dh=h/2.0f;
    float dl=l/2.0f;


    for(int x=0;x<b;x++){
        printf("%i\n",x);
        for(int y=0;y<h;y++){
            for(int z=0;z<l;z++){
                chunks[x][y][z]=new chunk_obj(3,  //texture
                                               2,  //program
                                               1,  //ObjID (not needed)
                                               glm::vec3((x-d)*16,(y-dh)*16,(z-dl)*16), //position
                                               cube_indices,
                                               cube_indexed_vertices,
                                               cube_indexed_uvs,
                                               cube_indexed_normals,
                                               this
                                               );
                //printf("%i \n",x*b*b+y*b+z);
            }
        }
    }
    printf("taste");
    for(int x=0;x<b;x++){
        for(int y=0;y<h;y++){
            for(int z=0;z<l;z++){
                if(x>0){
                    chunks[x][y][z]->set_neighbor(chunks[x-1][y][z],1);
                    chunks[x-1][y][z]->set_neighbor(chunks[x][y][z],3);
                }
                if(y>0){
                    chunks[x][y][z]->set_neighbor(chunks[x][y-1][z],5);
                    chunks[x][y-1][z]->set_neighbor(chunks[x][y][z],4);
                }
                if(z>0){
                    chunks[x][y][z]->set_neighbor(chunks[x][y][z-1],0);
                    chunks[x][y][z-1]->set_neighbor(chunks[x][y][z],2);
                }
            }
        }
    }
    printf("TIME");
    for(int x=0;x<b;x++){
        printf("%i\n",x);
        for(int y=0;y<h;y++){
            for(int z=0;z<l;z++){
                chunks[x][y][z]->refresh_chunk();
            }
        }
    }
    //chunks[0][0][0]->refresh_chunk();
    //chunks[0][0][0]->refresh_chunk();
    printf("TO");

    for(int i=0;i<b*16;i++){
        int x=42,y=30;
        chunks[x/16][y/16][i/16]->change_block(x%16,y%16,i%16,-1);
        y=31;
        chunks[x/16][y/16][i/16]->change_block(x%16,y%16,i%16,-1);
        y=32;
        chunks[x/16][y/16][i/16]->change_block(x%16,y%16,i%16,-1);
        y=30;x=43;
        chunks[x/16][y/16][i/16]->change_block(x%16,y%16,i%16,-1);
        y=31;
        chunks[x/16][y/16][i/16]->change_block(x%16,y%16,i%16,-1);
        y=32;
        chunks[x/16][y/16][i/16]->change_block(x%16,y%16,i%16,-1);

    }
    printf("DIE");



        static const GLfloat g_quad_vertex_buffer_data[] = {
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
        };

        glGenBuffers(1, &Screenvertices);
        glBindBuffer(GL_ARRAY_BUFFER, Screenvertices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

}

void gl_main::actions()
{
    computeInputs();
    //ausrichten skycube

    move_chunks();

    playercolision();
    selectBlock();

    computeMatrices();

    float scale= objects[skycubeID]->get_scale();
    objects[skycubeID]->move(glm::vec3(
                                 (getCamPos().x-lastCamPos.x)/scale,
                                 (getCamPos().y-lastCamPos.y)/scale,
                                 (getCamPos().z-lastCamPos.z)/scale));


    lastCamPos=getCamPos();
}

void gl_main::playercolision()
{

    float t;
    float dist=999999.0f; //naja
    short material = -1;
    glm::vec3 Block=glm::vec3(0,0,0);
    int cancel =0;

    glm::vec3 v;
    glm::vec3 absolMin;
    glm::vec3 absolMax;
    glm::vec3 newMin;
    glm::vec3 newMax;
    glm::vec3 newCamPos;

    glm::vec3 lastMin;
    glm::vec3 lastMax;

    short mat;
    float tec;
    float dist2;

    int side=1;

    float f=+0.00001f;
    while(side!=-1 and cancel<10){
        cancel+=1;
        side=-1;
        t=1.0;

        newCamPos = getCamPos();

        newMin=newCamPos-glm::vec3(PlayerEyePos[0],PlayerEyePos[1],PlayerEyePos[2]);
        newMax=newMin+glm::vec3(PlayerSize[0],PlayerSize[1],PlayerSize[2]);

        lastMin=lastCamPos-glm::vec3(PlayerEyePos[0],PlayerEyePos[1],PlayerEyePos[2]);
        lastMax=lastMin+glm::vec3(PlayerSize[0],PlayerSize[1],PlayerSize[2]);

        absolMin=glm::vec3(
                    std::min(newMin.x,lastMin.x),
                    std::min(newMin.y,lastMin.y),
                    std::min(newMin.z,lastMin.z));
        absolMax=glm::vec3(
                    std::max(newMax.x,lastMax.x),
                    std::max(newMax.y,lastMax.y),
                    std::max(newMax.z,lastMax.z));

        // 0, 1, 2, 3, 4, 5 z- x- z+ x+ y+ y-
        // z- x- z+ x+ y+ y-

        v=newMin-lastMin;
        for(int x=int(std::floor(absolMin.x));x<=std::floor(absolMax.x);x++){
            for(int y=int(std::floor(absolMin.y));y<=std::floor(absolMax.y);y++){
                for(int z=int(std::floor(absolMin.z));z<=std::floor(absolMax.z);z++){
                    mat=getBlockat(x,y,z);
                    if(mat!=-1){
                        if(lastMin.y+f>=y+1){if(newMin.y+f<y+1){
                                tec =((y+1.0f)-lastMin.y)/(v.y);
                                dist2 = distance(lastMin+glm::vec3(PlayerSize[0]/2,0,PlayerSize[2]/2),glm::vec3(x+0.5,y+1.0,z+0.5));
                                if(tec<t or (tec+f<t and dist2<dist and side!=-1)){glm::vec3 SP=lastMin+v*tec;
                                    if(SP.x+f<x+1 and SP.x+PlayerSize[0]>x+f and SP.z+f<z+1 and SP.z+PlayerSize[2]>z+f){
                                        dist=dist2;
                                        t=tec;material=mat;
                                        side=4;//of block
                                        Block=glm::vec3(x,y,z);
                        }}}}
                        if(lastMin.x+f>=x+1){if(newMin.x+f<x+1){
                                tec =((x+1.0f)-lastMin.x)/(v.x);
                                dist2 = distance(lastMin+glm::vec3(0,PlayerSize[1]/2,PlayerSize[2]/2),glm::vec3(x+1.0,y+0.5,z+0.5));
                                if(tec<t or (tec+f<t and dist2<dist and side!=-1)){glm::vec3 SP=lastMin+v*tec;
                                    if(SP.y+f<y+1 and SP.y+PlayerSize[1]>y+f and SP.z+f<z+1 and SP.z+PlayerSize[2]>z+f){
                                        dist=dist2;
                                        t=tec;material=mat;
                                        side=3;//of block
                                        Block=glm::vec3(x,y,z);
                        }}}}
                        if(lastMin.z+f>=z+1){if(newMin.z+f<z+1){
                                tec =((z+1.0f)-lastMin.z)/(v.z);
                                dist2 = distance(lastMin+glm::vec3(PlayerSize[0]/2,PlayerSize[1]/2,0),glm::vec3(x+0.5,y+0.5,z+1.0));
                                if(tec<t or (tec+f<t and dist2<dist and side!=-1)){glm::vec3 SP=lastMin+v*tec;
                                    if(SP.x+f<x+1 and SP.x+PlayerSize[0]>x+f and SP.y+f<y+1 and SP.y+PlayerSize[1]>y+f){
                                        t=tec;material=mat;
                                        dist=dist2;
                                        side=2;//of block
                                        Block=glm::vec3(x,y,z);
                        }}}}

                        if(lastMax.y<=y+f){if(newMax.y>y+f){
                                tec =((y)-lastMax.y)/(v.y);
                                dist2 = distance(lastMin+glm::vec3(PlayerSize[0]/2,PlayerSize[1],PlayerSize[2]/2),glm::vec3(x+0.5,y,z+0.5));
                                if(tec<t or (tec+f<t and dist2<dist and side!=-1)){glm::vec3 SP=lastMin+v*tec;
                                    if(SP.x+f<x+1 and SP.x+PlayerSize[0]>x+f and SP.z+f<z+1 and SP.z+PlayerSize[2]>z+f){
                                        t=tec;material=mat;
                                        dist=dist2;
                                        side=5;//of block
                                        Block=glm::vec3(x,y,z);
                        }}}}
                        if(lastMax.x<=x+f){if(newMax.x>x+f){
                                tec =((x)-lastMax.x)/(v.x);
                                dist2 = distance(lastMin+glm::vec3(PlayerSize[0],PlayerSize[1]/2,PlayerSize[2]/2),glm::vec3(x,y+0.5,z+0.5));
                                if(tec<t or (tec+f<t and dist2<dist and side!=-1)){glm::vec3 SP=lastMin+v*tec;
                                    if(SP.y+f<y+1 and SP.y+PlayerSize[1]>y+f and SP.z+f<z+1 and SP.z+PlayerSize[2]>z+f){
                                        t=tec;material=mat;
                                        dist=dist2;
                                        side=1;//of block
                                        Block=glm::vec3(x,y,z);
                        }}}}
                        if(lastMax.z<=z+f){if(newMax.z>z+f){
                                tec =((z)-lastMax.z)/(v.z);
                                dist2 = distance(lastMin+glm::vec3(PlayerSize[0]/2,PlayerSize[1]/2,PlayerSize[2]),glm::vec3(x+0.5,y+0.5,z));
                                if(tec<t or (tec+f<t and dist2<dist and side!=-1)){glm::vec3 SP=lastMin+v*tec;
                                    if(SP.x+f<x+1 and SP.x+PlayerSize[0]>x+f and SP.y+f<y+1 and SP.y+PlayerSize[1]>y+f){
                                        t=tec;material=mat;
                                        dist=dist2;
                                        side=0;//of block
                                        Block=glm::vec3(x,y,z);
                        }}}}
                        //printf("colision %i %i %i \n",x,y,z);
                    }
                }
            }
        }
        // 0, 1, 2, 3, 4, 5 z- x- z+ x+ y+ y-
        // z- x- z+ x+ y+ y-
        if(side!=-1){
            if(     side==4){
                setCamPos(glm::vec3(newCamPos.x,Block.y+1.0f+PlayerEyePos[1]        ,newCamPos.z));
                stopPlayerFall();
            }//v.y*t//hardset
            else if(side==5){setCamPos(glm::vec3(newCamPos.x,Block.y-PlayerSize[1]+PlayerEyePos[1],newCamPos.z));
                stopPlayerFall();

            }//v.y*t//hardset
            else if(side==3){setCamPos(glm::vec3(        Block.x+1.0f+PlayerEyePos[0],newCamPos.y,newCamPos.z));}
            else if(side==1){setCamPos(glm::vec3(Block.x-PlayerSize[0]+PlayerEyePos[0],newCamPos.y,newCamPos.z));}
            else if(side==2){setCamPos(glm::vec3(newCamPos.x,newCamPos.y,        Block.z+1.0f+PlayerEyePos[2]));}
            else if(side==0){setCamPos(glm::vec3(newCamPos.x,newCamPos.y,Block.z-PlayerSize[2]+PlayerEyePos[2]));}
//            printf("%f %f %f === %f %f %f ===%i %i %i %i\n",getCamPos().x,getCamPos().y,getCamPos().z,lastCamPos.x,
//                   lastCamPos.y,lastCamPos.z,side,getCamPos().x==lastCamPos.x,getCamPos().y==lastCamPos.y,getCamPos().z==lastCamPos.z);
//            printf("%f %f %f === %f %f %f\n",Block.y-PlayerSize[1]+PlayerEyePos[1],Block.x-PlayerSize[0]+PlayerEyePos[0],Block.z-PlayerSize[2]+PlayerEyePos[2],
//                    Block.y+1.0f+PlayerEyePos[1],Block.x+1.0f+PlayerEyePos[0],Block.z+1.0f+PlayerEyePos[2]);
        }
    }
//    if(material!=-1){
//        printf("\n");
//    }
    if(cancel==9){
        printf("colision float bug\n");
    }

    //START

//    int x=0;jhv
//    int y=1.7;
//    int z=0;
//    x+=chunk_sizes[0]/2.0*16;
//    y+=chunk_sizes[1]/2.0*16;
//    z+=chunk_sizes[2]/2.0*16;

    //    chunks[(x)/16][(y)/16][(z)/16]->change_block(x%16,y%16,z%16,2);
}

void gl_main::selectBlock()
{
    //catchbox
    float range=7;
    glm::vec3 camPositition=getCamPos();
    glm::vec3 forward=getForward();

    float t=1;
    glm::vec3 Block=glm::vec3(0,0,0);

    glm::vec3 BLICKCENTER = camPositition;
    glm::vec3 BLICKZIEL = camPositition+forward*range;
    glm::vec3 v=BLICKZIEL-BLICKCENTER;

    glm::vec3 absolMin=glm::vec3(
                std::min(BLICKCENTER.x,BLICKZIEL.x),
                std::min(BLICKCENTER.y,BLICKZIEL.y),
                std::min(BLICKCENTER.z,BLICKZIEL.z));
    glm::vec3 absolMax=glm::vec3(
                std::max(BLICKCENTER.x,BLICKZIEL.x),
                std::max(BLICKCENTER.y,BLICKZIEL.y),
                std::max(BLICKCENTER.z,BLICKZIEL.z));

    int side=-1;
    int mat;
    float tec=0;

    float f=+0.00001f;

        // 0, 1, 2, 3, 4, 5 z- x- z+ x+ y+ y-
        // z- x- z+ x+ y+ y-

        for(int x=int(std::floor(absolMin.x));x<=std::floor(absolMax.x);x++){
            for(int y=int(std::floor(absolMin.y));y<=std::floor(absolMax.y);y++){
                for(int z=int(std::floor(absolMin.z));z<=std::floor(absolMax.z);z++){
                    mat=getBlockat(x,y,z);
                    if(mat!=-1){
                        tec =((y+1.0f)-BLICKCENTER.y)/(v.y);
                        if(tec<t and tec>0){glm::vec3 SP=BLICKCENTER+v*tec;
                        if(SP.x+f<x+1.0f and SP.x>x+f and SP.z+f<z+1.0f and SP.z>z+f){
                             t=tec;
                             side=4;//of block
                             Block=glm::vec3(x,y,z);
                        }}
                                tec =((x+1.0f)-BLICKCENTER.x)/(v.x);
                                if(tec<t and tec>0){glm::vec3 SP=BLICKCENTER+v*tec;
                                    if(SP.y+f<y+1.0f and SP.y>y+f and SP.z+f<z+1.0f and SP.z>z+f){
                                        t=tec;
                                        side=3;//of block
                                        Block=glm::vec3(x,y,z);
                        }}
                                tec =((z+1.0f)-BLICKCENTER.z)/(v.z);
                                if(tec<t and tec>0){glm::vec3 SP=BLICKCENTER+v*tec;
                                    if(SP.x+f<x+1.0f and SP.x>x+f and SP.y+f<y+1.0f and SP.y>y+f){
                                        t=tec;
                                        side=2;//of block
                                        Block=glm::vec3(x,y,z);
                        }}
                                tec =((y)-BLICKCENTER.y)/(v.y);
                                if(tec<t and tec>0){glm::vec3 SP=BLICKCENTER+v*tec;
                                    if(SP.x+f<x+1.0f and SP.x>x+f and SP.z+f<z+1.0f and SP.z>z+f){
                                        t=tec;
                                        side=5;//of block
                                        Block=glm::vec3(x,y,z);
                        }}
                                tec =((x)-BLICKCENTER.x)/(v.x);
                                if(tec<t and tec>0){glm::vec3 SP=BLICKCENTER+v*tec;
                                    if(SP.y+f<y+1.0f and SP.y>y+f and SP.z+f<z+1.0f and SP.z>z+f){
                                        t=tec;
                                        side=1;//of block
                                        Block=glm::vec3(x,y,z);
                        }}
                                tec =((z)-BLICKCENTER.z)/(v.z);
                                if(tec<t and tec>0){glm::vec3 SP=BLICKCENTER+v*tec;
                                    if(SP.x+f<x+1.0f and SP.x>x+f and SP.y+f<y+1.0f and SP.y>y+f){
                                        t=tec;
                                        side=0;//of block
                                        Block=glm::vec3(x,y,z);
                        }}
                        //printf("colision %i %i %i \n",x,y,z);
                    }
                }
            }
        }
        // 0, 1, 2, 3, 4, 5 z- x- z+ x+ y+ y-
        // z- x- z+ x+ y+ y-
    //printf("%i   dirvec:%f %f %f=== %f %f %f ===%f %f %f \n",side,forward.x,forward.y,forward.z,absolMin.x,absolMax.x,absolMin.y,absolMax.y,absolMin.z,absolMax.z);

    if(side!=-1){
        //printf("here\n");
        //markBlock();
        int action=getBlockInteraction();
        if(action==2){//remove
            setBlockat(int(std::floor(Block.x)),int(std::floor(Block.y)),int(std::floor(Block.z)),-1);
        }
        else if(action==1){
            if(     side==4){Block.y+=1;}
            else if(side==5){Block.y-=1;}
            else if(side==3){Block.x+=1;}
            else if(side==1){Block.x-=1;}
            else if(side==2){Block.z+=1;}
            else if(side==0){Block.z-=1;}
            setBlockat(int(std::floor(Block.x)),int(std::floor(Block.y)),int(std::floor(Block.z)),1);
        }
    }
}

void gl_main::paint()
{


    size_t shapeID      = 100000;
    chunk* shape_ptr;
    std::vector<GLuint> vertexbuffers;
    std::vector<GLuint> elementbuffers;
    size_t buffer_count;
    /*
    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0,0,1024,1024); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    // We don't use bias in the shader, but instead we draw back faces,
    // which are already separated from the front faces by a small distance
    // (if your geometry is made this way)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(depthProgramID);

    glm::vec3 lightInvDir = glm::vec3(-11.5f,22.0f,8.5f);

    // Compute the MVP matrix from the light's point of view
    glm::mat4 depthProjectionMatrix = glm::ortho(-16*8.0f,16*8.0f,-16*8.0f,16*8.0f,0.0f,16*16.0f);
    //glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
    glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0,0,0), glm::vec3(0,1,0));
    // or, for spot light :
    //glm::vec3 lightPos(5, 20, 20);
    //glm::mat4 depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
    //glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos-lightInvDir, glm::vec3(0,1,0));

    glm::mat4 depthModelMatrix = glm::mat4(1.0);
    glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);


    for(size_t i=0;i<objects.size();i++){  //!!! watch out , i++ down there
       if(shapeID !=objects[i]->get_shapeID()){
                shapeID=objects[i]->get_shapeID();
                glBindBuffer(GL_ARRAY_BUFFER, shapes[shapeID]->get_vertexbuffer());
                glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapes[shapeID]->get_elementbuffer());
        }
        glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &objects[i]->get_ModelMatrix()[0][0]);
        glDrawElements(GL_TRIANGLES,shapes[shapeID]->get_index_size(),GL_UNSIGNED_SHORT,0);//(void*)0);
    }

    for(int x=0;x<chunk_sizes[0];x++){
        for(int y=0;y<chunk_sizes[1];y++){
            for(int z=0;z<chunk_sizes[2];z++){
                shape_ptr=chunks[x][y][z]->get_chunk_shape();
                vertexbuffers=shape_ptr->get_vertexbuffers();
                if(vertexbuffers.size()>0){
                    elementbuffers=shape_ptr->get_elementbuffers();
                    buffer_count=vertexbuffers.size();
                    glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &chunks[x][y][z]->get_ModelMatrix()[0][0]);
                    for(size_t buffer_number=0;buffer_number<buffer_count;buffer_number++){
                        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[buffer_number]);
                        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffers[buffer_number]);
                        glDrawElements(GL_TRIANGLES,shape_ptr->get_index_size(buffer_number),GL_UNSIGNED_SHORT,0);//(void*)0);
                    }
                }
            }
        }
    }
    */

    //////////////////////////////////////////////////////////////////////////
    /// \brief glViewport
    //////////////////////////////////////////////////////////////////////////
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,windowWidth,windowHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(ShadowMapID, 1);

    glm::mat4 ProjectionMatrix = getProjectionMatrix(); //evt. schon vorher multipiziren, nicht erst in shadern...
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::mat4 ModelMatrix;
    glm::vec3 lightPos  = glm::vec3(objects[skycubeID]->get_ModelMatrix() * glm::vec4(-11.5f,22.0f,8.5f,1));

    glm::mat4 biasMatrix(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );

    glm::mat4 depthBiasMVP; // = biasMatrix*depthMVP;
    ///////////
    /// \brief _Shadow - bug
    //////////
    glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);

    size_t TextureID    = 100000;
    shapeID      = 100000;
    size_t prog         = 100000; //WARNING if first obj uses shader 100000-> Bug !!!
    int    objID        = 100000;

    size_t objects_size=objects.size();

    for(size_t i=0;i<objects_size;i++){  //!!! watch out , i++ down there
        if(objID !=objects[i]->get_objID()){
            objID =objects[i]->get_objID();
            if(prog != objects[i]->get_programmID()){
                prog=objects[i]->get_programmID();
                glUseProgram(programIDs[prog]);
                glUniform3f(LightIDs[prog], lightPos.x, lightPos.y, lightPos.z);
                glUniformMatrix4fv(MatrixIDs[prog], 1, GL_FALSE, &ProjectionMatrix[0][0]);
                glUniformMatrix4fv(ViewMatrixIDs[prog], 1, GL_FALSE, &ViewMatrix[0][0]);
                glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);
                //glActiveTexture(GL_TEXTURE0);

                TextureID= 100000; //WARNING if first obj uses shader 100000-> Bug !!!
                shapeID  = 100000;
            }

            if(TextureID != objects[i]->get_TextureID()){
                TextureID=objects[i]->get_TextureID();

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, Textures[TextureID]);
                glUniform1i(TextureSamplerIDs[prog], 0);
            }

            if(shapeID !=objects[i]->get_shapeID()){
                shapeID=objects[i]->get_shapeID();
                glBindBuffer(GL_ARRAY_BUFFER, shapes[shapeID]->get_vertexbuffer());
                glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
                glBindBuffer(GL_ARRAY_BUFFER, shapes[shapeID]->get_uvbuffer());
                glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,0);
                glBindBuffer(GL_ARRAY_BUFFER, shapes[shapeID]->get_normalbuffer());
                glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapes[shapeID]->get_elementbuffer());
                //glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,0);
            }
        }

        glUniformMatrix4fv(ModelMatrixIDs[prog], 1, GL_FALSE, &objects[i]->get_ModelMatrix()[0][0]);
        glDrawElements(GL_TRIANGLES,shapes[shapeID]->get_index_size(),GL_UNSIGNED_SHORT,0);//(void*)0);
            // mode // count// type // element array buffer offset

    }

    //draw chunks
    //chunk* shape_ptr;

    //std::vector<GLuint> vertexbuffers;
    std::vector<GLuint> uvbuffers;
    std::vector<GLuint> normalbuffers;
    //std::vector<GLuint> elementbuffers;

    //size_t buffer_count;

    for(int x=0;x<chunk_sizes[0];x++){
        for(int y=0;y<chunk_sizes[1];y++){
            for(int z=0;z<chunk_sizes[2];z++){

                if(prog != chunks[x][y][z]->get_programmID()){
                    prog=chunks[x][y][z]->get_programmID();
                    glUseProgram(programIDs[prog]);
                    glUniform3f(LightIDs[prog], lightPos.x, lightPos.y, lightPos.z);
                    glUniformMatrix4fv(MatrixIDs[prog], 1, GL_FALSE, &ProjectionMatrix[0][0]);
                    glUniformMatrix4fv(ViewMatrixIDs[prog], 1, GL_FALSE, &ViewMatrix[0][0]);
                    glActiveTexture(GL_TEXTURE0);

                    TextureID= 100000; //WARNING if first obj uses shader 100000-> Bug !!!
                    shapeID  = 100000;
                }

                if(TextureID != chunks[x][y][z]->get_TextureID()){
                    TextureID = chunks[x][y][z]->get_TextureID();
                    //glActiveTexture(GL_TEXTURE0);
                    //glBindTexture(GL_TEXTURE_2D, Textures[3]);
                    //glActiveTexture(GL_TEXTURE1);
                    //glBindTexture(GL_TEXTURE_2D, Textures[4]);
                    //glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D_ARRAY, Textures[5]);
                    glUniform1i(TextureArraySampler, 0);
                }

                shape_ptr=chunks[x][y][z]->get_chunk_shape();

                vertexbuffers=shape_ptr->get_vertexbuffers();
                if(vertexbuffers.size()>0){
                    uvbuffers=shape_ptr->get_uvbuffers();
                    normalbuffers=shape_ptr->get_normalbuffers();
                    elementbuffers=shape_ptr->get_elementbuffers();

                    buffer_count=vertexbuffers.size();
                    glUniformMatrix4fv(ModelMatrixIDs[prog], 1, GL_FALSE, &chunks[x][y][z]->get_ModelMatrix()[0][0]);

                    for(size_t buffer_number=0;buffer_number<buffer_count;buffer_number++){
                        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffers[buffer_number]);
                        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
                        glBindBuffer(GL_ARRAY_BUFFER, uvbuffers[buffer_number]);
                        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
                        glBindBuffer(GL_ARRAY_BUFFER, normalbuffers[buffer_number]);
                        glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,0);
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffers[buffer_number]);


                        glDrawElements(GL_TRIANGLES,shape_ptr->get_index_size(buffer_number),GL_UNSIGNED_SHORT,0);//(void*)0);
                            // mode // count// type // element array buffer offset
                    }
                }
            }
        }
    }

    int w2=windowWidth/2;
    int h2=windowHeight/2;
    int width=4;
    int length=20;
    glViewport(w2-4,h2-4,8,8);
    glUseProgram(CrossShader);
    glBindBuffer(GL_ARRAY_BUFFER, Screenvertices);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    glViewport(w2-length/2,h2-width/2,length,width);
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
    glViewport(w2-width/2,h2-length/2,width,length);
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles




    // Swap buffers
    glfwPollEvents();
    glfwSwapBuffers(window);
}

void gl_main::mainloop()
{
    lastTime = glfwGetTime();
    nbFrames = 0;
    lastCamPos = getCamPos();

    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    do{
        measure_speed();
        actions();
        paint();
    }
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    clean();
}

void gl_main::printDeltaTime(const char *spez)
{
    static int max_prints=2000;

    if(max_prints>0){
        double currentTime = glfwGetTime();
        printf("%s dTime = %f\n",spez, currentTime - lastDeltaTime);
        lastDeltaTime=currentTime;
        max_prints-=1;
    }

}

void gl_main::measure_speed()
{
    // Measure speed
    double currentTime = glfwGetTime();
    nbFrames++;
    if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
        // printf and reset
        //printf("%f ms/frame\n", 1000.0/double(nbFrames));
        printf("FPS %i = %f ms/frame\n", nbFrames, 1000.0/double(nbFrames));
        printf("%f %f %f\n",getCamPos().x,getCamPos().y,getCamPos().z);
        nbFrames = 0;
        lastTime += 1.0;
    }
}

void gl_main::clean()
{
    // Cleanup VBO and shader

    glDeleteVertexArrays(1, &VertexArrayID);

    for(size_t i=0;i<programIDs.size();i++){
        glDeleteProgram(programIDs[i]);
    }
    for(size_t i=0;i<Textures.size();i++){
        glDeleteTextures(1, &Textures[i]);
    }

    for(size_t i=0;i<objects.size();i++){
        delete(objects[i]);
    }
    for(int x=0;x<chunk_sizes[0];x++){
        for(int y=0;y<chunk_sizes[1];y++){
            for(int z=0;z<chunk_sizes[2];z++){
                delete(chunks[x][y][z]);
            }
        }
    }
    /*
    //destruktor wegen new???
    int b=chunk_sizes[0];
    int h=chunk_sizes[1];
    int l=chunk_sizes[2];
    int bufferCount=b*h*l* 2 ; //vergrossern um mehr platz zu haben, lastet VRAM stark aus (max statt 2 -> 8)
    //! doesnot  risize atomaticly
    glDeleteBuffers(bufferCount, &chunk_vertex_buffer_stack[0]);
    glDeleteBuffers(bufferCount, &chunk_uv_buffer_stack[0]);
    glDeleteBuffers(bufferCount, &chunk_normal_buffer_stack[0]);
    glDeleteBuffers(bufferCount, &chunk_element_buffer_stack[0]);
    */

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}

void gl_main::move_chunks()
{
    /*
     * prufen ob aus chunk herausbewegt mit map_pos und getCamPos() (übergang bei 8/-8)
     * zB verschiebung der ersten reihe nach hinten, notiz in map_border
     * neuverlinkung der chunks
     * später: neuladen chunkinhalt
     * neulader der vorherigen und neuen border chunks
    */
    /*
    printf("%f %f %f\n",getCamPos().x,getCamPos().y,getCamPos().z);
    int chunk_sizes[3]={18,18,18};
    int map_border[3]={0,0,0};
    int map_pos[3]={0,0,0};
    chunk_obj* chunks[18][18][18];
    */

    float dx=chunk_sizes[0]/2.0;
    float dy=chunk_sizes[1]/2.0;
    float dz=chunk_sizes[2]/2.0;

    bool relode=true;

    if(getCamPos().x-map_pos[0]>8*4){//wenn in pos. xRichtung bewegt
        //int x=map_border[0];
        for(int y=0; y< chunk_sizes[1]; y++){
            for(int z=0; z< chunk_sizes[2]; z++){
                //bewege chunks auf andere seite
                chunks[map_border[0]][(y+map_border[1])%chunk_sizes[1]][(z+map_border[2])%chunk_sizes[2]]->replace_to(
                            glm::vec3(
                                (chunk_sizes[0]-dx)*16+map_pos[0],
                                (y-dy)*16+map_pos[1],
                                (z-dz)*16+map_pos[2]
                            )
                        );
                //aufheben verlinkung
                chunks[map_border[0]][y][z]->del_neighbor(3);
                chunks[(map_border[0]+1)%chunk_sizes[0]][y][z]->del_neighbor(1);
                //neu verlinkung
                int x_1 = (map_border[0]+chunk_sizes[0]-1)%chunk_sizes[0];
                chunks[map_border[0]][y][z]->set_neighbor(chunks[x_1][y][z],1);
                chunks[x_1][y][z]->set_neighbor(chunks[map_border[0]][y][z],3);
                //blocke neu laden
                //...
                //neuladen
                //FIX:PERFORMANCE: nur seiten neu laden
                if(relode){
                    chunks[ map_border[0]                  ][y][z]->refresh_chunk();
                    chunks[(map_border[0]+1)%chunk_sizes[0]][y][z]->refresh_chunk();
                    chunks[x_1][y][z]->refresh_chunk();
                }
                //ist in dieser Reihenfloge nur möglich,
                //da das neuladen nur abhangig von der verlinkung ist, nicht von der pos
            }
        }
        //printf("FIX3\n");
        map_border[0]=(map_border[0]+1)%chunk_sizes[0];
        map_pos[0]+=16;
    }
    else if(getCamPos().x-map_pos[0]<-8*4){//wenn in neg. xRichtung bewegt
        //int x=map_border[0];
        for(int y=0; y< chunk_sizes[1]; y++){
            for(int z=0; z< chunk_sizes[2]; z++){
                //bewege chunks auf andere seite
                int x=(map_border[0]+chunk_sizes[0]-1)%chunk_sizes[0];
                chunks[x][(y+map_border[1])%chunk_sizes[1]][(z+map_border[2])%chunk_sizes[2]]->replace_to(
                            glm::vec3(
                                (-1-dx)*16+map_pos[0],
                                (y-dy)*16+map_pos[1],
                                (z-dz)*16+map_pos[2]
                            )
                        );
                //aufheben verlinkung
                chunks[x][y][z]->del_neighbor(1);
                int x2=(map_border[0]+chunk_sizes[0]-2)%chunk_sizes[0];
                chunks[x2][y][z]->del_neighbor(3);
                //neu verlinkung
                int x_1 = (map_border[0]+1)%chunk_sizes[0];
                chunks[x][y][z]->set_neighbor(chunks[x_1][y][z],3);
                chunks[x_1][y][z]->set_neighbor(chunks[x][y][z],1);
                //blocke neu laden
                //...
                //neuladen
                //FIX:PERFORMANCE: nur seiten neu laden
                if(relode){
                    chunks[  x][y][z]->refresh_chunk();
                    chunks[ x2][y][z]->refresh_chunk();
                    chunks[x_1][y][z]->refresh_chunk();
                }
                //ist in dieser Reihenfloge nur möglich,
                //da das neuladen nur abhangig von der verlinkung ist, nicht von der pos
            }
        }
        //printf("FIX3\n");
        map_border[0]=(map_border[0]-1+chunk_sizes[0])%chunk_sizes[0];
        map_pos[0]-=16;
    }

    //#y

    if(getCamPos().y-map_pos[1]>8*4){//wenn in pos. yRichtung bewegt
        for(int x=0; x< chunk_sizes[0]; x++){
            for(int z=0; z< chunk_sizes[2]; z++){
                //bewege chunks auf andere seite
                int y=map_border[1];

                chunks[(x+map_border[0])%chunk_sizes[0]][y][(z+map_border[2])%chunk_sizes[2]]->replace_to(
                            glm::vec3(
                                (             x-dx)*16+map_pos[0],
                                (chunk_sizes[1]-dy)*16+map_pos[1],
                                (             z-dz)*16+map_pos[2]
                            )
                        );
                //aufheben verlinkung
                int y2=(y+1)%chunk_sizes[1];
                chunks[x][ y][z]->del_neighbor(4);
                chunks[x][y2][z]->del_neighbor(5);
                //neu verlinkung
                int y_1 = (map_border[1]+chunk_sizes[1]-1)%chunk_sizes[1];
                chunks[x][  y][z]->set_neighbor(chunks[x][y_1][z],5);
                chunks[x][y_1][z]->set_neighbor(chunks[x][  y][z],4);
                //blocke neu laden
                //...
                //neuladen
                //FIX:PERFORMANCE: nur seiten neu laden
                if(relode){
                    chunks[x][  y][z]->refresh_chunk();
                    chunks[x][ y2][z]->refresh_chunk();
                    chunks[x][y_1][z]->refresh_chunk();
                }
                //ist in dieser Reihenfloge nur möglich,
                //da das neuladen nur abhangig von der verlinkung ist, nicht von der pos
            }
        }
        //printf("FIX3\n");
        map_border[1]=(map_border[1]+1)%chunk_sizes[1];
        map_pos[1]+=16;
    }
    else if(getCamPos().y-map_pos[1]<-8*4){//wenn in neg. yRichtung bewegt
        //int x=map_border[0];
        for(int x=0; x< chunk_sizes[0]; x++){
            for(int z=0; z< chunk_sizes[2]; z++){
                //bewege chunks auf andere seite
                int y=(map_border[1]+chunk_sizes[1]-1)%chunk_sizes[1];
                chunks[(x+map_border[0])%chunk_sizes[0]][y][(z+map_border[2])%chunk_sizes[2]]->replace_to(
                            glm::vec3(
                                (x-dx)*16+map_pos[0],
                                (-1-dy)*16+map_pos[1],
                                (z-dz)*16+map_pos[2]
                            )
                        );
                //aufheben verlinkung
                int y2=(map_border[1]+chunk_sizes[1]-2)%chunk_sizes[1];
                chunks[x][ y][z]->del_neighbor(5);
                chunks[x][y2][z]->del_neighbor(4);
                //neu verlinkung
                int y_1 = (map_border[1]+1)%chunk_sizes[1];
                chunks[x][  y][z]->set_neighbor(chunks[x][y_1][z],4);
                chunks[x][y_1][z]->set_neighbor(chunks[x][  y][z],5);
                //blocke neu laden
                //...
                //neuladen
                //FIX:PERFORMANCE: nur seiten neu laden
                if(relode){
                    chunks[x][  y][z]->refresh_chunk();
                    chunks[x][ y2][z]->refresh_chunk();
                    chunks[x][y_1][z]->refresh_chunk();
                }
                //ist in dieser Reihenfloge nur möglich,
                //da das neuladen nur abhangig von der verlinkung ist, nicht von der pos
            }
        }
        //printf("FIX3\n");
        map_border[1]=(map_border[1]-1+chunk_sizes[1])%chunk_sizes[1];
        map_pos[1]-=16;
    }

    //#z

    if(getCamPos().z-map_pos[2]>8*4){//wenn in pos. zRichtung bewegt
        for(int x=0; x< chunk_sizes[0]; x++){
            for(int y=0; y< chunk_sizes[1]; y++){
                //bewege chunks auf andere seite
                int z=map_border[2];

                chunks[(x+map_border[0])%chunk_sizes[0]][(y+map_border[1])%chunk_sizes[1]][z]->replace_to(
                            glm::vec3(
                                (             x-dx)*16+map_pos[0],
                                (             y-dy)*16+map_pos[1],
                                (chunk_sizes[2]-dz)*16+map_pos[2]
                            )
                        );
                //aufheben verlinkung
                int z2=(z+1)%chunk_sizes[2];
                chunks[x][y][z]->del_neighbor(2);
                chunks[x][y][z2]->del_neighbor(0);
                //neu verlinkung
                int z_1 = (map_border[2]+chunk_sizes[2]-1)%chunk_sizes[2];
                chunks[x][y][  z]->set_neighbor(chunks[x][y][z_1],0);
                chunks[x][y][z_1]->set_neighbor(chunks[x][y][  z],2);
                //blocke neu laden
                //...
                //neuladen
                //FIX:PERFORMANCE: nur seiten neu laden
                if(relode){
                    chunks[x][y][  z]->refresh_chunk();
                    chunks[x][y][ z2]->refresh_chunk();
                    chunks[x][y][z_1]->refresh_chunk();
                }
                //ist in dieser Reihenfloge nur möglich,
                //da das neuladen nur abhangig von der verlinkung ist, nicht von der pos
            }
        }
        //printf("FIX3\n");
        map_border[2]=(map_border[2]+1)%chunk_sizes[2];
        map_pos[2]+=16;
    }
    else if(getCamPos().z-map_pos[2]<-8*4){//wenn in neg. zRichtung bewegt
        for(int x=0; x< chunk_sizes[0]; x++){
            for(int y=0; y< chunk_sizes[1]; y++){
                //bewege chunks auf andere seite
                int z=(map_border[2]+chunk_sizes[2]-1)%chunk_sizes[2];
                chunks[(x+map_border[0])%chunk_sizes[0]][(y+map_border[1])%chunk_sizes[1]][z]->replace_to(
                            glm::vec3(
                                (x-dx)*16+map_pos[0],
                                (y-dy)*16+map_pos[1],
                                (-1-dz)*16+map_pos[2]
                            )
                        );
                //aufheben verlinkung
                int z2=(map_border[2]+chunk_sizes[2]-2)%chunk_sizes[2];
                chunks[x][y][z]->del_neighbor(0);
                chunks[x][y][z2]->del_neighbor(2);
                //neu verlinkung
                int z_1 = (map_border[2]+1)%chunk_sizes[2];
                chunks[x][y][  z]->set_neighbor(chunks[x][y][z_1],2);
                chunks[x][y][z_1]->set_neighbor(chunks[x][y][  z],0);
                //blocke neu laden
                //...
                //neuladen
                //FIX:PERFORMANCE: nur seiten neu laden
                if(relode){
                    chunks[x][y][  z]->refresh_chunk();
                    chunks[x][y][ z2]->refresh_chunk();
                    chunks[x][y][z_1]->refresh_chunk();
                }
                //ist in dieser Reihenfloge nur möglich,
                //da das neuladen nur abhangig von der verlinkung ist, nicht von der pos
            }
        }
        //printf("FIX3\n");
        map_border[2]=(map_border[2]-1+chunk_sizes[2])%chunk_sizes[2];
        map_pos[2]-=16;
    }
    /*
    if(x>0){
        chunks[x][y][z]->set_neighbor(chunks[x-1][y][z],1);
        chunks[x-1][y][z]->set_neighbor(chunks[x][y][z],3);
    }
    if(y>0){
        chunks[x][y][z]->set_neighbor(chunks[x][y-1][z],5);
        chunks[x][y-1][z]->set_neighbor(chunks[x][y][z],4);
    }
    if(z>0){
        chunks[x][y][z]->set_neighbor(chunks[x][y][z-1],0);
        chunks[x][y][z-1]->set_neighbor(chunks[x][y][z],2);
    }


                float dx=chunk_sizes[0]/2-0.5f;
                float dy=chunk_sizes[1]/2-0.5f;
                float dz=chunk_sizes[2]/2-0.5f;

    glm::vec3((x-dx)*16,(y-dy)*16,(z-dz)*16), //position
    */

}

short gl_main::getBlockat(int x, int y, int z)
{
    x+=chunk_sizes[0]/2.0*16-map_pos[0];
    y+=chunk_sizes[1]/2.0*16-map_pos[1];
    z+=chunk_sizes[2]/2.0*16-map_pos[2];
    return chunks
            [(x/16+map_border[0])%chunk_sizes[0]]
            [(y/16+map_border[1])%chunk_sizes[1]]
            [(z/16+map_border[2])%chunk_sizes[2]]
            ->get_block(x%16,y%16,z%16);
}

void gl_main::setBlockat(int x, int y, int z, short mat)
{
    x+=chunk_sizes[0]/2.0*16-map_pos[0];
    y+=chunk_sizes[1]/2.0*16-map_pos[1];
    z+=chunk_sizes[2]/2.0*16-map_pos[2];
    return chunks
            [(x/16+map_border[0])%chunk_sizes[0]]
            [(y/16+map_border[1])%chunk_sizes[1]]
            [(z/16+map_border[2])%chunk_sizes[2]]
            ->change_block(x%16,y%16,z%16,mat);
}

/*
GLuint4back gl_main::get_chunk_buffer()
{

    if(chunk_vertex_buffer_stack.size() >1){
        _GLuint4back bufferz;
        bufferz.i0=chunk_vertex_buffer_stack.back();
        bufferz.i1=chunk_uv_buffer_stack.back();
        bufferz.i2=chunk_normal_buffer_stack.back();
        bufferz.i3=chunk_element_buffer_stack.back();

        chunk_vertex_buffer_stack.pop_back();
        chunk_uv_buffer_stack.pop_back();
        chunk_normal_buffer_stack.pop_back();
        chunk_element_buffer_stack.pop_back();

        return bufferz;
    }
    else{
        printf("ERROR: chunk-buffers empty");
        return {0,0,0,0};
    }

}

void gl_main::return_chunk_buffer(GLuint4back bufferz)
{
    chunk_vertex_buffer_stack.push_back(bufferz.i0);
    chunk_uv_buffer_stack.push_back(bufferz.i1);
    chunk_normal_buffer_stack.push_back(bufferz.i2);
    chunk_element_buffer_stack.push_back(bufferz.i3);
}
*/





















