#pragma once

#include "Vec.h"
#include "Mat.h"
#include "raylib.h"

// Input per-vertex attributes (from mesh buffers).
struct VertexInput {
	Mat4 worldMatrix;
	Mat4 worldViewMatrix;
	Mat4 worldViewProjectionMatrix;
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
	Color color;  // not sure if I need it yet

	// Constructor requiring matrices
	VertexInput(const Mat4& world,
		const Mat4& worldView,
		const Mat4& worldViewProj)
		: worldMatrix(world),
		worldViewMatrix(worldView),
		worldViewProjectionMatrix(worldViewProj) {
	}
};

// Output after vertex shader (interpolated per-fragment).
struct VertexOutput {
	Vec4 clipPosition;   // required for rasterization
	Vec3 worldPosition;  // useful for lighting
	Vec3 normal;         // in world space
	Vec2 uv;
	Color color;  // not sure if I need it yet
};

// Interface for a vertex shader
class IVertexShader {
public:
	virtual VertexOutput run(const VertexInput& in) = 0;
};

// Example vertex shader
class BasicVertexShader : public IVertexShader {
public:
	VertexOutput run(const VertexInput& in) override {
		VertexOutput out;

		// world position (could be useful for lighting)
		Vec4 worldPos = in.worldMatrix * Vec4(in.position, 1.0f);
		// clip position, necessary for rasterizing
		Vec4 clipPos = in.worldViewProjectionMatrix * Vec4(in.position, 1.0f);

		// convert to world space
		Vec3 worldNormal = (in.worldMatrix * Vec4(in.normal, 0.0f)).toVec3().normalized();

		out.clipPosition = clipPos;
		out.worldPosition = worldPos.toVec3();
		out.normal = worldNormal;
		out.uv = in.uv;
		out.color = in.color;
		return out;
	}
};

// Fragment shader input = rasterizer-interpolated VertexOutput
struct FragmentInput {
	Vec3 worldPosition;
	Vec3 normal;
	Vec2 uv;
	Color color;
};

// Interface for a fragment shader
class IFragmentShader {
public:
	virtual Color run(const FragmentInput& in) = 0;
};

// Example fragment shader (lambert diffuse)
//class BasicFragmentShader : public IFragmentShader {
//public:
//	Color run(const FragmentInput& in) override {
//		Vec3 lightDir = normalize(Vec3(0.5f, 1.0f, 0.3f));
//		float ndotl = std::max(0.0f, dot(in.normal, lightDir));
//		return in.color * ndotl; // simple shading
//	}
//};

// flat shade that only takes in the vertex buffer and uses 