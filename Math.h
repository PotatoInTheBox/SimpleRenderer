#pragma once
#define PI 3.14159265358979323846f

// Let me try to break this down for myself.

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
	// Add two Vec4 together
    Vec4 operator+(const Vec4 rhs) const;
	// Subtract the left Vec4 by the right Vec4
    Vec4 operator-(const Vec4 rhs) const;
	// Scale the `x, y, z, w` Vec4 value by a given scale factor
    Vec4 operator*(float s) const;
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
};

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

	static Mat4 translation(float tx, float ty, float tz = 0.0f);

	static Mat4 rotation(float angleX, float angleY, float angleZ);

	static Mat4 scale(float sx, float sy, float sz = 1.0f);

	static Mat4 compose(float tx, float ty, float tz,
		float angleX, float angleY, float angleZ,
		float sx, float sy, float sz);
	
	static Mat4 perspective(float fovDeg, float aspect, float near, float far);
};

// Convert degrees to radians
inline float deg2rad(float degrees) {
	return degrees * (PI / 180.0f);
}

// Convert radians to degrees
inline float rad2deg(float radians) {
	return radians * (180.0f / PI);
}
