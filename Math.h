#pragma once
#define PI 3.14159265358979323846f
#include "raylib.h"
#include "Vec.h"


struct Rect {
	int xMin, yMin;
	int xMax, yMax;
};


struct Tri {
	Vec3 v0, v1, v2;      // triangle vertices in screen space (x, y, z)
	//Vec4 color0, color1, color2; // optional vertex colors
	//Vec3 normal;          // optional flat normal
	//Vec2 uv0, uv1, uv2;   // optional texture coordinates
	Color color; // Flat color for this tri
	Rect boundingBox(int screenWidth, int screenHeight) const;
};

// Convert degrees to radians
inline float deg2rad(float degrees) {
	return degrees * (PI / 180.0f);
}

// Convert radians to degrees
inline float rad2deg(float radians) {
	return radians * (180.0f / PI);
}
