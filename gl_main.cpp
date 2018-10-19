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
    shapes.push_back(new chunk());


}

void gl_main::loadTextures()
{

    Textures.push_back(loadDDS("textures/uvmap.DDS",false));
    Textures.push_back(loadDDS("textures/rathalos1.DDS",false));
    Textures.push_back(loadDDS("textures/crafting_table.DDS",true));
    Textures.push_back(loadDDS("textures/skycube.DDS",true));

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


    objects.push_back(new baseobject(0,3,1,0,glm::vec3(0,0,0))); //skycube
    objects[0]->scale(256.0);
    skycubeID=0;

    //objects.push_back(new baseobject(0,0,0,0,glm::vec3(0,-10,0)));

    //objects.push_back(new baseobject(1,1,0,1,glm::vec3(0,10,0)));

    //objects.push_back(new baseobject(1,2,0,2,glm::vec3(0,0,0)));

    size_t b=18;
    size_t l=18;
    size_t h=3;

    float d=b/2-0.5f;
    float dh=h/2-0.5f +3;
    float dl=l/2-0.5f;

    for(size_t x=0;x<b;x++){
        for(size_t y=0;y<h;y++){
            for(size_t z=0;z<l;z++){
                chunks.push_back(new chunk_obj(2,  //texture
                                               0,  //program
                                               1,  //ObjID (not needed)
                                               glm::vec3((x-d)*16,(y-dh)*16,(z-dl)*16), //position
                                               cube_indices,
                                               cube_indexed_vertices,
                                               cube_indexed_uvs,
                                               cube_indexed_normals
                                               ));
                //printf("%i \n",x*b*b+y*b+z);
            }
        }
    }
    for(size_t x=0;x<b;x++){
        for(size_t y=0;y<h;y++){
            for(size_t z=0;z<l;z++){
                if(x>0){
                    chunks[    x*h*l + y*l + z]->set_neighbor(chunks[(x-1)*h*l + y*l + z],1);
                    chunks[(x-1)*h*l + y*l + z]->set_neighbor(chunks[    x*h*l + y*l + z],3);
                }
                if(y>0){
                    chunks[x*h*l +     y*l + z]->set_neighbor(chunks[x*h*l + (y-1)*l + z],5);
                    chunks[x*h*l + (y-1)*l + z]->set_neighbor(chunks[x*h*l + y*l   +   z],4);
                }
                if(z>0){
                    chunks[x*h*l +     y*l + z]->set_neighbor(chunks[x*h*l + y*l + (z-1)],0);
                    chunks[x*h*l + y*l + (z-1)]->set_neighbor(chunks[x*h*l + y*l +     z],2);
                }
            }
        }
    }

    for(size_t i=0;i<chunks.size();i++){
        chunks[i]->refresh_chunk();
    }


    for(size_t i=0;i<b*16;i++){
        size_t x=42,y=30;
        chunks[(x/16)*h*l + y/16*l + i/16]->change_block(x%16,y%16,i%16,-1);
        y=31;
        chunks[(x/16)*h*l + y/16*l + i/16]->change_block(x%16,y%16,i%16,-1);
        y=32;
        chunks[(x/16)*h*l + y/16*l + i/16]->change_block(x%16,y%16,i%16,-1);
        y=30;x=43;
        chunks[(x/16)*h*l + y/16*l + i/16]->change_block(x%16,y%16,i%16,-1);
        y=31;
        chunks[(x/16)*h*l + y/16*l + i/16]->change_block(x%16,y%16,i%16,-1);
        y=32;
        chunks[(x/16)*h*l + y/16*l + i/16]->change_block(x%16,y%16,i%16,-1);

    }


    /*
    printf("\nstart\n");
    chunks[0]->change_block(1,1,1,-1);
    printf("\nstart\n");
    chunks[0]->change_block(1,1,1,1);

    */



}

void gl_main::paint()
{

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    computeMatricesFromInputs();

    glm::mat4 ProjectionMatrix = getProjectionMatrix(); //evt. schon vorher multipiziren, nicht erst in shadern...
    glm::mat4 ViewMatrix = getViewMatrix();

    //ausrichten skycube
    float scale= objects[skycubeID]->get_scale();
    objects[skycubeID]->move(glm::vec3((getCamPos().x-lastCamPos.x)/scale,(getCamPos().y-lastCamPos.y)/scale,(getCamPos().z-lastCamPos.z)/scale));

    lastCamPos=getCamPos();

    glm::mat4 ModelMatrix;
    glm::vec3 lightPos  = glm::vec3(objects[skycubeID]->get_ModelMatrix() * glm::vec4(-11.5f,22.0f,8.5f,1));

    size_t TextureID    = 100000;
    size_t shapeID      = 100000;
    size_t prog         = 100000; //WARNING if first obj uses shader 100000-> Bug !!!
    int    objID        = 100000;

    size_t i=0;
    size_t objects_size=objects.size();

    while(i<objects_size){  //!!! watch out , i++ down there
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
            }
        }

        glUniformMatrix4fv(ModelMatrixIDs[prog], 1, GL_FALSE, &objects[i]->get_ModelMatrix()[0][0]);
        glDrawElements(GL_TRIANGLES,shapes[shapeID]->get_index_size(),GL_UNSIGNED_SHORT,0);//(void*)0);
            // mode // count// type // element array buffer offset
        i++;

    }

    //draw chunks

    i=0;
    objects_size=chunks.size();
    chunk* shape_ptr;

    while(i<objects_size){  //!!! watch out , i++ down there
        if(prog != chunks[i]->get_programmID()){
            prog=chunks[i]->get_programmID();
            glUseProgram(programIDs[prog]);
            glUniform3f(LightIDs[prog], lightPos.x, lightPos.y, lightPos.z);
            glUniformMatrix4fv(MatrixIDs[prog], 1, GL_FALSE, &ProjectionMatrix[0][0]);
            glUniformMatrix4fv(ViewMatrixIDs[prog], 1, GL_FALSE, &ViewMatrix[0][0]);
            glActiveTexture(GL_TEXTURE0);

            TextureID= 100000; //WARNING if first obj uses shader 100000-> Bug !!!
            shapeID  = 100000;
        }

        if(TextureID != chunks[i]->get_TextureID()){
            TextureID = chunks[i]->get_TextureID();
            glBindTexture(GL_TEXTURE_2D, Textures[TextureID]);
            glUniform1i(TextureSamplerIDs[prog], 0);
        }

        shape_ptr=chunks[i]->get_chunk_shape();

        glBindBuffer(GL_ARRAY_BUFFER, shape_ptr->get_vertexbuffer());
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
        glBindBuffer(GL_ARRAY_BUFFER, shape_ptr->get_uvbuffer());
        glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,0);
        glBindBuffer(GL_ARRAY_BUFFER, shape_ptr->get_normalbuffer());
        glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape_ptr->get_elementbuffer());

        glUniformMatrix4fv(ModelMatrixIDs[prog], 1, GL_FALSE, &chunks[i]->get_ModelMatrix()[0][0]);
        glDrawElements(GL_TRIANGLES,shape_ptr->get_index_size(),GL_UNSIGNED_SHORT,0);//(void*)0);
            // mode // count// type // element array buffer offset
        i++;

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
    for(size_t i=0;i<chunks.size();i++){
        delete(chunks[i]);
    }
    //destruktor wegen new???

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
}

























