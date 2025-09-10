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
#include <vector>
#include <array>
#include <algorithm> // for std::fill
#include <string>

// Cube vertices (x, y, z)
std::vector<Vec4> cubeVertices = {
	{-0.5f, -0.5f, -0.5f, 1.0f}, // 0
	{ 0.5f, -0.5f, -0.5f, 1.0f}, // 1
	{ 0.5f,  0.5f, -0.5f, 1.0f}, // 2
	{-0.5f,  0.5f, -0.5f, 1.0f}, // 3
	{-0.5f, -0.5f,  0.5f, 1.0f}, // 4
	{ 0.5f, -0.5f,  0.5f, 1.0f}, // 5
	{ 0.5f,  0.5f,  0.5f, 1.0f}, // 6
	{-0.5f,  0.5f,  0.5f, 1.0f}  // 7
};

// Cube faces (triangles, each 3 indices)
std::vector<std::array<int, 3>> cubeTriangles = {
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

    // CPU-side buffers
    std::vector<Color> rgbBuffer(WINDOW_WIDTH * WINDOW_HEIGHT); // RGB color buffer
    std::vector<float> zBuffer(WINDOW_WIDTH * WINDOW_HEIGHT);  // Z-buffer

    // Initialize buffers
    std::fill(rgbBuffer.begin(), rgbBuffer.end(), BLACK);
    std::fill(zBuffer.begin(), zBuffer.end(), 1.0f); // far plane

    // Create a texture to display the RGB buffer
    Image img;
    img.data = rgbBuffer.data();
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

    // set up a matrix


    // movement variables
    float ax = 0.0f;
    float ay = 0.0f;
    float az = 0.0f;
    const float speed = 5.0f; // amount to change per frame
    

    while (!WindowShouldClose()) {
        float dt = GetFrameTime(); // seconds between frames

        if (IsKeyDown(KEY_LEFT))  ax += speed * dt;
        if (IsKeyDown(KEY_RIGHT)) ax -= speed * dt;
        if (IsKeyDown(KEY_UP))    ay += speed * dt;
        if (IsKeyDown(KEY_DOWN))  ay -= speed * dt;
        if (IsKeyDown(KEY_S))     az -= speed * dt;
        if (IsKeyDown(KEY_W))     az += speed * dt;

		

        // Example: draw a gradient in CPU buffers
        for (int y = 0; y < WINDOW_HEIGHT; y++) {
            for (int x = 0; x < WINDOW_WIDTH; x++) {
                int idx = y * WINDOW_WIDTH + x;
                float z = float(y) / WINDOW_HEIGHT; // example depth
                if (z < zBuffer[idx]) {
                    zBuffer[idx] = z;
                    rgbBuffer[idx] = { (unsigned char)(x * 255 / WINDOW_WIDTH),
                                       (unsigned char)(y * 255 / WINDOW_HEIGHT),
                                       128, 255 }; // RGBA
                }
            }
        }

        // Update texture with CPU buffer
        UpdateTexture(screenTex, rgbBuffer.data());

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(screenTex, 0, 0, WHITE);

		// I want to make a movement matrix.
		// This will be our world matrix. Basically telling us where it should
		// go in the world

		Mat4 move_matrix = Mat4::translation(ax, ay, az + 8.0f);
		// Now I want to make a view matrix. Which is basically telling us where
		// the camera is (for now it doesn't exist because the camera is implied
		// to be at 0,0 facing towards positive z)
		Mat4 viewMatrix = Mat4::identity(); // simple camera at origin
		
		// Now I want to make the perspective matrix. Since it gets messy I'll
		// just call a helper function to generate it.
		Mat4 perspectiveMatrix = Mat4::perspective(90.0f, WINDOW_WIDTH / (WINDOW_HEIGHT * 1.0f), 0.01f, 1000.0f);

		Mat4 mvpMatrix = perspectiveMatrix * viewMatrix * move_matrix;
		//Mat4 mvpMatrix = move_matrix;

		std::vector<Vec4> viewSpaceVertices;
		viewSpaceVertices.reserve(cubeVertices.size());

		// apply the matrix transformation to all vertices
		for (auto& vert : cubeVertices) {
			Vec4 clip = mvpMatrix * vert;
			Vec4 ndc = { clip.x / clip.w, clip.y / clip.w, clip.z / clip.w, 1.0f };
			viewSpaceVertices.push_back(ndc);
		}

        // Loop through triangles
        float loop_count = 0.0f;
        for (auto& tri : cubeTriangles) {
            if (static_cast<int>(loop_count) * 500 > counter)
                break;
            loop_count += 1.0f * dt;
            for (int i = 0; i < 3; i++) {
                int idx0 = tri[i];
                int idx1 = tri[(i + 1) % 3]; // next vertex to form a line

                // Add 2.0 to Z to make sure we don't place it behind the camera
                // The biggest consequence is values from 1.0 to 0.0 actually get larger
                // since our division is actually making it bigger now. At near 0.0 it blows
                // up. At exactly 0.0 we just straight up are doing division by 0.
                float z0f = viewSpaceVertices[idx0].z;
                float z1f = viewSpaceVertices[idx1].z;

                //z0f += 2.0f;
                //z1f += 2.0f;
                
                // We do a very primitive perspective divide here (positive z is away from camera)
                // If we divide x and y by z we effectively make that point further towards the horizon.
                float x0f = viewSpaceVertices[idx0].x;
                float y0f = viewSpaceVertices[idx0].y;
                float x1f = viewSpaceVertices[idx1].x;
                float y1f = viewSpaceVertices[idx1].y;

                // apply simple movement
                /*x0f += ax;
                y0f += ay;
                x1f += ax;
                y1f += ay;
                z0f += az;
                z1f += az;*/

                // Perspective divide!
                /*x0f /= z0f;
                y0f /= z0f;
                x1f /= z1f;
                y1f /= z1f;*/

                // Scale and offset to center of screen
                int x0 = static_cast<int>(x0f * (WINDOW_WIDTH / 2.0f) + offsetX);
                int y0 = static_cast<int>(y0f * (WINDOW_HEIGHT / 2.0f) + offsetY);
                int x1 = static_cast<int>(x1f * (WINDOW_WIDTH / 2.0f) + offsetX);
                int y1 = static_cast<int>(y1f * (WINDOW_HEIGHT / 2.0f) + offsetY);

                DrawLine(x0, y0, x1, y1, WHITE);
            }
        }
		
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
