#pragma once

#include <vector>
#include "Vec.h"
#include "Mat.h"
#include "Tri.h"
#include "Vertex.h"
#include "Scene.h"
#include "RenderTarget.h"
#include "Rasterizer.h"



// Manages the entire pipeline with data going in and out.

//drawMesh(const Mesh&, IVertexShader&, IFragmentShader&)


class Renderer {
public:
	// Main render function
	void render(Scene& scene, RenderTarget& target) {

		// Loop over all objects in the scene
		for (SceneObject* obj : scene.objects) {

			// ======================
			// 1. Vertex Processing
			// ======================

			// clear previous data.
			obj->mesh.vertices.clipSpacePositions.clear();
			obj->mesh.vertices.worldSpaceNormals.clear();
			obj->mesh.vertices.screenSpacePositions.clear();

			// temp buffer to hold all triangle shade amount
			std::vector<float> shadeAmounts;

			// Transform vertices and normals
			for (size_t i = 0; i < obj->mesh.vertices.positions.size(); ++i) {
				Mat4 viewMatrix = scene.camera.getViewMatrix();
				Mat4 perspectiveMatrix = Mat4::perspective(90.0f, target.width / (target.height * 1.0f), 0.01f, 1000.0f);
				Mat4 mvpMatrix = perspectiveMatrix * viewMatrix * obj->modelMatrix;
				Vec4 clipPosition = mvpMatrix * Vec4(obj->mesh.vertices.positions[i], 1.0f);
				obj->mesh.vertices.clipSpacePositions.push_back(clipPosition);
				if (obj->mesh.vertices.normals.size() > i) {
					obj->mesh.vertices.worldSpaceNormals.push_back((
						obj->modelMatrix * Vec4(obj->mesh.vertices.normals[i], 0.0f)).toVec3());
				}
				// UVs and colors are passed through as-is
				//obj.vb.uvs = obj.vb.uvs;
			}

			// ======================
			// 2. Backface Culling
			// ======================
			// possibly put this in a buffer?
			std::vector<TriIdx> visibleTris;
			for (const auto& tri : obj->mesh.triangles.tris) {
				Vec3 v0 = obj->mesh.vertices.clipSpacePositions[tri[0]].toVec3();
				Vec3 v1 = obj->mesh.vertices.clipSpacePositions[tri[1]].toVec3();
				Vec3 v2 = obj->mesh.vertices.clipSpacePositions[tri[2]].toVec3();
				Vec3 triNormal = (v1 - v0).cross(v2 - v0).normalized();

				//// calculate our shade value for debugging
				//Vec3 down = Vec3{ 0,-1.0f,0 };
				//float shadeAmount = down.dot(triNormal);
				//shadeAmounts.push_back(shadeAmount);

				Vec3 forward = Vec3{ 0,0,1.0f };
				//if (triNormal.dot(forward) >= 0) continue; // skip backfaces
				visibleTris.push_back(tri);
			}

			// ======================
			// 3. Frustum Clipping
			// ======================
			// Clip triangles against near/far and view frustum
			std::vector<TriIdx> clippedTris;
			for (auto& tri : visibleTris) {
				// Clip triangle
				// Add resulting triangles to clippedTris

				// for now let's just drop triangles that are outside the frustrum.
				// we will remove any with a z value that is 0.01 and below;

				Vec4 v0 = obj->mesh.vertices.clipSpacePositions[tri[0]];
				Vec4 v1 = obj->mesh.vertices.clipSpacePositions[tri[1]];
				Vec4 v2 = obj->mesh.vertices.clipSpacePositions[tri[2]];

				auto outside = [](const Vec4& v) {
					//return 
					//	v.x < -v.w || 
					//	v.x > v.w ||
					//	v.y < -v.w || 
					//	v.y > v.w ||
					//	v.z < 0.01f || 
					//	v.z > v.w;
					//};
					return
						-v.w > v.x || v.x > v.w ||
						-v.w > v.y || v.y > v.w ||
						-v.w > v.z || v.z > v.w ||
						0 > v.w;
					};

				if (outside(v0) || outside(v1) || outside(v2))
					continue;

				clippedTris.push_back(tri);
			}

			// ======================
			// 4. Perspective Divide & Viewport Transform
			// ======================
			for (size_t i = 0; i < obj->mesh.vertices.clipSpacePositions.size(); ++i) {
				Vec4 ndc = obj->mesh.vertices.clipSpacePositions[i] / obj->mesh.vertices.clipSpacePositions[i].w;
				obj->mesh.vertices.screenSpacePositions.push_back(Vec3(
					(ndc.x * 0.5f + 0.5f) * (target.width - 1),
					target.height - (ndc.y * 0.5f + 0.5f) * (target.height - 1),
					ndc.z
				));
			}

			// ======================
			// 5. Rasterization
			// ======================
			for (auto& tri : clippedTris) {
				// - compute bounding box
				// - loop over pixels
				// - compute barycentric coordinates
				// - skip pixels outside triangle
				// - interpolate attributes: color, normals, uv
				// - fragment shader: lighting / texture
				// - depth test and write to target

				Vec3 v0 = obj->mesh.vertices.positions[tri[0]];
				Vec3 v1 = obj->mesh.vertices.positions[tri[1]];
				Vec3 v2 = obj->mesh.vertices.positions[tri[2]];
				Vec3 triNormal = (v1 - v0).cross(v2 - v0).normalized();

				// calculate our shade value for debugging
				Vec3 down = Vec3{ 0,1.0f,0 };
				float shadeAmount = (down.dot(triNormal) + 1.0f) / 2.0f;

				Color shadedColor{
					static_cast<unsigned char>(std::clamp(WHITE.r * shadeAmount, 0.0f, 255.0f)),
					static_cast<unsigned char>(std::clamp(WHITE.g * shadeAmount, 0.0f, 255.0f)),
					static_cast<unsigned char>(std::clamp(WHITE.b * shadeAmount, 0.0f, 255.0f)),
					WHITE.a
				};

				drawTriangle(Tri{
					obj->mesh.vertices.screenSpacePositions[tri[0]],
					obj->mesh.vertices.screenSpacePositions[tri[1]],
					obj->mesh.vertices.screenSpacePositions[tri[2]] },
					target.height, target.width, target.zBuffer, target.rgbBuffer, shadedColor);
			}
		}

		// ======================
		// 6. Post-processing (optional)
		// ======================
		// e.g., tone mapping, gamma correction, FXAA
	}
};