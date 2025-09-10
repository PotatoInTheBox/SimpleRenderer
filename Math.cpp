#include "Math.h"
#include <cmath>

// Make indexing easier by defining a macro in which we can specify the column
// and row of the matrix.
#define MAT4_INDEX(c, r) ((c) * 4 + (r))

Vec4 Vec4::operator+(const Vec4 rhs) const {
	// I tested it out with Vec4 and Vec4&.
	// The compiler does not give a fuck about pass by reference/value and will
	// just do whatever is faster. This means I get to choose for debugging and
	// writing. The optimized output will be the same regardless.
    return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
}

Vec4 Vec4::operator-(const Vec4 rhs) const {
    return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
}

Vec4 Vec4::operator*(float s) const {
    return { x * s, y * s, z * s, w * s };
}

float Vec4::dot(const Vec4 rhs) const {
	return x * rhs.x + y * rhs.y + z * rhs.z; // ignore w
}

float Vec4::operator*(const Vec4 rhs) const {
	return x * rhs.x + y * rhs.y + z * rhs.z; // ignore w
}

Vec4 Vec4::cross(const Vec4 rhs) const {
	return {
		y * rhs.z - z * rhs.y,
		z * rhs.x - x * rhs.z,
		x * rhs.y - y * rhs.x,
		0.0f  // w = 0 for direction
	};
}

Vec4 Vec4::operator^(const Vec4 rhs) const {
	return {
		y * rhs.z - z * rhs.y,
		z * rhs.x - x * rhs.z,
		x * rhs.y - y * rhs.x,
		0.0f  // w = 0 for direction
	};
}

Mat4 Mat4::identity() {
    Mat4 result{};
    /*result.m[0] = result.m[5] = result.m[10] = result.m[15] = 1.0f;*/
	result.m[MAT4_INDEX(0, 0)] = 1.0f;
	result.m[MAT4_INDEX(1, 1)] = 1.0f;
	result.m[MAT4_INDEX(2, 2)] = 1.0f;
	result.m[MAT4_INDEX(3, 3)] = 1.0f;
    return result;
}

Vec4 Mat4::operator*(const Vec4 v) const {
    return {
        m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12] * v.w,
        m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13] * v.w,
        m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w,
        m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w
    };
	// NOTE: if this is for the perspective divide then we can make a slight optimization
	// by setting our w output to 1. Thus reducing 25% of the work.
}

Mat4 Mat4::operator*(const Mat4& rhs) const {
    Mat4 out{};
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            out.m[col * 4 + row] =
                m[0 * 4 + row] * rhs.m[col * 4 + 0] +
                m[1 * 4 + row] * rhs.m[col * 4 + 1] +
                m[2 * 4 + row] * rhs.m[col * 4 + 2] +
                m[3 * 4 + row] * rhs.m[col * 4 + 3];
        }
    }
	// NOTE: This can probably be optimized for various situations. Such as 0 being
	// the value in parts of the matrix. This varies a lot so I'm hoping the compiler
	// can optimize that for us (for example if the compiler were to inline it may
	// become evident that a value will always have 0 leading to 0 always being the output).
    return out;
}

Mat4 Mat4::translation(float tx, float ty, float tz) {
	Mat4 result = Mat4::identity();
	/*result.m[12] = tx;
	result.m[13] = ty;
	result.m[14] = tz;*/
	result.m[MAT4_INDEX(3, 0)] = tx;
	result.m[MAT4_INDEX(3, 1)] = ty;
	result.m[MAT4_INDEX(3, 2)] = tz;
	return result;
}

Mat4 Mat4::scale(float sx, float sy, float sz) {
	Mat4 result = Mat4::identity();
	/*result.m[0] = sx;
	result.m[5] = sy;
	result.m[10] = sz;*/
	result.m[MAT4_INDEX(0, 0)] = sx;
	result.m[MAT4_INDEX(1, 1)] = sy;
	result.m[MAT4_INDEX(2, 2)] = sz;
	return result;
}

Mat4 Mat4::rotation(float angleX, float angleY, float angleZ) {
	float cx = cosf(angleX), sx = sinf(angleX);
	float cy = cosf(angleY), sy = sinf(angleY);
	float cz = cosf(angleZ), sz = sinf(angleZ);

	Mat4 rx = Mat4::identity();
	rx.m[5] = cx; rx.m[6] = -sx;
	rx.m[9] = sx; rx.m[10] = cx;

	Mat4 ry = Mat4::identity();
	ry.m[0] = cy; ry.m[2] = sy;
	ry.m[8] = -sy; ry.m[10] = cy;

	Mat4 rz = Mat4::identity();
	rz.m[0] = cz; rz.m[1] = -sz;
	rz.m[4] = sz; rz.m[5] = cz;

	// Combined rotation: Rz * Ry * Rx
	// I'm sure the compiler will optimize this... right?
	// NOTE: This may not be optimal since it makes quite a few identity matracies
	return rz * ry * rx;
}

Mat4 Mat4::compose(float tx, float ty, float tz,
	float angleX, float angleY, float angleZ, 
	float sx, float sy, float sz)
{
	Mat4 T = Mat4::translation(tx, ty, tz);
	Mat4 R = Mat4::rotation(angleX, angleY, angleZ);
	Mat4 S = Mat4::scale(sx, sy, sz);
	return T * R * S; // scale first, rotate second, translate last
}

Mat4 Mat4::perspective(float fovDeg, float aspect, float near, float far) {
	float fovRad = deg2rad(fovDeg);
	float f = 1.0f / std::tan(fovRad / 2.0f); // cotangent

	Mat4 result{};
	result.m[MAT4_INDEX(0, 0)] = f / aspect; // column 0, row 0
	result.m[MAT4_INDEX(1, 1)] = f;           // column 1, row 1
	result.m[MAT4_INDEX(2, 2)] = (far + near) / (near - far);   // column 2, row 2
	result.m[MAT4_INDEX(2, 3)] = -1.0f;                          // column 2, row 3
	result.m[MAT4_INDEX(3, 2)] = (2.0f * far * near) / (near - far); // column 3, row 2
	// all other entries are 0 by default
	return result;
}
