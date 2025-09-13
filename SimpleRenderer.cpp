// SimpleRenderer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include <iostream>
//
//int main()
//{
//    std::cout << "Hello World!\n";
//}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file



#include "raylib.h"
#include "Math.h"
#include "Vec.h"
#include "Mat.h"
#include "Camera.h"
#include <vector>
#include <array>
#include <algorithm> // for std::fill
#include <string>
#include "SimpleRenderer.h"
#include "RenderTarget.h"
#include "Vertex.h"
#include "Tri.h"
#include "Renderer.h"
#include "Scene.h"
#include "Mesh.h"
#include "ObjLoader.h"
#include <sstream>

// Cube vertices (x, y, z)
VertexBuffers vb;
std::vector<Vec3> cubeVertices = {
	{-0.5f, -0.5f, -0.5f}, // 0
	{ 0.5f, -0.5f, -0.5f}, // 1
	{ 0.5f,  0.5f, -0.5f}, // 2
	{-0.5f,  0.5f, -0.5f}, // 3
	{-0.5f, -0.5f,  0.5f}, // 4
	{ 0.5f, -0.5f,  0.5f}, // 5
	{ 0.5f,  0.5f,  0.5f}, // 6
	{-0.5f,  0.5f,  0.5f}  // 7
};

// Cube faces (triangles, each 3 indices)
TriIdxBuffer tb;
std::vector<TriIdx> cubeTriangles = {
	{0, 1, 2}, {0, 2, 3},  // Front
	{5, 4, 7}, {5, 7, 6},  // Back
	{4, 0, 3}, {4, 3, 7},  // Left
	{1, 5, 6}, {1, 6, 2},  // Right
	{3, 2, 6}, {3, 6, 7},  // Top
	{4, 5, 1}, {4, 1, 0}   // Bottom
};


