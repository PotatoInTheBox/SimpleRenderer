#pragma once
#include "raylib.h"
#include "RenderTarget.h"
#include "Scene.h"
#include "Renderer.h"

// for debugging
#include "Rasterizer.h"

// Given a scene we want to render it over and over again.

// This means we need to tell the scene when to UPDATE.

// Both graphics and ticks will update within the SAME UPDATE cuz I'm not programming all that.

// I see 0 reason to have this be a class.

// I will be including the entire code in the header. By including .h you basically
// are wanting to verbatum type this function out

void EngineRun(RenderTarget renderTarget, Scene& scene, int pixelScaleMult = 1) {

	// we need to set up raylib. I just need to open a new window and grab/set
	// an RGB buffer I can write to.

	SetTraceLogLevel(LOG_ERROR); // Ignore raylib info logs
	InitWindow(renderTarget.width * pixelScaleMult, renderTarget.height * pixelScaleMult, "Ivan's Simple Renderer");

	// Create a texture to display the RGB buffer
	Image img;
	img.data = renderTarget.rgbBuffer.data();
	img.width = renderTarget.width;
	img.height = renderTarget.height;
	img.mipmaps = 1;
	img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

	Texture2D screenTex = LoadTextureFromImage(img);

	// I'll try this out later. (pretty sure it's default tho)
	// SetTextureFilter(screenTex, TEXTURE_FILTER_POINT);

	// instantiate our renderer
	Renderer renderer = Renderer();

	while (!WindowShouldClose()) {
		renderTarget.clear();
		//drawPrettyGradient(renderTarget.height, renderTarget.width, renderTarget.zBuffer, renderTarget.rgbBuffer);

		
		//ClearBackground(BLACK);

		// update the scene
		float dt = GetFrameTime(); // seconds between frames
		scene.update(dt);

		// render the scene
		renderer.render(scene, renderTarget);

		// start drawing to screen using raylib
		BeginDrawing();
		UpdateTexture(screenTex, renderTarget.rgbBuffer.data());
		//DrawTexture(screenTex, 0, 0, WHITE);
		float scale = static_cast<float>(pixelScaleMult);
		Rectangle src = { 0, 0, (float)screenTex.width, (float)screenTex.height };
		Rectangle dst = { 0, 0, (float)(screenTex.width * scale), (float)(screenTex.height * scale) };
		Vector2 origin = { 0, 0 };

		DrawTexturePro(screenTex, src, dst, origin, 0.0f, WHITE);

		// debug stuff

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

		std::ostringstream ss4;
		ss4 << "Cam speedup speed: " << scene.camera.speedupMult;

		std::ostringstream ss5;
		ss5 << "Triangles drawn: " << trianglesDrawn << " Triangles Processed: " << clippedTrianglesRendered;
		trianglesDrawn = 0;
		clippedTrianglesRendered = 0;

		DrawText(ss.str().c_str(), 10, 30, 20, LIGHTGRAY);
		DrawText(ss3.str().c_str(), 10, 50, 20, LIGHTGRAY);
		DrawText(ss4.str().c_str(), 10, 70, 20, LIGHTGRAY);
		DrawText(ss5.str().c_str(), 10, 90, 20, LIGHTGRAY);
		//DrawText("Hello, Raylib!", 190, 200, 20, LIGHTGRAY);
		DrawFPS(10, 10);

		// finish rendering this frame.
		EndDrawing();
	}

	UnloadTexture(screenTex);
	CloseWindow();
}



// Eg. usage (kind of just following the convention Sebastian Lague used)
// Scene scene = FoxScene();
// RenderTarget renderTarget = RenderTarget(1920, 1080);
// EngineRun(renderTarget, scene) // will not return until rendering stops