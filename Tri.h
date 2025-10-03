#pragma once
#include "raylib.h"
#include <vector>
#include "Vec.h"
#include "Math.h"
#include "Shader.h"
#include "Vertex.h"

// As a reminder these are CCW
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
	// As a reminder these are CCW
	VertexOutput vertexOutput[3];  // 3 vertices after vertex shader
};

// TODO probably just move this over to a regular triangle...
struct ClippedTriangle {
	// As a reminder these are CCW
	ClippedVertex clippedVertex[3];
	Color color;  // not sure if I need it yet

	ClippedVertex ComputeIntersection(const ClippedVertex& prev, const ClippedVertex& curr, int edge) {
		constexpr float GUARD_BAND_XY = 5.0f; // allow vertices up to 5*w outside
		constexpr float EPS_Z = 1e-5f; // small epsilon for near/far planes

		auto planeEval = [&](const Vec4& v, int edge) {
			switch (edge) {
			case 0: return v.z + v.w + EPS_Z;      // near
			case 1: return v.w - v.z + EPS_Z;      // far
			case 2: return v.x + v.w * GUARD_BAND_XY;     // left
			case 3: return v.w * GUARD_BAND_XY - v.x;     // right
			case 4: return v.w * GUARD_BAND_XY - v.y;     // top
			case 5: return v.y + v.w * GUARD_BAND_XY;     // bottom
			default: return 0.0f;
			}
		};

		float f0 = planeEval(prev.clipPosition, edge);
		float f1 = planeEval(curr.clipPosition, edge);
		float t = f0 / (f0 - f1);
		return prev.lerp(curr, t);
	}

	// Check if a vertex is inside the frustum with guard band
	static bool isInside(const ClippedVertex& v, int edge) {
		constexpr float GUARD_BAND_XY = 5.0f;
		constexpr float EPS_Z = 1e-2f;

		switch (edge) {
		case 0: return v.clipPosition.z >= -v.clipPosition.w + EPS_Z;
		case 1: return v.clipPosition.z <= v.clipPosition.w - EPS_Z;
		case 2: return v.clipPosition.x >= -v.clipPosition.w * GUARD_BAND_XY;
		case 3: return v.clipPosition.x <= v.clipPosition.w * GUARD_BAND_XY;
		case 4: return v.clipPosition.y <= v.clipPosition.w * GUARD_BAND_XY;
		case 5: return v.clipPosition.y >= -v.clipPosition.w * GUARD_BAND_XY;
		}
		return true;
	}


