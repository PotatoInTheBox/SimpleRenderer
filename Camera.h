#pragma once
#include "Mat.h"
#include "Vec.h"
#include "Input.h"

// The camera is both our object responsible for viewing the scene.
class CustomCamera {
public:
	Vec3 position{ 0,0,0 }; // the current position of the camera (in world space)
	float pitch = 0.0f; // how much we are looking up and down (in degrees)
	float yaw = 0.0f; // what direction we are facing horizontally (in degrees)
	float mouseSensitivity = 0.2f; // how much to scale the per-pixel mouse movement (1 = one degree per pixel)
	float cameraSpeed = 1; // 1 == move one unit per second
	float speedupMult = 10; // how much to increase camera speed when holding shift

	// Move the camera relative to its own look direction (eg. z = 1 means forward)
	void move(const Vec3& localDelta);

	// Get the view matrix describing how objects should move relative to the camera
	Mat4 getViewMatrix() const;

	// get the forward direction relative to the camera
	Vec3 getForward() const;

	// updates the camera position depending on the input of the current frame.
	void update(float dt);
};