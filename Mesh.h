#pragma once
#include <vector>
#include "Vertex.h"
#include "Tri.h"

// Represents the triangles that make up a 3d object.
// TODO: could possibly hold other information such as the material... idk yet
struct MyMesh {
	VertexBuffers vertices;
	TriIdxBuffer triangles;

	MyMesh() = default;
	MyMesh(const VertexBuffers& vb, const TriIdxBuffer& tris)
		: vertices(vb), triangles(tris) {
	}
};