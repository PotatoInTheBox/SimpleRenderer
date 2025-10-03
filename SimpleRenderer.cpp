// SimpleRenderer.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <vector>
#include <array>
#include <string>
#include "raylib.h"
#include "SimpleRenderer.h"
#include "RenderTarget.h"
#include "TestScene.h"
#include "Engine.h"

static RenderTarget CreateScaledRenderTarget(int windowWidth, int windowHeight, int pixelScaleMult) {
	if (windowWidth % pixelScaleMult != 0 || windowHeight % pixelScaleMult != 0) {
		throw std::runtime_error("Window size must be divisible by pixelScaleMult");
	}
	int scaledWidth = windowWidth / pixelScaleMult;
	int scaledHeight = windowHeight / pixelScaleMult;

	printf("RenderTarget: %dx%d (from %dx%d @ scale %d)\n",
		scaledWidth, scaledHeight, windowWidth, windowHeight, pixelScaleMult);

	return RenderTarget(scaledWidth, scaledHeight);
}

int main() {
	//int WIDTH_RESOLUTION = 80; // 80, 320, 640, 800, 1600, 1920, 3840
	//int HEIGHT_RESOLUTION = 60; // 60, 240, 480, 600, 900, 1080, 2160
	//RenderTarget renderTarget = RenderTarget(WIDTH_RESOLUTION, HEIGHT_RESOLUTION);
	int TARGET_WINDOW_WIDTH = 2000; // 640, 800, 1600, 1920, 3840
	int TARGET_WINDOW_HEIGHT = 2000; // 480, 600, 900, 1080, 2160
	int pixelScaleMult = 8;
	RenderTarget renderTarget = CreateScaledRenderTarget(
		TARGET_WINDOW_WIDTH, 
		TARGET_WINDOW_HEIGHT, 
		pixelScaleMult);
	//Scene* scene = new GiantFloorScene();
	Scene * scene = new TestScene();
	
	EngineRun(renderTarget, *scene, pixelScaleMult);

	/*ClippedTriangle tri;
	tri.clippedVertex[0].clipPosition = { -0.5, -0.5, 0.5, 1 };
	tri.clippedVertex[1].clipPosition = { 0.5, 0.5, 0.5, 1 };
	tri.clippedVertex[2].clipPosition = { 2, -2, 0.5, 1 };

	std::vector<ClippedTriangle> result = tri.clipToFrustrum(0.01f);*/

	return 0;
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
