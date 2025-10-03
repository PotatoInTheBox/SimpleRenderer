#pragma once
#include <string>
#include "Mesh.h"
#include "Shader.h"
#include "Mat.h"
#include "Texture.h"

struct SceneObject {
	// name
	std::string name;

	// mesh type
	MyMesh mesh;

	// object transform (model matrix)
	Mat4 modelMatrix;

	// shader type
	Shaders shaders;

	// texture
	MyTexture* texture;

	// construct a scene object
	SceneObject(const std::string& n, const MyMesh& m, const Mat4& model = Mat4::identity(), MyTexture* tex = nullptr)
		: name(n), mesh(m), modelMatrix(model), texture(tex) {
		if (tex) {
			texture = tex;
			shaders.fragmentShader = new TextureFragmentShader();
			shaders.vertexShader = new BasicVertexShader();
		}
		else {
			texture = new MyTexture(); // heap-allocated default (1x1 white)
			shaders.fragmentShader = new BasicFragmentShader();
			shaders.vertexShader = new BasicVertexShader();
		}
	}

	// Destructor to free heap memory if we allocated it
	~SceneObject() {
		delete texture;
	}

	// Disable copy to avoid double-delete
	SceneObject(const SceneObject&) = delete;
	SceneObject& operator=(const SceneObject&) = delete;

};