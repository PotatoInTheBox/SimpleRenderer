#pragma once
#include <string>
#include "Mesh.h"
#include "Shader.h"
#include "Mat.h"

struct SceneObject {
	// name
	std::string name;

	// mesh type
	MyMesh mesh;

	// object transform (model matrix)
	Mat4 modelMatrix;

	// shader type
	// TODO

	// construct a scene object
	SceneObject(const std::string& n, const MyMesh& m, const Mat4& model = Mat4::identity())
		: name(n), mesh(m), modelMatrix(model) {
	}

};