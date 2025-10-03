#pragma once

#include "Vec.h"
#include "Mat.h"
#include "Texture.h"
#include "raylib.h"

// Input per-vertex attributes (from mesh buffers).
struct VertexInput {
	Mat4 worldMatrix;
	Mat4 worldViewMatrix;
	Mat4 worldViewProjectionMatrix;
	Mat4 viewMatrix;
	Mat4 perspectiveMatrix;
	Vec3 position;
	Vec3 normal;
	Vec2 uv;
	Color color;  // not sure if I need it yet

	// Constructor requiring matrices
	VertexInput(const Mat4& world,
		const Mat4& worldView,
		const Mat4& worldViewProj,
		const Mat4& view,
		const Mat4& perspective)
		: worldMatrix(world),
		worldViewMatrix(worldView),
		worldViewProjectionMatrix(worldViewProj),
		viewMatrix(view),
		perspectiveMatrix(perspective){
	}
};

// Output after vertex shader (interpolated per-fragment).
struct VertexOutput {
	Vec4 clipPosition;   // required for rasterization
	Vec3 viewPosition;
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
		// for perspective correction
		Vec4 viewPos = in.worldViewMatrix * Vec4(in.position, 1.0f);

		// convert to world space
		Vec3 worldNormal = (in.worldMatrix * Vec4(in.normal, 0.0f)).toVec3().normalized();

		out.clipPosition = clipPos;
		out.viewPosition = viewPos.toVec3();
		out.worldPosition = worldPos.toVec3();
		out.normal = worldNormal;
		out.uv = in.uv;
		out.color = in.color;
		return out;
	}
};

#define MAT4_INDEX(c, r) ((c) * 4 + (r))

// Skybox shader
class SkyboxVertexShader : public IVertexShader {
public:
	VertexOutput run(const VertexInput& in) override {
		VertexOutput out;

		// Copy view matrix and zero translation
		Mat4 viewRotOnly = in.viewMatrix;
		viewRotOnly.m[MAT4_INDEX(3, 0)] = 0.0f;
		viewRotOnly.m[MAT4_INDEX(3, 1)] = 0.0f;
		viewRotOnly.m[MAT4_INDEX(3, 2)] = 0.0f;

		// Apply model/world matrix first
		Vec4 worldPos4 = in.worldMatrix * Vec4(in.position, 1.0f);
		Vec3 worldPos = worldPos4.toVec3();

		// Apply rotation-only view
		Vec4 worldDir4 = viewRotOnly * worldPos4;
		Vec3 worldDir = worldDir4.toVec3();

		// Clip space
		Vec4 clipPos = in.perspectiveMatrix * worldDir4;
		// Alternatively, if you have combined worldViewProjectionMatrix:
		// Vec4 clipPos = in.worldViewProjectionMatrix * Vec4(in.position, 1.0f);

		// Output
		out.clipPosition = clipPos;
		out.worldPosition = worldPos;
		out.viewPosition = worldDir;  // used for sampling cubemap
		out.normal = Vec3(0, 0, 0);   // not used for skybox
		out.uv = in.uv;
		out.color = in.color;          // usually white for skybox

		return out;
	}
};

// Fragment shader input = rasterizer-interpolated VertexOutput
struct FragmentInput {
	Vec3 worldPosition;  // useful for point lighting
	Vec3 normal;  // needed for shading
	Vec2 uv;  // needed for texturing
	Color color;  // debug color
	MyTexture* texture;  // for sampling textures
};

// Interface for a fragment shader
class IFragmentShader {
public:
	virtual Color run(const FragmentInput& in) = 0;
};

// Example fragment shader
class BasicFragmentShader : public IFragmentShader {
public:
	Color run(const FragmentInput& in) override {
		Vec3 lightDir = Vec3(0.5f, 1.0f, 0.3f).normalized();
		float diffuse = std::max(0.0f, in.normal.dot(lightDir) / 2.0f + 0.5f);
		return { 
			static_cast<uint8_t>(255 * diffuse),
			static_cast<uint8_t>(255 * diffuse),
			static_cast<uint8_t>(255 * diffuse),
			255 }; // simple shading
	}
};

// textured fragment shader
class TextureFragmentShader : public IFragmentShader {
public:
	Color run(const FragmentInput& in) override {
		Vec3 lightDir = Vec3(0.5f, 1.0f, 0.3f).normalized();
		float diffuse = std::max(0.0f, in.normal.dot(lightDir) / 2.0f + 0.5f);
		MyTexture* texture = in.texture;
		Vec3 color = (*texture).sampleTexture(in.uv.x, in.uv.y);
		color *= diffuse;
		return {
			static_cast<uint8_t>(255 * color.x),
			static_cast<uint8_t>(255 * color.y),
			static_cast<uint8_t>(255 * color.z),
			255 };
	}
};

class SkyboxFragmentShader : public IFragmentShader {
public:
	Color run(const FragmentInput& in) override {
		MyTexture* texture = in.texture;
		Vec3 color = (*texture).sampleTexture(in.uv.x, in.uv.y);
		return {
			static_cast<uint8_t>(in.color.r * color.x),
			static_cast<uint8_t>(in.color.g * color.y),
			static_cast<uint8_t>(in.color.b * color.z),
			255 };
	}
};

struct Shaders {
	IVertexShader* vertexShader;
	IFragmentShader* fragmentShader;
};

// flat shade that only takes in the vertex buffer and uses 