	// So the strategy is to generate an n-gon polygon by adding a new vertex
	// every single time we intersect the cube. Then we simply "triangulate fan"
	// the polygon into a bunch of triangles which we can render.
	// At most this will take 5 triangles I think (TODO prove this).
	// So we can simply repeat the process on 6 planes (8 if we do z which we will)
	// 
	std::vector<ClippedTriangle> clipToFrustrum() {
		std::vector<ClippedVertex> buffers[2];
		buffers[0].assign(clippedVertex, clippedVertex + 3);
		size_t cur = 0;

		for (int edge = 0; edge < 6; ++edge) {
			auto& input = buffers[cur];
			auto& output = buffers[1 - cur];
			output.clear();

			bool allInside = true;
			for (auto& v : input) if (!isInside(v, edge)) { allInside = false; break; }
			if (allInside) continue; // skip plane if fully inside

			size_t n = input.size();
			for (size_t i = 0; i < n; ++i) {
				ClippedVertex currV = input[i];
				ClippedVertex prevV = input[(i + n - 1) % n];
				bool currInside = isInside(currV, edge);
				bool prevInsideV = isInside(prevV, edge);

				if (currInside) {
					if (!prevInsideV) output.push_back(ComputeIntersection(prevV, currV, edge));
					output.push_back(currV);
				}
				else if (prevInsideV) {
					output.push_back(ComputeIntersection(prevV, currV, edge));
				}
			}
			cur = 1 - cur; // swap buffers
		}

		std::vector<ClippedTriangle> result;
		auto& finalList = buffers[cur];
		if (finalList.size() < 3) return result;

		for (size_t i = 1; i + 1 < finalList.size(); ++i) {
			ClippedTriangle t;
			t.color = color;
			t.clippedVertex[0] = finalList[0];
			t.clippedVertex[1] = finalList[i];
			t.clippedVertex[2] = finalList[i + 1];
			result.push_back(t);
		}

		return result;
		//// make sure we have adequate space to store our polygons
		//std::vector<ClippedVertex> poly;

		//// load our current vertices
		////for (int i = 0; i < 3; i++) poly[i] = tri.clippedVertex[i];
		//for (ClippedVertex& v : clippedVertex) {
		//	poly.push_back(v);
		//}

		//std::vector<ClippedVertex> outputList = poly;
		//

	 //   // https://en.wikipedia.org/wiki/Sutherland%E2%80%93Hodgman_algorithm
		//for (size_t edge = 0; edge < 6; edge++) {
		//	std::vector<ClippedVertex> inputList = outputList;
		//	outputList.clear();

		//	for (int i = 0; i < inputList.size(); i += 1) {
		//		ClippedVertex current_point = inputList[i];
		//		int prevIndex = (i + inputList.size() - 1) % inputList.size();
		//		ClippedVertex prev_point = inputList[prevIndex];
		//		ClippedVertex Intersecting_point = ComputeIntersection(prev_point, current_point, edge);

		//		if (isInside(current_point, edge)) {
		//			if (!isInside(prev_point, edge)) {
		//				outputList.push_back(Intersecting_point);
		//			}
		//			outputList.push_back(current_point);
		//		}
		//		else if (isInside(prev_point, edge)) {
		//			outputList.push_back(Intersecting_point);
		//		}
		//	}
		//}

		//std::vector<ClippedTriangle> result;
		//if (outputList.size() < 3) return result; // fully clipped

		//for (size_t i = 1; i + 1 < outputList.size(); ++i) {
		//	ClippedTriangle t;
		//	t.color = color;

		//	t.clippedVertex[0] = outputList[0];
		//	t.clippedVertex[1] = outputList[i];
		//	t.clippedVertex[2] = outputList[i + 1];

		//	result.push_back(t);
		//}

		//return result;
	}

	std::array<ClippedTriangle, 2> clipZOne(int clipdIdx1, int clipIdx2, float nearClipDistance) {
		int indexClip = clipdIdx1 ? 0 : clipIdx2 ? 1 : 2;
		int indexNext = (indexClip + 1) % 3;
		int indexPrev = (indexClip - 1 + 3) % 3;

		Vec4 pointClipped = clippedVertex[indexClip].clipPosition;
		Vec4 pointA = clippedVertex[indexNext].clipPosition;
		Vec4 pointB = clippedVertex[indexPrev].clipPosition;

		// Fraction along triangle edge at which the depth is equal to the clip distance
		//float fracA = (nearClipDistance - pointClipped.z) / (pointA.z - pointClipped.z);
		//float fracB = (nearClipDistance - pointClipped.z) / (pointB.z - pointClipped.z);

		// ------------------------------------------------------------
		// Compute "special z" for each edge (clipped->A, clipped->B).
		// ------------------------------------------------------------
		auto computeZSpecial = [](const Vec4& pOut, const Vec4& pIn) {
			float tX = (fabs(pOut.x) > pOut.w) ? pOut.w / fabs(pOut.x) : 1.0f;
			float tY = (fabs(pOut.y) > pOut.w) ? pOut.w / fabs(pOut.y) : 1.0f;
			float t = std::max(tX, tY);
			return pOut.z * t / 4.0f; // hack factor
			};

		float zSpecialA = computeZSpecial(pointClipped, pointA);
		float zSpecialB = computeZSpecial(pointClipped, pointB);

		// ------------------------------------------------------------
		// Interpolate new vertices at this special depth
		// ------------------------------------------------------------
		float fracA = (zSpecialA - pointClipped.z) / (pointA.z - pointClipped.z);
		float fracB = (zSpecialB - pointClipped.z) / (pointB.z - pointClipped.z);

		float fracAtest = (nearClipDistance - pointClipped.z) / (pointA.z - pointClipped.z);
		float fracBtest = (nearClipDistance - pointClipped.z) / (pointB.z - pointClipped.z);

		// New triangle points (in view space)
		ClippedVertex clippedVertexA = clippedVertex[indexClip].lerp(clippedVertex[indexNext], fracA);
		ClippedVertex clippedVertexB = clippedVertex[indexClip].lerp(clippedVertex[indexPrev], fracB);

		// Triangle A is causing me issues for some reason.
		// When I get far away (100 units or more)
		// then this specific new triangle bugs out completely.
		// It's almost as if it's writing an incorrect z value.
		ClippedTriangle newTriangleA;
		newTriangleA.color = color;
		newTriangleA.clippedVertex[indexClip] = clippedVertexA;
		newTriangleA.clippedVertex[indexNext] = clippedVertex[indexNext];
		newTriangleA.clippedVertex[indexPrev] = clippedVertex[indexPrev];
		newTriangleA.color = BLUE;

		ClippedTriangle newTriangleB;
		newTriangleB.color = color;
		newTriangleB.clippedVertex[indexClip] = clippedVertexB;
		newTriangleB.clippedVertex[indexNext] = clippedVertexA;
		newTriangleB.clippedVertex[indexPrev] = clippedVertex[indexPrev];
		newTriangleB.color = YELLOW;
		return { newTriangleA, newTriangleB };
	}

