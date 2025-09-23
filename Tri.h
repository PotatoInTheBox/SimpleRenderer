#pragma once
#include "raylib.h"
#include <vector>
#include "Vec.h"
#include "Math.h"
#include "Shader.h"
#include "Vertex.h"

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
	float depth0;
	float depth1;
	float depth2;
	Color color;
	bool hasNormals = false;
	bool hasUvs = false;

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

struct ClippedTriangle {
	ClippedVertex clippedVertex[3];
	Color color;  // not sure if I need it yet

	std::array<ClippedTriangle, 2> clipZOne(int clipdIdx1, int clipIdx2, float nearClipDistance) {
		int indexClip = clipdIdx1 ? 0 : clipIdx2 ? 1 : 2;
		int indexNext = (indexClip + 1) % 3;
		int indexPrev = (indexClip - 1 + 3) % 3;

		Vec4 pointClipped = clippedVertex[indexClip].clipPosition;
		Vec4 pointA = clippedVertex[indexNext].clipPosition;
		Vec4 pointB = clippedVertex[indexPrev].clipPosition;

		// Fraction along triangle edge at which the depth is equal to the clip distance
		float fracA = (nearClipDistance - pointClipped.z) / (pointA.z - pointClipped.z);
		float fracB = (nearClipDistance - pointClipped.z) / (pointB.z - pointClipped.z);

		// New triangle points (in view space)
		ClippedVertex clippedVertexA = clippedVertex[indexClip].lerp(clippedVertex[indexNext], fracA);
		ClippedVertex clippedVertexB = clippedVertex[indexClip].lerp(clippedVertex[indexPrev], fracB);

		ClippedTriangle newTriangleA;
		newTriangleA.color = color;
		newTriangleA.clippedVertex[indexClip] = clippedVertexA;
		newTriangleA.clippedVertex[indexNext] = clippedVertex[indexNext];
		newTriangleA.clippedVertex[indexPrev] = clippedVertex[indexPrev];

		ClippedTriangle newTriangleB;
		newTriangleB.color = color;
		newTriangleB.clippedVertex[indexClip] = clippedVertexB;
		newTriangleB.clippedVertex[indexNext] = clippedVertexA;
		newTriangleB.clippedVertex[indexPrev] = clippedVertex[indexPrev];

		return { newTriangleA, newTriangleB };
	}

	ClippedTriangle clipZTwo(int clipdIdx1, int clipIdx2, float nearClipDistance) const {
		int indexNonClip = !clipdIdx1 ? 0 : !clipIdx2 ? 1 : 2;
		int indexNext = (indexNonClip + 1) % 3;
		int indexPrev = (indexNonClip - 1 + 3) % 3;

		Vec4 pointNotClipped = clippedVertex[indexNonClip].clipPosition;
		Vec4 pointA = clippedVertex[indexNext].clipPosition;
		Vec4 pointB = clippedVertex[indexPrev].clipPosition;

		// Fraction along triangle edge at which the depth is equal to the clip distance
		float fracA = (nearClipDistance - pointNotClipped.z) / (pointA.z - pointNotClipped.z);
		float fracB = (nearClipDistance - pointNotClipped.z) / (pointB.z - pointNotClipped.z);

		ClippedTriangle newTriangle;
		newTriangle.color = color;
		ClippedVertex clippedVertexA = clippedVertex[indexNonClip].lerp(clippedVertex[indexNext], fracA);
		ClippedVertex clippedVertexB = clippedVertex[indexNonClip].lerp(clippedVertex[indexPrev], fracB);
		newTriangle.clippedVertex[indexNonClip] = clippedVertex[indexNonClip];
		newTriangle.clippedVertex[indexNext] = clippedVertexA;
		newTriangle.clippedVertex[indexPrev] = clippedVertexB;
		return newTriangle;
	}
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