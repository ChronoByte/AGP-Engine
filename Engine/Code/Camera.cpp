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

	isOrbit = false;
	
}

void Camera::Update(App* app)
{
	vec3 newPos(0.0f);
	float speed = 25.0f * app->deltaTime;

	

	if(app->input.mouseButtons[LEFT] == BUTTON_PRESSED)
	{
		
		if (app->input.keys[K_W] == BUTTON_PRESSED) newPos += speed * this->camForward;
		if (app->input.keys[K_S] == BUTTON_PRESSED) newPos -= speed * this->camForward;

		if (app->input.keys[K_D] == BUTTON_PRESSED) newPos += speed * this->camRight;
		if (app->input.keys[K_A] == BUTTON_PRESSED) newPos -= speed * this->camRight;

		if (app->input.keys[K_E] == BUTTON_PRESSED) newPos += speed * this->camUp;
		if (app->input.keys[K_Q] == BUTTON_PRESSED) newPos -= speed * this->camUp;

		isOrbit = false;
	
		this->position += newPos;
		//this->target += newPos;

		this->viewMatrix = CalculateCameraRotation(app);
	}
	else if (app->input.mouseButtons[RIGHT] == BUTTON_PRESSED) {

		isOrbit = true;
		this->viewMatrix = CalculateCameraRotation(app);

	}

	if (app->input.mouseButtons[RIGHT] == BUTTON_RELEASE) isOrbit = false;



	//this->viewMatrix = glm::lookAt(this->position, this->target, glm::vec3(0, 1, 0));


}

glm::mat4 Camera::CalculateCameraRotation(App* app)
{
	if (isOrbit)
	{
		
		glm::vec4 pos(this->position.x, this->position.y, this->position.z, 1);
		glm::vec4 pivot_pos(this->pivot.x, this->pivot.y, this->pivot.z, 1);

		float deltaAngleX = (2 * PI / app->displaySize.x); 
		float deltaAngleY = (PI / app->displaySize.y); 

		float xoffset = -app->input.mouseDelta.x;
		float yoffset = -app->input.mouseDelta.y;

		xoffset *= deltaAngleX;
		yoffset *= deltaAngleY;

		glm::mat4x4 rotationX(1.0f);
		rotationX = glm::rotate(rotationX, xoffset, glm::vec3(0, 1, 0));
		pos = (rotationX * (pos - pivot_pos)) + pivot_pos;

		glm::mat4x4 rotationY(1.0f);
		rotationY = glm::rotate(rotationY, yoffset, camRight);
		glm::vec3 finalPosition = (rotationY * (pos - pivot_pos)) + pivot_pos;

		this->position = std::move(finalPosition);
		this->pivot = std::move(pivot);
		this->camForward = glm::normalize(this->pivot - this->position);
		this->camRight = glm::normalize(glm::cross(camForward, glm::vec3(0, 1, 0)));
		this->camUp = glm::normalize(glm::cross(camRight, camForward));

		//Recalculate viewMatrix (pivot = cam direction to target)
		viewMatrix = glm::lookAt(position, pivot, camUp);
		
	}
	else
	{
		float xoffset = app->input.mouseDelta.x;
		float yoffset = -app->input.mouseDelta.y;

		//Sensivility
		xoffset *= app->deltaTime * 12.f;
		yoffset *= app->deltaTime * 12.f;

		//Update euler angles
		yaw += xoffset;
		pitch += yoffset;

		// Avoid screen flip
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		

		//Recaulculate cam's orientation
		orientation.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		orientation.y = sin(glm::radians(pitch));
		orientation.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	
		//Recalculate axis
		camForward = glm::normalize(orientation);
		camRight = glm::normalize(glm::cross(camForward, glm::vec3(0, 1, 0)));
		camUp = glm::cross(camRight, camForward);

		//Recalculate viewMatrix (pos + forward = cam direction to target)
		viewMatrix = glm::lookAt(position, position + camForward, camUp);
	}

	return viewMatrix;
}
