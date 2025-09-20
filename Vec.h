#pragma once

#include <math.h>

// All Vec2 Vec3 Vec4 

struct Vec4;  // forward declare
struct Vec3;  // forward declare
struct Vec2;  // forward declare



// This is obviously a vector with 4 positions (x, y, z, w).
// with w being our perspective divide for x, y, and z
// We want them as a vec4 because they need to be a homogeneous coordinate.
// This means that it has an additional coordinate that can help us with  
// translation (which is only possible with addition and not multiplication if using vec3)
// and it can help us with perspective divide.
// An operation return a new Vec4 which makes sense because we are dealing with
// 16 bytes (easily fits within our stack/registers)
struct Vec4 {
	float x, y, z, w;

	// ======== Constructors ========

	// default constructor (all zeroes including w)
	Vec4();
	// Constructor from individual floats
	Vec4(float _x, float _y, float _z, float _w);
	// Constructor from Vec3 + w
	Vec4(const Vec3& v, float _w);

	// ======== Arithmetic ========

	// Add two Vec4 together
	Vec4 operator+(const Vec4 rhs) const { return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w }; };
	// add a Vec4 to another Vec4
	Vec4& operator+=(const Vec4 rhs) { 
		x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w;
		return *this;
	};
	// Subtract the left Vec4 by the right Vec4
	Vec4 operator-(const Vec4 rhs) const { return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w }; };
	// Subtract the left Vec4 by the right Vec4 and store it back
	Vec4& operator-=(const Vec4 rhs) {
		x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w;
		return *this;
	};
	// Scale the `x, y, z, w` Vec4 value by a given scale factor
	Vec4 operator*(float s) const { return { x * s, y * s, z * s, w * s }; };
	// Scale the `x, y, z, w` Vec4 value by a given scale factor and store it back
	Vec4& operator*=(float s) {
		x *= s; y *= s; z *= s; w *= s;
		return *this;
	};
	// division
	Vec4 operator/(float s) const { return { x / s, y / s, z / s, w / s }; };
	Vec4& operator/=(float s) {
		x /= s; y /= s; z /= s; w /= s;
		return *this;
	};
	// unary
	Vec4 operator-() const { return { -x, -y, -z, w}; };

	// ======== Dot ========

	// Multiply the x's, multiply the y's, and sum them together to get out
	// dot product value. Which effectively tells us how much two vectors are
	// facing the same direction. (if there is a z then do it for the z as well.)
	float dot(const Vec4 rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }; // ignore w
	// Shorthand operator for .dot()
	float operator*(const Vec4 rhs) const { return this->dot(rhs); };

	// ======== Cross ========

	// Multiply the first x with the second y, and the first y with the second x.
	// Which will give us a perpendicular vector to this one. (if there is a z then
	// do it for the z as well.)
	Vec4 cross(const Vec4 rhs) const {
		return {
			y * rhs.z - z * rhs.y,
			z * rhs.x - x * rhs.z,
			x * rhs.y - y * rhs.x,
			0.0f  // w = 0 for direction
		};
	};
	// Shorthand operator for .cross()
	Vec4 operator^(const Vec4 rhs) const { return this->cross(rhs); };
	
	// ======== Length ========

	// length function (ignoring w)
	float length() const { return sqrtf(x * x + y * y + z * z); };
	// normalize funciton (ignoring w)
	Vec4 normalized() const { return *this / length(); };

	// ======== Lerp ========

	// linearly interpolate between two vectors and a given fraction to create a new vector
	Vec4 lerp(const Vec4 rhs, float t) const {
		// TODO Do not bother clamping yet
		return *this + (rhs - *this) * t;
	};

	// ======== Dimension ========
	
	// Convert this vector to a Vec3 by cutting off the w.
	Vec3 toVec3() const;
	// Convert this vector to a Vec3 by cutting off the z and w.
	Vec2 toVec2() const;
};

// x, y, z positions
struct Vec3 {
	float x, y, z;
	Vec3();
	Vec3(float _x, float _y, float _z);
	Vec3(float _x, float _y);
	Vec3(const Vec2& v, float _z);
	Vec3 operator+(const Vec3 rhs) const;
	Vec3& operator+=(const Vec3 rhs);
	Vec3 operator-(const Vec3 rhs) const;
	Vec3& operator-=(const Vec3 rhs);
	Vec3 operator*(float s) const;
	Vec3& operator*=(float s);
	float dot(const Vec3 rhs) const;
	float operator*(const Vec3 rhs) const;
	Vec3 cross(const Vec3 rhs) const;
	Vec3 operator^(const Vec3 rhs) const;
	Vec3 normalized() const;
	// Convert this vector to a Vec4 by adding a w with a value of 1.0f
	Vec4 toVec4() const;
};

// x, y positions
struct Vec2 {
	float x, y;
	Vec2 operator+(const Vec2 rhs) const;
	Vec2& operator+=(const Vec2 rhs);
	Vec2 operator-(const Vec2 rhs) const;
	Vec2& operator-=(const Vec2 rhs);
	Vec2 operator*(float s) const;
	Vec2& operator*=(float s);
	float dot(const Vec2 rhs) const;
	float operator*(const Vec2 rhs) const;
	Vec3 cross(const Vec2 rhs) const;
	Vec3 operator^(const Vec2 rhs) const;
};