#include "Camera.h"
#include "engine.h"

Camera::Camera()
{
}

Camera::Camera(glm::vec3 pos, glm::vec3 rot)
	:position(pos)
{
	yaw = rot.x;
	pitch = rot.y;
	roll = rot.z;
}

void Camera::Update(App* app)
{
	vec3 newPos(0.0f);
	float speed = 25.0f * app->deltaTime;

	

	if(app->input.mouseButtons[LEFT] == BUTTON_PRESSED)
	{

		this->yaw += app->input.mouseDelta.x * app->deltaTime * 12.f;
		this->pitch -= app->input.mouseDelta.y * app->deltaTime * 12.f;

		if (this->pitch > 90.0f)
			this->pitch = 90.0f;
		if (this->pitch < -90.0f)
			this->pitch = -90.0f;


		if (app->input.keys[K_W] == BUTTON_PRESSED) newPos += speed * this->camForward;
		if (app->input.keys[K_S] == BUTTON_PRESSED) newPos -= speed * this->camForward;

		if (app->input.keys[K_D] == BUTTON_PRESSED) newPos -= speed * this->camRight;
		if (app->input.keys[K_A] == BUTTON_PRESSED) newPos += speed * this->camRight;

		if (app->input.keys[K_E] == BUTTON_PRESSED) newPos += speed * this->camUp;
		if (app->input.keys[K_Q] == BUTTON_PRESSED) newPos -= speed * this->camUp;


		this->position += newPos;
		this->target += newPos;

	}

	this->viewMatrix = CalculateCameraRotation();
	//this->viewMatrix = glm::lookAt(this->position, this->target, glm::vec3(0, 1, 0));


}

glm::mat4 Camera::CalculateCameraRotation()
{

	//Recaulculate cam's orientation
	orientation.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	orientation.y = sin(glm::radians(pitch));
	orientation.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	
	//Recalculate axis
	camForward = glm::normalize(orientation);
	camRight = glm::normalize(glm::cross(glm::vec3(0, 1, 0), camForward));
	camUp = glm::cross(camForward, camRight);

	//Recalculate viewMatrix (pos + forward = cam direction to target)
	viewMatrix = glm::lookAt(position, position + camForward, camUp);

	return viewMatrix;
}
