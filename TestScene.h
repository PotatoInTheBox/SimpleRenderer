#pragma once
// using unique pointers might be a mistake...
#include <memory>
#include "Scene.h"
#include "ObjLoader.h"
#include "Texture.h"
#include "Shader.h"

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
		Mat4 skyboxMatrix = Mat4::scale(400.0f, 400.0f, 400.0f);

		// meshes

		MyMesh foxMesh = ObjLoader::LoadFromFile("Resources/foxSitting.obj");
		MyMesh utahTeapotMesh = ObjLoader::LoadFromFile("Resources/utahTeapot.obj");
		MyMesh ivanMesh = ObjLoader::LoadFromFile("Resources/name.obj");
		MyMesh monkeyMesh = ObjLoader::LoadFromFile("Resources/blender_monkey.obj");
		MyMesh floorMesh = ObjLoader::LoadFromFile("Resources/floor.obj");
		MyMesh skyboxMesh = ObjLoader::LoadFromFile("Resources/skybox.obj");
		

		// fox SHOULD canonically be on the RIGHT (positive x)
		SceneObject* foxObject = new SceneObject("Fox", foxMesh, foxStartMatrix);
		SceneObject* utahTeapotObject = new SceneObject("UtahTeapot", utahTeapotMesh, teapotMatrix);
		SceneObject* ivanObject = new SceneObject("Ivan", ivanMesh, ivanMatrix);
		SceneObject* monkeyObject = new SceneObject("Blender Monkey", monkeyMesh, monkeyMatrix);
		SceneObject* floorObject = new SceneObject("Floor", floorMesh, floorMatrix);
		SceneObject* skyboxObject = new SceneObject("Skybox", skyboxMesh, skyboxMatrix);
		
		// load texture
		foxObject->texture->loadFromFile("Resources/colMap.bytes");
		floorObject->texture->loadFromFile("Resources/uvGrid.bytes");
		skyboxObject->texture->loadFromFile("Resources/uvGrid.bytes");

		foxObject->shaders.fragmentShader = new TextureFragmentShader();
		floorObject->shaders.fragmentShader = new TextureFragmentShader();
		skyboxObject->shaders.fragmentShader = new SkyboxFragmentShader();
		skyboxObject->shaders.vertexShader = new BasicVertexShader();
		
		
		// load objects

		objects.push_back(foxObject);
		objects.push_back(utahTeapotObject);
		objects.push_back(ivanObject);
		objects.push_back(monkeyObject);
		objects.push_back(floorObject);
		objects.push_back(skyboxObject);

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
		SceneObject* hexagonObject = new SceneObject("Blender Monkey", hexagonMesh, hexagonMatrix);
		objects.push_back(hexagonObject);
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
		SceneObject* floorObject = new SceneObject("Floor", floorMesh, floorMatrix); floorObject->texture->loadFromFile("Resources/uvGrid.bytes");
		floorObject->shaders.fragmentShader = new TextureFragmentShader();
		objects.push_back(floorObject);
	};

	void update(float dt) override {

		camera.update(dt);
	};
};

struct SkyBoxScene : Scene {
	SkyBoxScene() {
		Mat4 skyboxMatrix = Mat4::scale(900.0f, 900.0f, 900.0f);
		MyMesh skyboxMesh = ObjLoader::LoadFromFile("Resources/skybox.obj");
		SceneObject* skyboxObject = new SceneObject("Skybox", skyboxMesh, skyboxMatrix);
		skyboxObject->texture->loadFromFile("Resources/skyboxTex.bytes"); 
		skyboxObject->shaders.fragmentShader = new SkyboxFragmentShader();
		objects.push_back(std::move(skyboxObject));
	};

	void update(float dt) override {

		camera.update(dt);
	};
};


struct GiantFloorScene : Scene {
	GiantFloorScene() {
		camera.move(Vec3{ 0,500,8 }); // move the camera up a little (a lot)
		Mat4 floorMatrix = Mat4::scale(9000.0f, 9000.0f, 9000.0f);
		MyMesh floorMesh = ObjLoader::LoadFromFile("Resources/floor.obj");
		SceneObject* floorObject = new SceneObject("Floor", floorMesh, floorMatrix); floorObject->texture->loadFromFile("Resources/uvGrid.bytes");
		floorObject->shaders.fragmentShader = new TextureFragmentShader();
		objects.push_back(floorObject);
	};

	void update(float dt) override {

		camera.update(dt);
	};
};