#pragma once

#include <vector>
#include "Vec.h"

struct ModelVertices {
	bool hasNormals = true;
	bool hasUvs = true;
	std::vector<Vec3> positions;  // vertex positions
	std::vector<Vec3> normals;    // vertex normals (used for lighting/shading)
	// per-vertex color (can be used for shading, debugging, or final color interpolation)
	//std::vector<Vec4> colors;
	std::vector<Vec2> uvs;        // texture coordinates (for sampling textures)
};

// Represents a vertex that will get modified throughout the pipeline.
// Most critically it contains a position but it also contains other data
// types that are part of this vertex.
struct VertexBuffers {
	std::vector<Vec4> clipSpacePositions; // similar to gl_Position
	std::vector<Vec3> worldSpaceNormals; // relative to world space
	std::vector<Vec3> screenSpacePositions; // relative to the screen
	// per-vertex color (can be used for shading, debugging, or final color interpolation)
	//std::vector<Vec4> colors;
	//std::vector<Vec2> uvs;        // texture coordinates (for sampling textures)
	// do I need it???  optional z-value per vertex; usually not needed unless
	// you want a vertex-based depth buffer
	std::vector<float> depths;
};

struct ClippedVertex {
	Vec3 position;  // useful for depth
	Vec4 clipPosition;   // required for rasterization
	Vec3 viewPosition;   // only for perspective correction
	Vec3 worldPosition;  // useful for lighting
	Vec3 worldNormal;         // in world space
	Vec2 uv;             // after vertex shader

	// create a new clipped vertex between this one and another one
	ClippedVertex lerp(ClippedVertex other, float fraction) const {
		ClippedVertex v;
		v.position = position.lerp(other.position, fraction);
		v.clipPosition = clipPosition.lerp(other.clipPosition, fraction);
		v.viewPosition = viewPosition.lerp(other.viewPosition, fraction);
		v.worldPosition = worldPosition.lerp(other.worldPosition, fraction);
		v.worldNormal = worldNormal.lerp(other.worldNormal, fraction);
		v.uv = uv.lerp(other.uv, fraction);
		return v;
	}
};