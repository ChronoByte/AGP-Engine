#include "Camera.h"
#include "engine.h"

Camera::Camera()
{
}

Camera::Camera(glm::vec3 pos, glm::vec3 target)
	:position(pos), target(target)
{
}

void Camera::Update(App* app)
{
	vec3 newPos(0.0f);
	float speed = 10.0f * app->deltaTime;

	
	if (app->input.keys[K_W] == BUTTON_PRESSED) newPos.z -= speed;
	if (app->input.keys[K_S] == BUTTON_PRESSED) newPos.z += speed;

	if (app->input.keys[K_D] == BUTTON_PRESSED) newPos.x += speed;
	if (app->input.keys[K_A] == BUTTON_PRESSED) newPos.x -= speed;

	if (app->input.keys[K_E] == BUTTON_PRESSED) newPos.y += speed;
	if (app->input.keys[K_Q] == BUTTON_PRESSED) newPos.y -= speed;


	this->position += newPos;
	this->target += newPos;

	

	this->viewMatrix = glm::lookAt(this->position, this->target, glm::vec3(0, 1, 0));


}
