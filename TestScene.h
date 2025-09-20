#pragma once
// using unique pointers might be a mistake...
#include <memory>
#include "Scene.h"
#include "ObjLoader.h"

struct TestScene : Scene {
	TestScene() {
		// load a bunch of test object. Place them in the correct place.
		// and place the camera in the correct place.
		camera.move(Vec3{ 0,0,8 });

		// start positions

		Mat4 foxStartMatrix = Mat4::translation(5.0f, 0.0f, 0.0f);
		Mat4 teapotMatrix = Mat4::translation(0.0f, 0.0f, 0.0f);
		Mat4 ivanMatrix = Mat4::translation(0.0f, 5.0f, 0.0f);

		// meshes

		MyMesh foxMesh = ObjLoader::LoadFromFile("Resources/foxSitting.obj");
		MyMesh utahTeapotMesh = ObjLoader::LoadFromFile("Resources/utahTeapot.obj");
		MyMesh ivanMesh = ObjLoader::LoadFromFile("Resources/name.obj");

		// fox SHOULD canonically be on the RIGHT (positive x)
		std::unique_ptr<SceneObject> foxObject = std::make_unique<SceneObject>("Fox", foxMesh, foxStartMatrix);
		std::unique_ptr<SceneObject> utahTeapotObject = std::make_unique<SceneObject>("UtahTeapot", utahTeapotMesh, teapotMatrix);
		std::unique_ptr<SceneObject> ivanObject = std::make_unique<SceneObject>("Ivan", ivanMesh, ivanMatrix);

		// load objects

		objects.push_back(std::move(foxObject));
		objects.push_back(std::move(utahTeapotObject));
		objects.push_back(std::move(ivanObject));

	};

	void update(float dt) override {

		camera.update(dt);
	};
};