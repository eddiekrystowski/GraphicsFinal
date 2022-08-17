#pragma once

#include <glm\glm.hpp>

enum Projection { Orthographic, Perspective };

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    PAUSE
};

class Camera {
private:
	float aspectRatio, nearClip, farClip;
	// orthographic
	float orthographicSize;
	// perspective
	float fieldOfView;

	Projection projectionType;
	glm::mat4 projectionMatrix;

	glm::vec3 position, front, worldUp, up, right;
	float yaw, pitch;
	glm::mat4 viewMatrix;

	
	float MovementSpeed;
	float MouseSensitivity;

	void InitViewMatrix();
	void UpdateVectors();

public:
	Camera();
	Camera(Projection projection, float size, float aspectRatio, float nearClip = 0.1f, float farClip = 1000.0f);

	Projection GetProjectionType();
	glm::mat4 GetProjectionMatrix();

	void SetPosition(glm::vec3 position);
	glm::vec3 GetPosition();
	glm::vec3 GetFront();
	glm::vec3 GetUp();
	glm::vec3 GetRight();

	void SetPitch(float pitch);
	float GetPitch();
	void SetYaw(float yaw);
	float GetYaw();
	glm::mat4 GetViewMatrix();

	float GetOrthographicSize();
	void SetOrthographicSize(float orthographicSize);
	float GetFieldOfView();
	void SetFieldOfView(float fov);
	float GetAspectRatio();
	void SetAspectRatio(float aspectRatio);
	float GetNearClip();
	void SetNearClip(float nearClip);
	float GetFarClip();
	void SetFarClip(float farClip);

	void ProcessMouseScroll(float yoffset);
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
	void pause();

	bool paused = false;

	glm::vec3 GetDirection();
private:
	void ResetCamera();
	void SetOrthographic(float size, float aspectRatio, float nearClip = 0.1f, float farClip = 1000.0f);
	void SetPerspective(float fieldOfView, float aspectRatio, float nearClip = 0.1f, float farClip = 1000.0f);
};



//#pragma once
//
//#include <glad/glad.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#include <vector>
//
//// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
//enum Camera_Movement {
//    FORWARD,
//    BACKWARD,
//    LEFT,
//    RIGHT,
//    UP,
//    DOWN,
//    PAUSE
//};
//
//// Default camera values
//const float YAW         = -90.0f;
//const float PITCH       =  0.0f;
//const float SPEED       =  50.0f;
//const float SENSITIVITY =  0.1f;
//const float ZOOM        =  45.0f;
//
//
//// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
//class Camera
//{
//public:
//    // camera Attributes
//    glm::vec3 Position;
//    glm::vec3 Front;
//    glm::vec3 Up;
//    glm::vec3 Right;
//    glm::vec3 WorldUp;
//    float nearClip;
//    float farClip;
//    // euler Angles
//    float Yaw;
//    float Pitch;
//    // camera options
//    float MovementSpeed;
//    float MouseSensitivity;
//    float Zoom;
//    //pause
//    bool paused;
//
//    // constructor with vectors
//    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
//    {
//        Position = position;
//        WorldUp = up;
//        Yaw = yaw;
//        Pitch = pitch;
//        paused = false;
//        nearClip = 0.1;
//        farClip = 1000.0f;
//        updateCameraVectors();
//    }
//    // constructor with scalar values
//    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
//    {
//        Position = glm::vec3(posX, posY, posZ);
//        WorldUp = glm::vec3(upX, upY, upZ);
//        Yaw = yaw;
//        Pitch = pitch;
//        paused = false;
//        updateCameraVectors();
//    }
//
//    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
//    glm::mat4 GetViewMatrix()
//    {
//        return glm::lookAt(Position, Position + Front, Up);
//    }
//
//    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
//    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
//    {
//        if (paused) return;
//        float velocity = MovementSpeed * deltaTime;
//        if (direction == FORWARD)
//            Position += Front * velocity;
//        if (direction == BACKWARD)
//            Position -= Front * velocity;
//        if (direction == LEFT)
//            Position -= Right * velocity;
//        if (direction == RIGHT)
//            Position += Right * velocity;
//        if (direction == UP)
//            Position += Up * velocity;
//        if (direction == DOWN)
//            Position -= Up * velocity;
//    }
//
//    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
//    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
//    {
//        if (paused) return;
//        xoffset *= MouseSensitivity;
//        yoffset *= MouseSensitivity;
//
//        Yaw   += xoffset;
//        Pitch += yoffset;
//
//        // make sure that when pitch is out of bounds, screen doesn't get flipped
//        if (constrainPitch)
//        {
//            if (Pitch > 89.0f)
//                Pitch = 89.0f;
//            if (Pitch < -89.0f)
//                Pitch = -89.0f;
//        }
//
//        // update Front, Right and Up Vectors using the updated Euler angles
//        updateCameraVectors();
//    }
//
//    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
//    void ProcessMouseScroll(float yoffset)
//    {
//        if (paused) return;
//        Zoom -= (float)yoffset;
//        if (Zoom < 1.0f)
//            Zoom = 1.0f;
//        if (Zoom > 45.0f)
//            Zoom = 45.0f; 
//    }
//
//    //disables input processing when paused
//    void pause() {
//        paused = !paused;
//    }
//
//private:
//    // calculates the front vector from the Camera's (updated) Euler Angles
//    void updateCameraVectors()
//    {
//        // calculate the new Front vector
//        glm::vec3 front;
//        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//        front.y = sin(glm::radians(Pitch));
//        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//        Front = glm::normalize(front);
//        // also re-calculate the Right and Up vector
//        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
//        Up    = glm::normalize(glm::cross(Right, Front));
//    }
//};
