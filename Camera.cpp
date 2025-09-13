#include "Camera.h"
#include "Math.h"
#include "Mat.h"
#include "Vec.h"
#include <cmath>

void CustomCamera::move(const Vec3& localDelta) {
	// Build rotation (yaw then pitch, no roll)
	Mat3 rotation = Mat3::rotation(deg2rad(pitch), deg2rad(-yaw), 0.0f);

	// Transform local input into world space
	Vec3 worldDelta = rotation * localDelta;

	position += worldDelta;
}

Mat4 CustomCamera::getViewMatrix() const {
	// Build rotation
	Mat3 rotation = Mat3::rotation(deg2rad(pitch), deg2rad(-yaw), 0.0f);

	// Extend to 4x4
	Mat4 rot4 = rotation.toMat4();
	Mat4 trans = Mat4::translation(-position.x, -position.y, -position.z);

	return rot4 * trans;
}

Vec3 CustomCamera::getForward() const {

	Mat3 rotation = Mat3::rotation(deg2rad(pitch), deg2rad(-yaw), 0.0f);
	Vec3 forward = { 0,0,1 };
	return rotation * forward;
}