int main() {
	/*int WINDOW_WIDTH = 800;
	int WINDOW_HEIGHT = 600;*/
	int WINDOW_WIDTH = 1920;
	int WINDOW_HEIGHT = 1080;
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello Raylib in Visual Studio");

	RenderTarget renderTarget = RenderTarget(WINDOW_WIDTH, WINDOW_HEIGHT);

	// Create a texture to display the RGB buffer
	Image img;
	img.data = renderTarget.rgbBuffer.data();
	img.width = WINDOW_WIDTH;
	img.height = WINDOW_HEIGHT;
	img.mipmaps = 1;
	img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

	Texture2D screenTex = LoadTextureFromImage(img);
	// What meth is chatgpt cooking?!?! I can't unload the img pointer because
	// rgbBuffer.data() resides in our image and we plan to access it later.
	// Plus, it seems RayLib doesn't like trying to deallocate it for some reason.
	//UnloadImage(img); // texture now owns GPU memory

	const int offsetX = WINDOW_WIDTH / 2;
	const int offsetY = WINDOW_HEIGHT / 2;

	int counter = 0;

	// load vertices and triangles
	for (Vec3 v : cubeVertices)
	{
		vb.positions.push_back(v);
	}
	for (TriIdx f : cubeTriangles)
	{
		tb.tris.push_back(f);
	}


	// draw variables


	// movement variables
	const float speed = 5.0f; // amount to change per frame

	
	// set up camera
	CustomCamera camera;
	// camera variables
	const float sensitivity = 0.2f;
	Vector2 mousePos = GetMousePosition();
	float lastMousePosX = mousePos.x;
	float lastMousePosY = mousePos.y;

	// draw a gradient on the buffer.
	//drawPrettyGradient(WINDOW_HEIGHT, WINDOW_WIDTH, zBuffer, rgbBuffer);

	// create scene, renderer, mesh, etc
	Scene scene = Scene();
	scene.camera = camera;
	scene.camera.move(Vec3{0,0,8});
	MyMesh cubeMesh = MyMesh(vb, tb);
	Mat4 object_matrix = Mat4::translation(0.0f, 0.0f, 0.0f);
	// cube SHOULD canonically be on the LEFT (negative x)
	SceneObject cubeObject = SceneObject(std::string("Cube"), cubeMesh, Mat4::translation(-5.0f, 0.0f, 0.0f));
	MyMesh foxMesh = ObjLoader::LoadFromFile("Resources/foxSitting.obj");
	MyMesh utahTeapotMesh = ObjLoader::LoadFromFile("Resources/utahTeapot.obj");
	// fox SHOULD canonically be on the RIGHT (positive x)
	SceneObject foxObject = SceneObject("Fox", foxMesh, Mat4::translation(5.0f, 0.0f, 0.0f));
	SceneObject utahTeapotObject = SceneObject("UtahTeapot", utahTeapotMesh, object_matrix);
	scene.objects.push_back(&cubeObject);
	scene.objects.push_back(&foxObject);
	scene.objects.push_back(&utahTeapotObject);
	Renderer renderer = Renderer();

	renderer.render(scene, renderTarget);

	while (!WindowShouldClose()) {
		renderTarget.clear();
		drawPrettyGradient(WINDOW_HEIGHT, WINDOW_WIDTH, renderTarget.zBuffer, renderTarget.rgbBuffer);

		float dt = GetFrameTime(); // seconds between frames

		// the problem is here. I ax, ay, and az are world positions.
		// I must keep them as world positions. When adding to them I must take
		// into consideration where the camera is facing. (eg. if the camera
		// is looking down the x axis then left/right should move us in the
		// z axis)
		Vec3 deltaMoveCamera = {};
		if (!IsKeyDown(KEY_LEFT_CONTROL)) {
			if (IsKeyDown(KEY_D))		deltaMoveCamera.x += speed * dt; // right
			if (IsKeyDown(KEY_A))		deltaMoveCamera.x -= speed * dt; // left
			if (IsKeyDown(KEY_SPACE))   deltaMoveCamera.y += speed * dt; // up
			if (IsKeyDown(KEY_C))		deltaMoveCamera.y -= speed * dt; // down
			if (IsKeyDown(KEY_S))		deltaMoveCamera.z += speed * dt; // forward
			if (IsKeyDown(KEY_W))		deltaMoveCamera.z -= speed * dt; // backward
		}
		else {
			Vec3 deltaMoveObject;
			if (IsKeyDown(KEY_D))		deltaMoveObject.x += speed * dt;
			if (IsKeyDown(KEY_A))		deltaMoveObject.x -= speed * dt;
			if (IsKeyDown(KEY_SPACE))   deltaMoveObject.y += speed * dt;
			if (IsKeyDown(KEY_C))		deltaMoveObject.y -= speed * dt;
			if (IsKeyDown(KEY_S))		deltaMoveObject.z += speed * dt;
			if (IsKeyDown(KEY_W))		deltaMoveObject.z -= speed * dt;

			utahTeapotObject.modelMatrix.m[12] += deltaMoveObject.x;
			utahTeapotObject.modelMatrix.m[13] += deltaMoveObject.y;
			utahTeapotObject.modelMatrix.m[14] += deltaMoveObject.z;
		}

		scene.camera.move(deltaMoveCamera);


		// What I could to is put them in a Vec3. Then I apply the view rotation
		// only. Then I add the Vec3 to the world camera position.
		
		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
			Vector2 mousePos = GetMousePosition();
			float mouseXDelta = mousePos.x - lastMousePosX;
			float mouseYDelta = mousePos.y - lastMousePosY;

			scene.camera.yaw		-= mouseXDelta * sensitivity;
			scene.camera.pitch	+= mouseYDelta * sensitivity;


			if (scene.camera.pitch > 89.999f) scene.camera.pitch = 89.999f;
			if (scene.camera.pitch < -89.999f) scene.camera.pitch = -89.999f;

		}
		Vector2 mousePos = GetMousePosition();
		lastMousePosX = mousePos.x;
		lastMousePosY = mousePos.y;
		

		BeginDrawing();
		ClearBackground(BLACK);

		renderer.render(scene, renderTarget);

		UpdateTexture(screenTex, renderTarget.rgbBuffer.data());
		DrawTexture(screenTex, 0, 0, WHITE);

		std::ostringstream ss;
		ss.precision(2);
		ss << std::fixed;
		ss << "Cam Pos: X=" << scene.camera.position.x
			<< " Y=" << scene.camera.position.y
			<< " Z=" << scene.camera.position.z;

		std::ostringstream ss3;
		Vec3 forward = scene.camera.getForward();
		ss3 << "Cam Look: X=" << forward.x
			<< " Y=" << forward.y
			<< " Z=" << forward.z;
		
		std::ostringstream ss2;
		ss2.precision(2);
		ss2 << std::fixed;
		auto test = (utahTeapotObject.modelMatrix * Vec4{ 0,0,0,1 }).x;
		ss2 << "Object Pos: X=" << (utahTeapotObject.modelMatrix * Vec4{0,0,0,1}).x
			<< " Y=" << (utahTeapotObject.modelMatrix * Vec4{ 0,0,0,1 }).y
			<< " Z=" << (utahTeapotObject.modelMatrix * Vec4{ 0,0,0,1 }).z;
		
		DrawText(ss.str().c_str(), 10, 30, 20, LIGHTGRAY);
		DrawText(ss3.str().c_str(), 10, 50, 20, LIGHTGRAY);
		DrawText(ss2.str().c_str(), 10, 70, 20, LIGHTGRAY);
		DrawText((std::string("Aspect: ") + std::to_string(WINDOW_WIDTH / (WINDOW_HEIGHT * 1.0f))).c_str(), 190, 150, 20, LIGHTGRAY);
		DrawText("Hello, Raylib!", 190, 200, 20, LIGHTGRAY);
		DrawFPS(10, 10);
		EndDrawing();
		counter += 1;
	}

	UnloadTexture(screenTex);
	CloseWindow();
	return 0;
}

