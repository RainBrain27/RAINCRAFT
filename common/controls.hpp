#ifndef CONTROLS_HPP
#define CONTROLS_HPP
#include <glm/glm.hpp>

void computeInputs();
void computeMatrices();
void stopPlayerFall();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::vec3 getCamPos();
<<<<<<< HEAD
glm::vec3 setCamPos(glm::vec3 pos);
glm::vec3 getForward();
int getBlockInteraction();
=======
>>>>>>> parent of 9cc1eb9... mehrere texturen in chunks

#endif
