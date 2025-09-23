#pragma once
// using unique pointers might be a mistake...
#include <memory>
#include "Scene.h"
#include "ObjLoader.h"
#include "Texture.h"

struct TestScene : Scene {
	TestScene() {
		// load a bunch of test object. Place them in the correct place.
		// and place the camera in the correct place.
		camera.move(Vec3{ 0,0,8 });

		// start positions

		Mat4 foxStartMatrix = Mat4::translation(5.0f, 0.0f, 0.0f); // right
		Mat4 teapotMatrix = Mat4::translation(0.0f, 0.0f, 0.0f); // middle
		Mat4 ivanMatrix = Mat4::translation(0.0f, 5.0f, 0.0f); // top
		Mat4 monkeyMatrix = Mat4::translation(-5.0f, 0.0f, 0.0f); // left
		Mat4 floorMatrix = Mat4::translation(0.0f, -5.0f, 0.0f); // bottom
		Mat4 skyboxMatrix = Mat4::scale(1.0f, 1.0f, 1.0f);

		// meshes

		MyMesh foxMesh = ObjLoader::LoadFromFile("Resources/foxSitting.obj");
		MyMesh utahTeapotMesh = ObjLoader::LoadFromFile("Resources/utahTeapot.obj");
		MyMesh ivanMesh = ObjLoader::LoadFromFile("Resources/name.obj");
		MyMesh monkeyMesh = ObjLoader::LoadFromFile("Resources/blender_monkey.obj");
		MyMesh floorMesh = ObjLoader::LoadFromFile("Resources/floor.obj");
		MyMesh skyboxMesh = ObjLoader::LoadFromFile("Resources/skybox.obj");
		

		// fox SHOULD canonically be on the RIGHT (positive x)
		std::unique_ptr<SceneObject> foxObject = std::make_unique<SceneObject>("Fox", foxMesh, foxStartMatrix);
		std::unique_ptr<SceneObject> utahTeapotObject = std::make_unique<SceneObject>("UtahTeapot", utahTeapotMesh, teapotMatrix);
		std::unique_ptr<SceneObject> ivanObject = std::make_unique<SceneObject>("Ivan", ivanMesh, ivanMatrix);
		std::unique_ptr<SceneObject> monkeyObject = std::make_unique<SceneObject>("Blender Monkey", monkeyMesh, monkeyMatrix);
		std::unique_ptr<SceneObject> floorObject = std::make_unique<SceneObject>("Floor", floorMesh, floorMatrix);
		std::unique_ptr<SceneObject> skyboxObject = std::make_unique<SceneObject>("Skybox", skyboxMesh, skyboxMatrix);
		
		// load texture
		foxObject->texture->loadFromFile("Resources/colMap.bytes");
		floorObject->texture->loadFromFile("Resources/uvGrid.bytes");
		skyboxObject->texture->loadFromFile("Resources/skyboxTex.bytes");
		
		
		
		// load objects

		objects.push_back(std::move(foxObject));
		objects.push_back(std::move(utahTeapotObject));
		objects.push_back(std::move(ivanObject));
		objects.push_back(std::move(monkeyObject));
		objects.push_back(std::move(floorObject));
		//objects.push_back(std::move(skyboxObject));

	};

	void update(float dt) override {

		camera.update(dt);
	};
};

struct HexagonScene : Scene {
	HexagonScene() {
		camera.move(Vec3{ 0,0,8 });
		Mat4 hexagonMatrix = Mat4::translation(0.0f, 0.0f, 0.0f);
		MyMesh hexagonMesh = ObjLoader::LoadFromFile("Resources/hexagon.obj");
		std::unique_ptr<SceneObject> hexagonObject = std::make_unique<SceneObject>("Blender Monkey", hexagonMesh, hexagonMatrix);
		objects.push_back(std::move(hexagonObject));
	};

	void update(float dt) override {

		camera.update(dt);
	};
};

struct FloorScene : Scene {
	FloorScene() {
		camera.move(Vec3{ 0,0,8 });
		Mat4 floorMatrix = Mat4::translation(0.0f, -5.0f, 0.0f); // bottom
		MyMesh floorMesh = ObjLoader::LoadFromFile("Resources/floor.obj");
		std::unique_ptr<SceneObject> floorObject = std::make_unique<SceneObject>("Floor", floorMesh, floorMatrix); floorObject->texture->loadFromFile("Resources/uvGrid.bytes");
		objects.push_back(std::move(floorObject));
	};

	void update(float dt) override {

		camera.update(dt);
	};
};

struct SkyBoxScene : Scene {
	SkyBoxScene() {
		Mat4 skyboxMatrix = Mat4::scale(10000.0f, 10000.0f, 10000.0f);
		MyMesh skyboxMesh = ObjLoader::LoadFromFile("Resources/skybox.obj");
		std::unique_ptr<SceneObject> skyboxObject = std::make_unique<SceneObject>("Skybox", skyboxMesh, skyboxMatrix);
		skyboxObject->texture->loadFromFile("Resources/skyboxTex.bytes"); 
		objects.push_back(std::move(skyboxObject));
	};

	void update(float dt) override {

		camera.update(dt);
	};
};