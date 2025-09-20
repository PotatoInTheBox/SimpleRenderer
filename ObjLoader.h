#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "Vec.h"
#include "Mesh.h"
#include "Vertex.h"

// load actual .obj files.

struct ObjLoader {
	struct VertexData {
		Vec3 position;
		Vec3 normal;
		Vec2 texCoord;
	};

	static MyMesh LoadFromString(const std::string& objText) {
		std::vector<Vec3> positions;
		std::vector<Vec3> normals;
		std::vector<Vec2> texCoords;
		//std::vector<ModelVertices> vertices;

		// Fill Mesh
		ModelVertices vb;
		ModelTriangles triangles;

		std::istringstream ss(objText);
		std::string line;

		while (std::getline(ss, line)) {
			// Trim leading/trailing whitespace
			line.erase(0, line.find_first_not_of(" \t\r\n"));
			line.erase(line.find_last_not_of(" \t\r\n") + 1);

			if (line.empty() || line[0] == '#') continue;

			if (line.rfind("v ", 0) == 0) {
				std::istringstream ls(line.substr(2));
				float x, y, z;
				ls >> x >> y >> z;
				positions.push_back(Vec3{ x, y, z });
			}
			else if (line.rfind("vn ", 0) == 0) {
				std::istringstream ls(line.substr(3));
				float x, y, z;
				ls >> x >> y >> z;
				normals.push_back(Vec3{ x, y, z });
			}
			else if (line.rfind("vt ", 0) == 0) {
				std::istringstream ls(line.substr(3));
				float u, v;
				ls >> u >> v;
				texCoords.push_back(Vec2{ u, v });
			}
			else if (line.rfind("f ", 0) == 0) {
				std::istringstream ls(line.substr(2));
				std::vector<std::string> faceElements;
				std::string token;
				while (ls >> token) faceElements.push_back(token);

				std::vector<uint32_t> vertexIndices;
				std::vector<uint32_t> normalIndices;
				std::vector<uint32_t> uvIndices;

				// Triangulate polygon if more than 3 vertices
				for (size_t i = 0; i < faceElements.size(); ++i) {
					std::string& fe = faceElements[i];
					std::replace(fe.begin(), fe.end(), '/', ' '); // replace / with space

					std::istringstream feStream(fe);
					int vi = 0, ti = 0, ni = 0;
					feStream >> vi >> ti >> ni;

					if (ni <= 0)
						vb.hasNormals = false;
					if (ti <= 0)
						vb.hasUvs = false;

					vertexIndices.push_back(vi - 1);
					uvIndices.push_back(ti - 1);
					normalIndices.push_back(ni - 1);
				}

				for (size_t i = 0; i < vertexIndices.size() - 2; ++i) {
					triangles.vertexTris.push_back({ vertexIndices[0], vertexIndices[1 + i], vertexIndices[2 + i] });
					triangles.normalTris.push_back({ normalIndices[0], normalIndices[1 + i], normalIndices[2 + i] });
					triangles.uvTris.push_back({ uvIndices[0], uvIndices[1 + i], uvIndices[2 + i] });
				}
			}
		}

		

		vb.positions = positions;
		if (vb.hasNormals)
			vb.normals = normals;
		if (vb.hasUvs)
			vb.uvs = texCoords;

		// TODO optimization is possible. If we create new vertices they get put
		// at the end of the list. This means while rendering we may have to jump
		// to near the end of the list. Which may marginally hurt performance.
		// If we sort the indices then load the triangle vertices in the order we see them
		// then we may get a performance increase.

		return MyMesh{ vb, triangles };
	}

	static MyMesh LoadFromFile(const std::string& filename) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			std::cerr << "Failed to open OBJ file: " << filename << "\n";
			return MyMesh{};
		}
		std::stringstream buffer;
		buffer << file.rdbuf();
		return LoadFromString(buffer.str());
	}
};