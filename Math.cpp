#include "Math.h"
#include <cmath>

Rect Tri::boundingBox(int screenWidth, int screenHeight) const {
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

