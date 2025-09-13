#pragma once

#include "Vec.h"
#include <vector>
#include "Tri.h"
#include "Math.h"


// implement barycentric triangle rasterization to do interpolation, 
// depth testing, and anything else we need to figure out to get a pixel
// created.


Vec3 ndcToScreen(const Vec3& v, int width, int height) {
	//float x = (v.x * 0.5f + 0.5f) * (width - 1);
	//float y = (1.0f - (v.y * 0.5f + 0.5f)) * (height - 1); // flip Y
	float x = (v.x * 0.5f + 0.5f) * (width - 1);
	float y = (v.y * 0.5f + 0.5f) * (height - 1);
	return { x, y, v.z };
}

void drawTriangle(Tri tri, int WINDOW_HEIGHT, int WINDOW_WIDTH, std::vector<float>& zBuffer, std::vector<Color>& rgbBuffer, Color color = { 255, 122, 122, 255 }) {


	/*Tri screenTri;
	screenTri.v0 = ndcToScreen(tri.v0, WINDOW_WIDTH, WINDOW_HEIGHT);
	screenTri.v1 = ndcToScreen(tri.v1, WINDOW_WIDTH, WINDOW_HEIGHT);
	screenTri.v2 = ndcToScreen(tri.v2, WINDOW_WIDTH, WINDOW_HEIGHT);*/

	Rect boundingBox = tri.boundingBox(WINDOW_WIDTH, WINDOW_HEIGHT);

	Vec3 v0 = tri.v0;
	Vec3 v1 = tri.v1;
	Vec3 v2 = tri.v2;

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
					rgbBuffer[index] = color;

					// DEBUG draw a the edges of the triangle
					if (w0 < 0.01 || w1 < 0.01 || w2 < 0.01) {
						rgbBuffer[index] = GREEN;
					}
				}
			}
		}
	}
}