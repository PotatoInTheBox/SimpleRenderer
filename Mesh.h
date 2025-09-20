#pragma once
#include <vector>
#include "Vertex.h"
#include "Tri.h"

// Represents the triangles that make up a 3d object.
// TODO: could possibly hold other information such as the material... idk yet
struct MyMesh {
	ModelVertices vertices;
	ModelTriangles triangles;

	MyMesh() = default;
	MyMesh(const ModelVertices& vb, const ModelTriangles& vertexTris)
		: vertices(vb), triangles(vertexTris) {
	}
};