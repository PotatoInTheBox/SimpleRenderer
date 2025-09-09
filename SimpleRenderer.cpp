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
#include <vector>
#include <array>
#include <algorithm> // for std::fill

// Cube vertices (x, y, z)
std::vector<std::array<float, 3>> cubeVertices = {
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
std::vector<std::array<int, 3>> cubeTriangles = {
    // Front face
    {0, 1, 2}, {0, 2, 3},
    // Back face
    {5, 4, 7}, {5, 7, 6},
    // Left face
    {4, 0, 3}, {4, 3, 7},
    // Right face
    {1, 5, 6}, {1, 6, 2},
    // Top face
    {3, 2, 6}, {3, 6, 7},
    // Bottom face
    {4, 5, 1}, {4, 1, 0}
};

int main() {
    int WINDOW_WIDTH = 800;
    int WINDOW_HEIGHT = 600;
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
    //UnloadImage(img); // texture now owns GPU memory

    const float scale = 200.0f;   // scale factor
    const int offsetX = WINDOW_WIDTH / 2;
    const int offsetY = WINDOW_HEIGHT / 2;

    int counter = 0;

    // set up a matrix


    // movement variables
    float ax = 0.0f;
    float ay = 0.0f;
    float az = 0.0f;
    const float speed = 0.01f; // amount to change per frame
    

    while (!WindowShouldClose()) {

        if (IsKeyDown(KEY_LEFT))  ax -= speed;
        if (IsKeyDown(KEY_RIGHT)) ax += speed;
        if (IsKeyDown(KEY_UP))    ay -= speed;
        if (IsKeyDown(KEY_DOWN))  ay += speed;
        if (IsKeyDown(KEY_S))     az -= speed;
        if (IsKeyDown(KEY_W))     az += speed;

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

        // Loop through triangles
        int loop_count = 0;
        for (auto& tri : cubeTriangles) {
            if (loop_count * 500 > counter)
                break;
            loop_count += 1;
            for (int i = 0; i < 3; i++) {
                int idx0 = tri[i];
                int idx1 = tri[(i + 1) % 3]; // next vertex to form a line

                // Add 2.0 to Z to make sure we don't place it behind the camera
                // The biggest consequence is values from 1.0 to 0.0 actually get larger
                // since our division is actually making it bigger now. At near 0.0 it blows
                // up. At exactly 0.0 we just straight up are doing division by 0.
                float z0f = cubeVertices[idx0][2];
                float z1f = cubeVertices[idx1][2];

                z0f += 2.0f;
                z1f += 2.0f;
                
                // We do a very primitive perspective divide here (positive z is away from camera)
                // If we divide x and y by z we effectively make that point further towards the horizon.
                float x0f = cubeVertices[idx0][0];
                float y0f = cubeVertices[idx0][1];
                float x1f = cubeVertices[idx1][0];
                float y1f = cubeVertices[idx1][1];

                // apply simple movement
                x0f += ax;
                y0f += ay;
                x1f += ax;
                y1f += ay;
                z0f += az;
                z1f += az;

                // Perspective divide!
                x0f /= z0f;
                y0f /= z0f;
                x1f /= z1f;
                y1f /= z1f;

                // Scale and offset to center of screen
                int x0 = static_cast<int>(x0f * scale + offsetX);
                int y0 = static_cast<int>(y0f * scale + offsetY);
                int x1 = static_cast<int>(x1f * scale + offsetX);
                int y1 = static_cast<int>(y1f * scale + offsetY);

                DrawLine(x0, y0, x1, y1, WHITE);
            }
        }

        DrawText("Hello, Raylib!", 190, 200, 20, LIGHTGRAY);
        DrawFPS(10, 10);
        EndDrawing();
        counter += 1;
    }

    UnloadTexture(screenTex);
    CloseWindow();
    return 0;
}
