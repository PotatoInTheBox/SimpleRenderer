#pragma once
#include <vector>
#include "Vec.h"

// A single triangle, storing indices into the vertex buffer
struct TriIdx {
	std::array<uint32_t, 3> verts;

	// convenience accessors
	uint32_t& operator[](size_t i) { return verts[i]; }
	const uint32_t& operator[](size_t i) const { return verts[i]; }
};

// A buffer of triangles
struct TriIdxBuffer {
	std::vector<TriIdx> tris;

	void add(uint32_t v0, uint32_t v1, uint32_t v2) {
		tris.push_back({ v0, v1, v2 });
	}

	size_t size() const { return tris.size(); }
};