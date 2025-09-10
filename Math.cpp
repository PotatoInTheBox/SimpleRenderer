#include "Math.h"


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
    result.m[0] = result.m[5] = result.m[10] = result.m[15] = 1.0f;
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
