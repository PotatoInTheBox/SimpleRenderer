#pragma once

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

	Vec4();
	Vec4(float _x, float _y, float _z, float _w);
	Vec4(const Vec3& v, float _w);

	// Add two Vec4 together
	Vec4 operator+(const Vec4 rhs) const;
	Vec4& operator+=(const Vec4 rhs);
	// Subtract the left Vec4 by the right Vec4
	Vec4 operator-(const Vec4 rhs) const;
	Vec4& operator-=(const Vec4 rhs);
	// Scale the `x, y, z, w` Vec4 value by a given scale factor
	Vec4 operator*(float s) const;
	Vec4& operator*=(float s);
	// Multiply the x's, multiply the y's, and sum them together to get out
	// dot product value. Which effectively tells us how much two vectors are
	// facing the same direction. (if there is a z then do it for the z as well.)
	float dot(const Vec4 rhs) const;
	// Shorthand operator for .dot()
	float operator*(const Vec4 rhs) const;
	// Multiply the first x with the second y, and the first y with the second x.
	// Which will give us a perpendicular vector to this one. (if there is a z then
	// do it for the z as well.)
	Vec4 cross(const Vec4 rhs) const;
	// Shorthand operator for .cross()
	Vec4 operator^(const Vec4 rhs) const;
	// division
	Vec4 operator/(float s) const;
	// Convert this vector to a Vec3 by cutting off the w.
	Vec3 toVec3() const;
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