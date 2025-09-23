#pragma once

#include <vector>
#include "Vec.h"
#include "Mat.h"
#include "Tri.h"
#include "Vertex.h"
#include "Scene.h"
#include "RenderTarget.h"
#include "Rasterizer.h"
#include "Shader.h"



// Manages the entire pipeline with data going in and out.

//drawMesh(const Mesh&, IVertexShader&, IFragmentShader&)

void perspectiveDivide() {


}

Vec3 viewportTransform(Vec4 ndc, int viewportWidth, int viewportHeight) {
	return Vec3(
		(ndc.x * 0.5f + 0.5f) * (viewportWidth - 1),
		viewportHeight - (ndc.y * 0.5f + 0.5f) * (viewportHeight - 1),
		ndc.z
	);
}

bool is_outside_frustrum(Vec4 v0, Vec4 v1, Vec4 v2) {

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
			/*-v.w > v.x || v.x > v.w ||
			-v.w > v.y || v.y > v.w ||
			-v.w > v.z || v.z > v.w ||*/
			0 > v.w;
		};

	if (outside(v0) || outside(v1) || outside(v2))
		return true;
	else
		return false;
}

bool is_backface(Vec3 v0, Vec3 v1, Vec3 v2) {
	Vec3 triNormal = (v1 - v0).cross(v2 - v0).normalized();

	Vec3 forward = Vec3{ 0,0,1.0f };
	if (triNormal.dot(forward) >= 0)
		return true;
	else
		return false;
}

void renderClippedTriangle(ClippedTriangle clippedTriangle, SceneObject* obj, Scene& scene, RenderTarget& target) {

	Vec3 v0 = clippedTriangle.clippedVertex[0].position;
	Vec3 v1 = clippedTriangle.clippedVertex[1].position;
	Vec3 v2 = clippedTriangle.clippedVertex[2].position;

	// =========== PERSPECTIVE DIVIDE ===========

	Vec4 cv1 = clippedTriangle.clippedVertex[0].clipPosition;
	Vec4 cv2 = clippedTriangle.clippedVertex[1].clipPosition;
	Vec4 cv3 = clippedTriangle.clippedVertex[2].clipPosition;

	Vec4 ndc1 = cv1 / cv1.w;
	Vec4 ndc2 = cv2 / cv2.w;
	Vec4 ndc3 = cv3 / cv3.w;


	// =========== VIEWPORT TRANSFORM  ===========

	Vec3 vv1 = viewportTransform(ndc1, target.width, target.height);
	Vec3 vv2 = viewportTransform(ndc2, target.width, target.height);
	Vec3 vv3 = viewportTransform(ndc3, target.width, target.height);


	// =========== BACKFACE CULL REJECTION ===========

	if (is_backface(vv1, vv2, vv3)) {
		return;  // move to next triangle (skip this one)
	}


	// =========== RASTERIZATION ===========

	Vec3 viewV0 = clippedTriangle.clippedVertex[0].viewPosition;
	Vec3 viewV1 = clippedTriangle.clippedVertex[1].viewPosition;
	Vec3 viewV2 = clippedTriangle.clippedVertex[2].viewPosition;
	float z0 = viewV0.z;
	float z1 = viewV1.z;
	float z2 = viewV2.z;

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
	Tri triScreen;
	triScreen.v0 = vv1;
	triScreen.v1 = vv2;
	triScreen.v2 = vv3;
	triScreen.depth0 = z0;
	triScreen.depth1 = z1;
	triScreen.depth2 = z2;
	if (obj->mesh.vertices.hasNormals) {
		triScreen.n0 = clippedTriangle.clippedVertex[0].worldNormal;
		triScreen.n1 = clippedTriangle.clippedVertex[1].worldNormal;
		triScreen.n2 = clippedTriangle.clippedVertex[2].worldNormal;
		triScreen.hasNormals = true;
	}
	if (obj->mesh.vertices.hasUvs) {
		triScreen.uv0 = clippedTriangle.clippedVertex[0].uv;
		triScreen.uv1 = clippedTriangle.clippedVertex[1].uv;
		triScreen.uv2 = clippedTriangle.clippedVertex[2].uv;
		triScreen.hasUvs = true;
	}

	triScreen.color = shadedColor;
	drawTriangle(triScreen, target.height, target.width, target.zBuffer, target.rgbBuffer, obj->texture);
}


