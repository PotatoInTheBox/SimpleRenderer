#include "Vec.h"
#include <cmath>

// default constructor
Vec4::Vec4() : x(0), y(0), z(0), w(0) {}

// Constructor from individual floats
Vec4::Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}

// Constructor from Vec3 + w
Vec4::Vec4(const Vec3& v, float _w) : x(v.x), y(v.y), z(v.z), w(_w) {}

Vec3 Vec4::toVec3() const {
	return Vec3{ x, y, z };
}

Vec2 Vec4::toVec2() const {
	return Vec2{ x, y };
}

// default constructor
Vec3::Vec3() : x(0), y(0), z(0) {}

// Constructor from individual floats
Vec3::Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

Vec3::Vec3(float _x, float _y) : x(_x), y(_y) { z = 0.0f; }

// Constructor from Vec2 + z
Vec3::Vec3(const Vec2& v, float _z) : x(v.x), y(v.y), z(_z) {}

Vec3 Vec3::operator+(const Vec3 rhs) const { return { x + rhs.x, y + rhs.y, z + rhs.z }; }
Vec3& Vec3::operator+=(const Vec3 rhs) {
	x += rhs.x; y += rhs.y; z += rhs.z;
	return *this;
}
Vec3 Vec3::operator-(const Vec3 rhs) const { return { x - rhs.x, y - rhs.y, z - rhs.z }; }
Vec3& Vec3::operator-=(const Vec3 rhs) {
	x -= rhs.x; y -= rhs.y; z -= rhs.z;
	return *this;
}
Vec3 Vec3::operator*(float s) const { return { x * s, y * s, z * s }; }
Vec3& Vec3::operator*=(float s) {
	x *= s; y *= s; z *= s;
	return *this;
}
float Vec3::dot(const Vec3 rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
float Vec3::operator*(const Vec3 rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
Vec3 Vec3::cross(const Vec3 rhs) const {
	return {
		y * rhs.z - z * rhs.y,
		z * rhs.x - x * rhs.z,
		x * rhs.y - y * rhs.x };
}
Vec3 Vec3::operator^(const Vec3 rhs) const {
	return {
		y * rhs.z - z * rhs.y,
		z * rhs.x - x * rhs.z,
		x * rhs.y - y * rhs.x
	};
}

Vec3 Vec3::normalized() const {
	float len = std::sqrt(x * x + y * y + z * z);
	if (len > 0.0f) {
		return { x / len, y / len, z / len };
	}
	return { 0.0f, 0.0f, 0.0f }; // safe fallback for zero vector
}

// ======== Lerp ========

	// linearly interpolate between two vectors and a given fraction to create a new vector
Vec3 Vec3::lerp(const Vec3 rhs, float t) const {
	// TODO Do not bother clamping yet
	return *this + (rhs - *this) * t;
};

Vec4 Vec3::toVec4() const { return Vec4{ x, y, z, 1.0f }; }

Vec2 Vec2::operator+(const Vec2 rhs) const { return { x + rhs.x, y + rhs.y }; }
Vec2& Vec2::operator+=(const Vec2 rhs) {
	x += rhs.x; y += rhs.y;
	return *this;
}
Vec2 Vec2::operator-(const Vec2 rhs) const { return { x - rhs.x, y - rhs.y }; }
Vec2& Vec2::operator-=(const Vec2 rhs) {
	x -= rhs.x; y -= rhs.y;
	return *this;
}
Vec2 Vec2::operator*(float s) const { return { x * s, y * s }; }
Vec2& Vec2::operator*=(float s) {
	x *= s; y *= s;
	return *this;
}
float Vec2::dot(const Vec2 rhs) const { return x * rhs.x + y * rhs.y; }
float Vec2::operator*(const Vec2 rhs) const { return x * rhs.x + y * rhs.y; }
Vec3 Vec2::cross(const Vec2 rhs) const {
	return {
		0,
		0,
		x * rhs.y - y * rhs.x };
}
Vec3 Vec2::operator^(const Vec2 rhs) const {
	return {
		0,
		0,
		x * rhs.y - y * rhs.x
	};
}

// ======== Lerp ========

// linearly interpolate between two vectors and a given fraction to create a new vector
Vec2 Vec2::lerp(const Vec2 rhs, float t) const {
	// TODO Do not bother clamping yet
	return *this + (rhs - *this) * t;
};