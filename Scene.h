#pragma once

#include "SceneObject.h"
#include "Camera.h"

// For managing objects in our scene. Mostly to spawn objects and add them to the
// renderer.

// Scene containing all objects and a camera
struct Scene {
	CustomCamera camera;
	std::vector<SceneObject*> objects;

	//Scene(std::vector<SceneObject>& objs) : objects(objs) {}
	// optional: scene-level lights, environment maps, etc.
};