#pragma once

#include "Vec.h"
#include <vector>
#include "Tri.h"
#include "Math.h"
#include "Shader.h"
#include "Texture.h"


// implement barycentric triangle rasterization to do interpolation, 
// depth testing, and anything else we need to figure out to get a pixel
// created.

int trianglesDrawn = 0;


Vec3 ndcToScreen(const Vec3& v, int width, int height) {
	//float x = (v.x * 0.5f + 0.5f) * (width - 1);
	//float y = (1.0f - (v.y * 0.5f + 0.5f)) * (height - 1); // flip Y
	float x = (v.x * 0.5f + 0.5f) * (width - 1);
	float y = (v.y * 0.5f + 0.5f) * (height - 1);
	return { x, y, v.z };
}

void drawLine(Vec3 from, Vec3 to, Color color, int WINDOW_HEIGHT, int WINDOW_WIDTH, std::vector<float>& zBuffer, std::vector<Color>& rgbBuffer) {
	int x0 = static_cast<int>(from.x);
	int y0 = static_cast<int>(from.y);
	int x1 = static_cast<int>(to.x);
	int y1 = static_cast<int>(to.y);

	float z0 = from.z;
	float z1 = to.z;

	int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy;

	while (true) {
		// If fully out of bounds, stop
		if ((x0 < 0 && x1 < 0) || (x0 >= WINDOW_WIDTH && x1 >= WINDOW_WIDTH) ||
			(y0 < 0 && y1 < 0) || (y0 >= WINDOW_HEIGHT && y1 >= WINDOW_HEIGHT)) {
			break;
		}
		// Interpolate depth
		float t = (dx != 0 ? float(x0 - static_cast<int>(from.x)) / dx : 0.0f);
		float z = z0 * (1 - t) + z1 * t;

		if (x0 >= 0 && x0 < WINDOW_WIDTH && y0 >= 0 && y0 < WINDOW_HEIGHT) {
			int idx = y0 * WINDOW_WIDTH + x0;
			if (z < zBuffer[idx]) {
				zBuffer[idx] = z;
				rgbBuffer[idx] = color;
			}
		}

		if (x0 == x1 && y0 == y1) break;
		int e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}
}

void drawTriangle(
	Tri tri, 
	int WINDOW_HEIGHT, int WINDOW_WIDTH, 
	std::vector<float>& zBuffer, 
	std::vector<Color>& rgbBuffer, 
	MyTexture* texture,
	IFragmentShader* fragmentShader) {

	trianglesDrawn += 1;

	Rect boundingBox = tri.boundingBox(WINDOW_WIDTH, WINDOW_HEIGHT);

	Vec3 v0 = tri.v0;
	Vec3 v1 = tri.v1;
	Vec3 v2 = tri.v2;

	Vec3 n0 = tri.n0; // vertex normals
	Vec3 n1 = tri.n1;
	Vec3 n2 = tri.n2;

	Vec3 e0 = { v1.x - v0.x, v1.y - v0.y };
	Vec3 e1 = { v2.x - v1.x, v2.y - v1.y };
	Vec3 e2 = { v0.x - v2.x, v0.y - v2.y };

	FragmentInput fragmentInput;

	float area = (e0.x * e2.y - e0.y * e2.x);
	for (int y = boundingBox.yMin; y <= boundingBox.yMax; y++) {
		for (int x = boundingBox.xMin; x <= boundingBox.xMax; x++) {
			// Compute barycentric coordinates
			float w0 = ((v1.x - v2.x) * (y - v2.y) - (v1.y - v2.y) * (x - v2.x)) / area;
			float w1 = ((v2.x - v0.x) * (y - v2.y) - (v2.y - v0.y) * (x - v2.x)) / area;
			//float w2 = 1.0f - w0 - w1;
			float w2 = ((v0.x - v1.x) * (y - v1.y) - (v0.y - v1.y) * (x - v1.x)) / area;

			// If pixel is inside the triangle
			if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
				// Interpolate depth
				float z = w0 * v0.z + w1 * v1.z + w2 * v2.z;

				// Perspective-correct barycentrics
				float denom = (w0 / tri.depth0 + w1 / tri.depth1 + w2 / tri.depth2);
				float cw0 = (w0 / tri.depth0) / denom;
				float cw1 = (w1 / tri.depth1) / denom;
				float cw2 = (w2 / tri.depth2) / denom;

				// Interpolate attributes
				float u = cw0 * tri.uv0.x + cw1 * tri.uv1.x + cw2 * tri.uv2.x;
				float v = cw0 * tri.uv0.y + cw1 * tri.uv1.y + cw2 * tri.uv2.y;

				int index = y * WINDOW_WIDTH + x;
				if (z < zBuffer[index]) { // depth test
					zBuffer[index] = z;
					if (tri.hasUvs) {
						fragmentInput.worldPosition;  // TODO
						Vec3 pixelNormal = (n0 * cw0 + n1 * cw1 + n2 * cw2).normalized();
						fragmentInput.normal = pixelNormal;
						fragmentInput.uv = {u, v};
						fragmentInput.texture = texture;
						fragmentInput.color = tri.color;
						Color color = fragmentShader->run(fragmentInput);
						rgbBuffer[index] = color;
						
					} else if (tri.hasNormals) {
						// Interpolate normal (affine)
						/*Vec3 pixelNormal = (n0 * cw0 + n1 * cw1 + n2 * cw2).normalized();
						Vec3 lightDir = Vec3{ 0,1,0 }.normalized();
						float intensity = pixelNormal.dot(lightDir);
						intensity = std::clamp((intensity + 1) / 2, 0.0f, 1.0f);
						rgbBuffer[index] = Color{
							static_cast<unsigned char>(color.r * intensity),
							static_cast<unsigned char>(color.g * intensity),
							static_cast<unsigned char>(color.b * intensity),
							color.a
						};*/
						fragmentInput.worldPosition;  // TODO
						Vec3 pixelNormal = (n0 * cw0 + n1 * cw1 + n2 * cw2).normalized();
						fragmentInput.normal = pixelNormal;
						//fragmentInput.uv = { u, v };
						//fragmentInput.texture = texture;
						fragmentInput.color = tri.color;
						Color color = fragmentShader->run(fragmentInput);
						rgbBuffer[index] = color;
					}
					else {
						// Simple flat color (use vertex colors if desired)
						rgbBuffer[index] = tri.color;
					}
					

					// DEBUG draw a the edges of the triangle
					if (cw0 < 0.01f || cw1 < 0.01f || cw2 < 0.01f) {
						rgbBuffer[index] = DARKGREEN;
					}
				}
			}
		}
	}
}