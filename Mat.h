#pragma once

#include "Vec.h"

struct Mat4;  // forward declare
struct Mat3;  // forward declare

// This is a homogeneous matrix. It's like a 3d matrix but also has a w component.
// This means that we can do a bunch of cool stuff like translation with multiplication.
// We can do perspective divide using the build in w (assuming our perspective matrix)
// places the values correctly. This is column major and not row major so starting
// from 0 we go down the matrix going down the entire column. Then we go to the
// next column and repeat. So in our contiguous array index 1 would actually be
// going down the matrix rather than going to the right.
struct Mat4 {
	float m[16]; // column-major

	// An identity matrix is a matrix with 1's diagonally spread from top-left to
	// bottom-right. Any multiplication with an identity matrix results in no change.
	static Mat4 identity();

	// Multiply this matrix against a vector. Meaning all columns of the vertex
	// are multiplied by all the rows of the matrix for each output (for the same output column).
	// The result here would be a Vec4 with the transformation applied.
	Vec4 operator*(const Vec4 v) const;

	// A matrix multiplied by a matrix is another matrix. The logic is the same
	// as the Vec4 multiplication but we then repeat the previous multiplication
	// for every row in our bigger matrix to fill in the rows for our output matrix.
	Mat4 operator*(const Mat4& rhs) const;

	// Return a new matrix with the following x,y,z translation applied
	static Mat4 translation(float tx, float ty, float tz = 0.0f);

	// Return a new matrix with the following x,y,z rotation (radians) applied
	static Mat4 rotation(float angleX, float angleY, float angleZ);

	// Return a new matrix with the following x,y,z scale applied
	static Mat4 scale(float sx, float sy, float sz = 1.0f);

	// Return a new matrix with the translation, rotation, and scale combined.
	static Mat4 compose(float tx, float ty, float tz,
		float angleX, float angleY, float angleZ,
		float sx, float sy, float sz);

	// Return a perspective matrix
	static Mat4 perspective(float fovDeg, float aspect, float near, float far);

	// Helper function to generate and fps view. Likely not going to be used
	// since I would rather use the matrix approach.
	// Ref: https://www.3dgep.com/understanding-the-view-matrix/
	static Mat4 fpsView(Vec3 eye, float pitch, float yaw);

};

struct Mat3 {
	float m[9]; // column-major

	// An identity matrix is a matrix with 1's diagonally spread from top-left to
	// bottom-right. Any multiplication with an identity matrix results in no change.
	static Mat3 identity();

	// Multiply this matrix against a vector. Meaning all columns of the vertex
	// are multiplied by all the rows of the matrix for each output (for the same output column).
	// The result here would be a Vec4 with the transformation applied.
	Vec3 operator*(const Vec3 v) const;

	// A matrix multiplied by a matrix is another matrix. The logic is the same
	// as the Vec4 multiplication but we then repeat the previous multiplication
	// for every row in our bigger matrix to fill in the rows for our output matrix.
	Mat3 operator*(const Mat3& rhs) const;

	// Return a new matrix with the following x,y,z translation applied
	static Mat3 translation(float tx, float ty, float tz = 0.0f);

	// Return a new matrix with the following x,y,z rotation (radians) applied
	static Mat3 rotation(float angleX, float angleY, float angleZ);

	// Return a new matrix with the following x,y,z scale applied
	static Mat3 scale(float sx, float sy, float sz = 1.0f);

	// Return a new matrix with the translation, rotation, and scale combined.
	static Mat3 compose(
		float angleX, float angleY, float angleZ,
		float sx, float sy, float sz);

	Mat4 toMat4() const;
};