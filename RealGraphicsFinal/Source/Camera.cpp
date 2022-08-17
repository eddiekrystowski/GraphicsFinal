#include "Camera.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <cstdio>

Camera::Camera() {
	SetPerspective(45.0f, 1.0f);
	InitViewMatrix();
}
Camera::Camera(Projection projection, float size, float aspectRatio, float nearClip, float farClip) {
	if (projection == Projection::Perspective) {
		SetPerspective(size, aspectRatio, nearClip, farClip);
	}
	else {
		SetOrthographic(size, aspectRatio, nearClip, farClip);
	}
	InitViewMatrix();
}
void Camera::InitViewMatrix() {
	position = glm::vec3(0.0, 0.0, 0.0);
	front = glm::vec3(0, 0, 1);
	worldUp = glm::vec3(0, 1, 0);
	yaw = -90.0f;
	pitch = 0.0f;
	MovementSpeed = 35.0f;
	MouseSensitivity = 0.1f;
	UpdateVectors();
}

Projection Camera::GetProjectionType() {
	return projectionType;
}
glm::mat4 Camera::GetProjectionMatrix() {
	return projectionMatrix;
}
void Camera::SetPosition(glm::vec3 position) {
	this->position = position;
	viewMatrix = glm::lookAt(this->position, this->position + this->front, this->up);
}
glm::vec3 Camera::GetPosition() {
	return position;
}
glm::vec3 Camera::GetFront() {
	return front;
}
glm::vec3 Camera::GetUp() {
	return up;
}
glm::vec3 Camera::GetRight() {
	return right;
}
void Camera::SetPitch(float pitch) {
	this->pitch = pitch;
	UpdateVectors();
}
float Camera::GetPitch() {
	return pitch;
}
void Camera::SetYaw(float yaw) {
	this->yaw = yaw;
	UpdateVectors();
}
float Camera::GetYaw() {
	return yaw;
}
void Camera::UpdateVectors() {
	glm::vec3 front;
	front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
	front.y = sin(glm::radians(this->pitch));
	front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

	this->front = glm::normalize(front);
	this->right = glm::normalize(glm::cross(this->front, this->worldUp));
	this->up = glm::normalize(glm::cross(this->right, this->front));

	viewMatrix = glm::lookAt(this->position, this->position + this->front, this->up);
}
glm::mat4 Camera::GetViewMatrix() {
	return viewMatrix;
}

float Camera::GetOrthographicSize() {
	return orthographicSize;
}
void Camera::SetOrthographicSize(float orthographicSize) {
	if (projectionType == Projection::Orthographic) {
		SetPerspective(orthographicSize, aspectRatio, nearClip, farClip);
	}
}
float Camera::GetFieldOfView() {
	return fieldOfView;
}
void Camera::SetFieldOfView(float fov) {
	if (projectionType == Projection::Perspective) {
		SetPerspective(fov, aspectRatio, nearClip, farClip);
	}
}
float Camera::GetAspectRatio() {
	return aspectRatio;
}
void Camera::SetAspectRatio(float aspectRatio) {
	this->aspectRatio = aspectRatio;
	ResetCamera();
}
float Camera::GetNearClip() {
	return nearClip;
}
void Camera::SetNearClip(float nearClip) {
	this->nearClip = nearClip;
	ResetCamera();
}
float Camera::GetFarClip() {
	return farClip;
}
void Camera::SetFarClip(float farClip) {
	this->farClip = farClip;
	ResetCamera();
}

void Camera::ResetCamera() {
	if (projectionType == Projection::Perspective) {
		SetPerspective(fieldOfView, aspectRatio, nearClip, farClip);
	}
	else {
		SetOrthographic(orthographicSize, aspectRatio, nearClip, farClip);
	}
}
void Camera::SetOrthographic(float orthographicSize, float aspectRatio, float nearClip, float farClip) {
	this->projectionType = Projection::Orthographic;
	this->orthographicSize = orthographicSize;
	this->aspectRatio = aspectRatio;
	this->nearClip = nearClip;
	this->farClip = farClip;

	float left, right, bottom, top;
	left = -orthographicSize * aspectRatio;
	right = orthographicSize * aspectRatio;
	bottom = -orthographicSize;
	top = orthographicSize;

	projectionMatrix = glm::ortho(left, right, bottom, top, nearClip, farClip);
}
void Camera::SetPerspective(float fieldOfView, float aspectRatio, float nearClip, float farClip) {
	this->projectionType = Projection::Perspective;
	this->fieldOfView = fieldOfView;
	this->aspectRatio = aspectRatio;
	this->nearClip = nearClip;
	this->farClip = farClip;

	projectionMatrix = glm::perspective(fieldOfView, aspectRatio, nearClip, 10000.0f);
	//projectionMatrix = glm::infinitePerspective(fieldOfView, aspectRatio, nearClip);
}


glm::vec3 Camera::GetDirection() {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, GetPosition());
	//glm::mat4 modelView = model * GetViewMatrix();
	//return glm::vec3(modelView[2][0], modelView[2][1], modelView[2][2]);
	const glm::mat4 inverted = glm::inverse(model);
	const glm::vec3 forward = normalize(glm::vec3(inverted[2]));
	return this->front;
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    if (paused) return;
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        position += front * velocity;
    if (direction == BACKWARD)
		position -= front * velocity;
    if (direction == LEFT)
		position -= right * velocity;
    if (direction == RIGHT)
		position += right * velocity;
    if (direction == UP)
		position += up * velocity;
    if (direction == DOWN)
		position -= up * velocity;

	UpdateVectors();

}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    if (paused) return;
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (pitch > 89.0f)
			pitch = 89.0f;
        if (pitch < -89.0f)
			pitch = -89.0f;
    }

    // update Front, Right and Up Vectosrs using the updated Euler angles
    UpdateVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset) {
    if (paused) return;
    //Zoom -= (float)yoffset;
    //if (Zoom < 1.0f)
    //    Zoom = 1.0f;
    //if (Zoom > 45.0f)
    //    Zoom = 45.0f; 
}

//disables input processing when paused
void Camera::pause() {
    paused = !paused;
}
