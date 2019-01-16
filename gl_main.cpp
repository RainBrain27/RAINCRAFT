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
    window = glfwCreateWindow( 1024, 768, "RAINCRAFT", NULL, NULL);
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

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

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

    float d=b/2.0;
    float dh=h/2.0;
    float dl=l/2.0;

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
                                               cube_indexed_normals
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

//    int x=0;
//    int y=1.7;
//    int z=0;
//    x+=chunk_sizes[0]/2.0*16;
//    y+=chunk_sizes[1]/2.0*16;
//    z+=chunk_sizes[2]/2.0*16;

//    chunks[(x)/16][(y)/16][(z)/16]->change_block(x%16,y%16,z%16,2);


}

void gl_main::actions()
{
    computeMatricesFromInputs();
    //ausrichten skycube
    float scale= objects[skycubeID]->get_scale();
    objects[skycubeID]->move(glm::vec3(
                                 (getCamPos().x-lastCamPos.x)/scale,
                                 (getCamPos().y-lastCamPos.y)/scale,
                                 (getCamPos().z-lastCamPos.z)/scale));
    move_chunks();

    playercolision();

    lastCamPos=getCamPos();
}

void gl_main::playercolision()
{
    glm::vec3 newCamPos = getCamPos();
    //lastCamPos
    glm::vec3 newMin=newCamPos-PlayerEyePos;
    glm::vec3 newMax=newMin+PlayerSize;
    glm::vec3 lastMin=lastCamPos-PlayerEyePos;
    glm::vec3 lastMax=lastMin+PlayerSize;
    glm::vec3 absolMin=glm::vec3(
                std::min(newMin.x,lastMin.x),
                std::min(newMin.y,lastMin.y),
                std::min(newMin.z,lastMin.z));
    glm::vec3 absolMax=glm::vec3(
                std::max(newMax.x,lastMax.x),
                std::max(newMax.y,lastMax.y),
                std::max(newMax.z,lastMax.z));
    glm::vec3 chunkPos=glm::vec3(chunk_sizes[0]/2.0*16,chunk_sizes[1]/2.0*16,chunk_sizes[2]/2.0*16);
    absolMax+=chunkPos;
    absolMin+=chunkPos;

    for(int x=absolMin.x;x<=absolMax.x;x++){
        for(int y=absolMin.y;y<=absolMax.y;y++){
            for(int z=absolMin.z;z<=absolMax.z;z++){
                if(chunks[x/16][y/16][z/16]->get_block(x%16,y%16,z%16)!=-1){
                    printf("colision\n");
                }
            }
        }
    }

//    int x=0;
//    int y=1.7;
//    int z=0;
//    x+=chunk_sizes[0]/2.0*16;
//    y+=chunk_sizes[1]/2.0*16;
//    z+=chunk_sizes[2]/2.0*16;

//    chunks[(x)/16][(y)/16][(z)/16]->change_block(x%16,y%16,z%16,2);
}

void gl_main::paint()
{

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    actions();

    glm::mat4 ProjectionMatrix = getProjectionMatrix(); //evt. schon vorher multipiziren, nicht erst in shadern...
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::mat4 ModelMatrix;
    glm::vec3 lightPos  = glm::vec3(objects[skycubeID]->get_ModelMatrix() * glm::vec4(-11.5f,22.0f,8.5f,1));

    size_t TextureID    = 100000;
    size_t shapeID      = 100000;
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
                glActiveTexture(GL_TEXTURE0);

                TextureID= 100000; //WARNING if first obj uses shader 100000-> Bug !!!
                shapeID  = 100000;
            }

            if(TextureID != objects[i]->get_TextureID()){
                TextureID=objects[i]->get_TextureID();
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
    chunk* shape_ptr;

    std::vector<GLuint> vertexbuffers;
    std::vector<GLuint> uvbuffers;
    std::vector<GLuint> normalbuffers;
    std::vector<GLuint> elementbuffers;

    size_t buffer_count;

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
        paint();
    }
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    clean();
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
    //destruktor wegen new???

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

    bool relode=false;

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

























