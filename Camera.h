#pragma once
#include "Mat.h"
#include "Vec.h"

// The camera is both our object responsible for viewing the scene.
class CustomCamera {
public:
	Vec3 position{ 0,0,0 };
	float pitch = 0.0f;
	float yaw = 0.0f;

	void move(const Vec3& localDelta);
	Mat4 getViewMatrix() const;
	Vec3 getForward() const;
};