	ClippedTriangle clipZTwo(int clipdIdx1, int clipIdx2, float nearClipDistance) const {
		int indexNonClip = !clipdIdx1 ? 0 : !clipIdx2 ? 1 : 2;
		int indexNext = (indexNonClip + 1) % 3;
		int indexPrev = (indexNonClip - 1 + 3) % 3;

		Vec4 pointNotClipped = clippedVertex[indexNonClip].clipPosition;
		Vec4 pointA = clippedVertex[indexNext].clipPosition;
		Vec4 pointB = clippedVertex[indexPrev].clipPosition;

		// Am I currently clipping outside of the view frustrum?

		// Answer. Yes I am, if x or y is bigger than w then it is outside the frustrum.

		// Solution. How hard would it be to clip to the frustrum instead?

		// It would require me to construct a new triangle.

		// Maybe I can clip further from 0 while still rendering correctly?

		// 

		// Fraction along triangle edge at which the depth is equal to the clip distance
		//float fracA = (nearClipDistance - pointNotClipped.z) / (pointA.z - pointNotClipped.z);
		//float fracB = (nearClipDistance - pointNotClipped.z) / (pointB.z - pointNotClipped.z);

		// ------------------------------------------------------------
		// Instead of always clipping to nearClipDistance,
		// compute a "special z" where x or y would hit w.
		// ------------------------------------------------------------
		float tX = (fabs(pointNotClipped.x) > pointNotClipped.w)
			? pointNotClipped.w / fabs(pointNotClipped.x)
			: 1.0f;

		float tY = (fabs(pointNotClipped.y) > pointNotClipped.w)
			? pointNotClipped.w / fabs(pointNotClipped.y)
			: 1.0f;

		float t = std::max(tX, tY);

		// Special z: push point toward the frustum edge instead of near clip
		float zSpecial = pointNotClipped.z * t / 4;

		// ------------------------------------------------------------
		// Interpolate new vertices at this special depth
		// ------------------------------------------------------------
		float fracA = (zSpecial - pointNotClipped.z) / (pointA.z - pointNotClipped.z);
		float fracB = (zSpecial - pointNotClipped.z) / (pointB.z - pointNotClipped.z);

		

		ClippedTriangle newTriangle;
		newTriangle.color = color;
		ClippedVertex clippedVertexA = clippedVertex[indexNonClip].lerp(clippedVertex[indexNext], fracA);
		ClippedVertex clippedVertexB = clippedVertex[indexNonClip].lerp(clippedVertex[indexPrev], fracB);
		newTriangle.clippedVertex[indexNonClip] = clippedVertex[indexNonClip];
		newTriangle.clippedVertex[indexNext] = clippedVertexA;
		newTriangle.clippedVertex[indexPrev] = clippedVertexB;
		newTriangle.color = RED;
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