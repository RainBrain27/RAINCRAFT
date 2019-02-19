// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"
#include "stdio.h"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 65.0f;

<<<<<<< HEAD
float speed = 5.0f; // 3 units / second
=======
float speed = 30.0f; // 3 units / second
>>>>>>> parent of 9cc1eb9... mehrere texturen in chunks
float mouseSpeed = 0.005f;

glm::vec3 beschleunigun=glm::vec3(0,-9.81,0);
glm::vec3 geschwindigkeit=glm::vec3(0,0,0);
bool jumped=false;

int blockinteraction=0;


void computeInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
//    glm::vec3 direction(
//		cos(verticalAngle) * sin(horizontalAngle),
//		sin(verticalAngle),
//		cos(verticalAngle) * cos(horizontalAngle)
//	);

	glm::vec3 forward(
		sin(horizontalAngle),
		0,
		cos(horizontalAngle)
	);

	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);

	glm::vec3 upward = glm::vec3(
		0,
		1,
		0
	);

	// Up vector
//	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		position += forward * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		position -= forward * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        //position += upward * deltaTime * speed;
        jumped=true;
    }else{jumped=false;}
	
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        //position -= upward * deltaTime * speed;
	}

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        blockinteraction=1;
        //printf("clicked\n");
    }
    else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
        blockinteraction=2;
        //printf("clicked\n");
    }
    else{
        blockinteraction=0;
    }

    position+=upward*(geschwindigkeit*deltaTime+beschleunigun*3.0f*0.5f*deltaTime*deltaTime); //s=v*t+a/2*t^2
    geschwindigkeit+=deltaTime*beschleunigun*3.0f;

    lastTime = currentTime;
}
void computeMatrices(){
	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

<<<<<<< HEAD
    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - 3.14f/2.0f),
        0,
        cos(horizontalAngle - 3.14f/2.0f)
    );

    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    glm::vec3 up = glm::cross( right, direction );

	// Projection matrix : 45? Field of View, 4:3 ratio, display range : 0.1 unit <-> 256 units
    ProjectionMatrix = glm::perspective(glm::radians(FoV), 16.0f / 9.0f, 0.1f, 256.0f);
=======
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 256 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 256.0f);
>>>>>>> parent of 9cc1eb9... mehrere texturen in chunks
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
}
glm::vec3 getCamPos(){
	return position;
<<<<<<< HEAD
}

vec3 setCamPos(vec3 pos)
{
    position=pos;
}

void stopPlayerFall()
{
    geschwindigkeit.y=0;
    if(jumped){
        geschwindigkeit.y=8.5*2;
    }
}

vec3 getForward()
{
    return glm::vec3(
                cos(verticalAngle) * sin(horizontalAngle),
                sin(verticalAngle),
                cos(verticalAngle) * cos(horizontalAngle)
            );
}

int getBlockInteraction()
{
    static double lastTimeBlock = glfwGetTime();
    static bool first=true;
    if(blockinteraction!=0 and (glfwGetTime()-lastTimeBlock>0.33 or first)){
        lastTimeBlock=glfwGetTime();
        first=false;
        return blockinteraction;
    }
    return 0;
}
=======
}
>>>>>>> parent of 9cc1eb9... mehrere texturen in chunks