Vec3 ndcToScreen2(const Vec3& v, int width, int height) {
	//float x = (v.x * 0.5f + 0.5f) * (width - 1);
	//float y = (1.0f - (v.y * 0.5f + 0.5f)) * (height - 1); // flip Y
	float x = (v.x * 0.5f + 0.5f) * (width - 1);
	float y = (v.y * 0.5f + 0.5f) * (height - 1);
	return { x, y, v.z };
}

void drawTriangle2(Tri tri, int WINDOW_HEIGHT, int WINDOW_WIDTH, std::vector<float>& zBuffer, std::vector<Color>& rgbBuffer) {
	

	Tri screenTri;
	screenTri.v0 = ndcToScreen(tri.v0, WINDOW_WIDTH, WINDOW_HEIGHT);
	screenTri.v1 = ndcToScreen(tri.v1, WINDOW_WIDTH, WINDOW_HEIGHT);
	screenTri.v2 = ndcToScreen(tri.v2, WINDOW_WIDTH, WINDOW_HEIGHT);

	Rect boundingBox = screenTri.boundingBox(WINDOW_WIDTH, WINDOW_HEIGHT);

	Vec3 v0 = screenTri.v0;
	Vec3 v1 = screenTri.v1;
	Vec3 v2 = screenTri.v2;

	// Precompute edge vectors
	//Vec2 e0 = { v1.x - v0.x, v1.y - v0.y };
	//Vec2 e1 = { v2.x - v1.x, v2.y - v1.y };
	//Vec2 e2 = { v0.x - v2.x, v0.y - v2.y };
	Vec3 e0 = { v1.x - v0.x, v1.y - v0.y };
	Vec3 e1 = { v2.x - v1.x, v2.y - v1.y };
	Vec3 e2 = { v0.x - v2.x, v0.y - v2.y };

	float area = (e0.x * e2.y - e0.y * e2.x);
	for (int y = boundingBox.yMin; y < boundingBox.yMax; y++) {
		for (int x = boundingBox.xMin; x < boundingBox.xMax; x++) {
			// Compute barycentric coordinates
			float w0 = ((v1.x - v2.x) * (y - v2.y) - (v1.y - v2.y) * (x - v2.x)) / area;
			float w1 = ((v2.x - v0.x) * (y - v2.y) - (v2.y - v0.y) * (x - v2.x)) / area;
			float w2 = 1.0f - w0 - w1;

			// If pixel is inside the triangle
			if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
				// Interpolate depth
				float z = w0 * v0.z + w1 * v1.z + w2 * v2.z;

				int index = y * WINDOW_WIDTH + x;
				if (z < zBuffer[index]) { // depth test
					zBuffer[index] = z;

					// Simple flat color (use vertex colors if desired)
					rgbBuffer[index] = Color{ 255, 122, 122, 255 };
				}
			}
		}
	}
}


void drawPrettyGradient(int WINDOW_HEIGHT, int WINDOW_WIDTH, std::vector<float>& zBuffer, std::vector<Color>& rgbBuffer)
{
	for (int y = 0; y < WINDOW_HEIGHT; y++) {
		for (int x = 0; x < WINDOW_WIDTH; x++) {
			int idx = y * WINDOW_WIDTH + x;
			rgbBuffer[idx] = { (unsigned char)(x * 255 / WINDOW_WIDTH),
				(unsigned char)(y * 255 / WINDOW_HEIGHT),
				128, 255 }; // RGBA
		}
	}
}
