#pragma once
#include "raylib.h"
#include <vector>
#include "Vec.h"
#include "Math.h"
#include "Shader.h"

struct Tri {
	Vec3 v0;
	Vec3 v1;
	Vec3 v2;
	Vec3 n0;
	Vec3 n1;
	Vec3 n2;
	Vec2 uv0;
	Vec2 uv1;
	Vec2 uv2;
	Color color;
	bool hasNormals = false;

	Rect boundingBox(int screenWidth, int screenHeight) const {
		float minX = fminf(fminf(v0.x, v1.x), v2.x);
		float minY = fminf(fminf(v0.y, v1.y), v2.y);
		float maxX = fmaxf(fmaxf(v0.x, v1.x), v2.x);
		float maxY = fmaxf(fmaxf(v0.y, v1.y), v2.y);

		// Clamp to screen bounds
		minX = fmaxf(0.0f, minX);
		minY = fmaxf(0.0f, minY);
		maxX = fminf(float(screenWidth - 1), maxX);
		maxY = fminf(float(screenHeight - 1), maxY);

		return Rect{ int(minX), int(minY), int(maxX), int(maxY) };
	}
};

// this is the working triangle that will be used to process a triangle per face
struct ProcessedTriangle {
	VertexOutput vertexOutput[3];  // 3 vertices after vertex shader
};

// A single triangle, storing indices into the vertex buffer
struct TriIdx {
	std::array<uint32_t, 3> verts;
	std::array<uint32_t, 3> normalIdx;
	std::array<uint32_t, 3> uvIdx;

	// convenience accessors
	uint32_t& operator[](size_t i) { return verts[i]; }
	const uint32_t& operator[](size_t i) const { return verts[i]; }
};

struct ModelTriangles {
	std::vector<TriIdx> tris;
};

// A buffer of triangles
struct TriIdxBuffer {
	//std::vector<TriIdx> vertexTris;
	std::vector<uint32_t> clippedTris;
	std::vector<uint32_t> culledTris;
	
};