class Renderer {
public:
	// Main render function
	void render(Scene& scene, RenderTarget& target) {

		// prepare a buffer we can use to modify the data with

		// Loop over all objects in the scene
		for (const auto& obj : scene.objects) {

			// Transform vertices and normals
			Mat4 viewMatrix = scene.camera.getViewMatrix();
			Mat4 perspectiveMatrix = Mat4::perspective(90.0f, target.width / (target.height * 1.0f), 0.01f, 1000.0f);
			Mat4 mvpMatrix = perspectiveMatrix * viewMatrix * obj->modelMatrix;

			// prepare the relevant matrix
			Mat4 worldMatrix = obj->modelMatrix;
			Mat4 worldViewMatrix = viewMatrix * obj->modelMatrix;
			Mat4 worldViewProjectionMatrix = perspectiveMatrix * viewMatrix * obj->modelMatrix;

			BasicVertexShader* shader = new BasicVertexShader();
			
			// go through each triangle
			for (size_t i = 0; i < obj->mesh.triangles.tris.size(); i++)
			{
				// =========== PREPARE ===========

				TriIdx tri = obj->mesh.triangles.tris[i];
				Vec3 v0 = obj->mesh.vertices.positions[tri[0]];
				Vec3 v1 = obj->mesh.vertices.positions[tri[1]];
				Vec3 v2 = obj->mesh.vertices.positions[tri[2]];
				
				// create our 3 vertex to work with using our vertex shader
				ProcessedTriangle processedTriangle;
				for (size_t o = 0; o < tri.verts.size(); o++)
				{
					// =========== VERTEX SHADER ===========

					size_t positionIdx = tri.verts[o];
					size_t normalIdx = tri.normalIdx[o];
					size_t uvIdx = tri.uvIdx[o];

					VertexInput vertexIn = { worldMatrix, worldViewMatrix, worldViewProjectionMatrix };
					vertexIn.position = obj->mesh.vertices.positions[positionIdx];
					if (obj->mesh.vertices.hasNormals)
						vertexIn.normal = obj->mesh.vertices.normals[normalIdx];
					if (obj->mesh.vertices.hasUvs)
						vertexIn.uv = obj->mesh.vertices.uvs[uvIdx];
					vertexIn.color = WHITE;
					VertexOutput vertexOut = shader->run(vertexIn);
					processedTriangle.vertexOutput[o] = vertexOut;
				}


				// =========== FRUSTRUM CLIPPING REJECTION ===========

				Vec4 cv1 = processedTriangle.vertexOutput[0].clipPosition;
				Vec4 cv2 = processedTriangle.vertexOutput[1].clipPosition;
				Vec4 cv3 = processedTriangle.vertexOutput[2].clipPosition;
				float nearClipDistance = 0.01f;
				
				// at this point we need to deal with triangles intersecting the camera
					// we can't draw them until we clip them.

				bool clip1 = cv1.z <= nearClipDistance;
				bool clip2 = cv2.z <= nearClipDistance;
				bool clip3 = cv3.z <= nearClipDistance;
				int clipCount = (clip1 ? 1 : 0) + (clip2 ? 1 : 0) + (clip3 ? 1 : 0);

				ClippedTriangle clippedTriangle;
				clippedTriangle.clippedVertex[0].position = v0;
				clippedTriangle.clippedVertex[1].position = v1;
				clippedTriangle.clippedVertex[2].position = v2;
				for (size_t i = 0; i < 3; i++)
				{
					//clippedTriangle.clippedVertex[i].position = processedTriangle.vertexOutput[i].position;
					clippedTriangle.clippedVertex[i].clipPosition = processedTriangle.vertexOutput[i].clipPosition;
					clippedTriangle.clippedVertex[i].viewPosition = processedTriangle.vertexOutput[i].viewPosition;
					clippedTriangle.clippedVertex[i].worldPosition = processedTriangle.vertexOutput[i].worldPosition;
					clippedTriangle.clippedVertex[i].worldNormal = processedTriangle.vertexOutput[i].normal;
					clippedTriangle.clippedVertex[i].uv = processedTriangle.vertexOutput[i].uv;
				}
				
				if (clipCount == 0) {
					renderClippedTriangle(clippedTriangle, obj.get(), scene, target);
				}
				else if (clipCount == 1) {
					// #1 one vertex is outside the 0 z frustrum. We need to clip it into 2 triangles (quad)
					std::array<ClippedTriangle, 2> clipped = clippedTriangle.clipZOne(clip1, clip2, nearClipDistance);
					ClippedTriangle triA = clipped[0];
					ClippedTriangle triB = clipped[1];
					renderClippedTriangle(triA, obj.get(), scene, target);
					renderClippedTriangle(triB, obj.get(), scene, target);

				}
				else if (clipCount == 2) {
					// #2 two vertices are outside the 0 z frustrum. We need to clip into 1 triangle
					ClippedTriangle clipTri2 = clippedTriangle.clipZTwo(clip1, clip2, nearClipDistance);
					renderClippedTriangle(clipTri2, obj.get(), scene, target);
				}
				else {
					// #3 it is completely out of the frustrum. (we can skip)
				}
			}
		}

		// ======================
		// 6. Post-processing (optional)
		// ======================
		// e.g., tone mapping, gamma correction, FXAA
	}
};