#pragma once

#include <vector>
#include "Vec.h"
#include "Tri.h"

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