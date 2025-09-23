// SimpleRenderer.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <vector>
#include <array>
#include <string>
#include "raylib.h"
#include "SimpleRenderer.h"
#include "RenderTarget.h"
#include "TestScene.h"
#include "Engine.h"


int main() {
	int WINDOW_WIDTH = 1600; // 1600, 1920, 3840
	int WINDOW_HEIGHT = 900; // 900, 1080, 2160
	RenderTarget renderTarget = RenderTarget(WINDOW_WIDTH, WINDOW_HEIGHT);
	Scene* scene = new TestScene();
	EngineRun(renderTarget, *scene);
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
