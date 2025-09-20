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

void CustomCamera::update(float dt) {
	Vec3 deltaMoveCamera = {};
	if (IsKeyDown(KEY_D))		deltaMoveCamera.x += cameraSpeed * dt; // right
	if (IsKeyDown(KEY_A))		deltaMoveCamera.x -= cameraSpeed * dt; // left
	if (IsKeyDown(KEY_SPACE))   deltaMoveCamera.y += cameraSpeed * dt; // up
	if (IsKeyDown(KEY_C))		deltaMoveCamera.y -= cameraSpeed * dt; // down
	if (IsKeyDown(KEY_S))		deltaMoveCamera.z += cameraSpeed * dt; // forward
	if (IsKeyDown(KEY_W))		deltaMoveCamera.z -= cameraSpeed * dt; // backward
	if (IsKeyDown(KEY_LEFT_SHIFT)) deltaMoveCamera *= speedupMult;
	move(deltaMoveCamera);

	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		Vec2 mouseDelta = Input::GetMouseDelta();

		yaw -= mouseDelta.x * mouseSensitivity;
		pitch += mouseDelta.y * mouseSensitivity;

		if (pitch > 89.999f) pitch = 89.999f;  // don't go upsidedown
		if (pitch < -89.999f) pitch = -89.999f;  // don't go upsidedown

	}
}