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
		std::vector<Vec3> readPositions;
		std::vector<Vec3> readNormals;
		std::vector<Vec2> readTexCoords;
		std::vector<TriIdx> readTris;
		std::vector<ModelVertices> vertices;

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
				readPositions.push_back(Vec3{ x, y, z });
			}
			else if (line.rfind("vn ", 0) == 0) {
				std::istringstream ls(line.substr(3));
				float x, y, z;
				ls >> x >> y >> z;
				readNormals.push_back(Vec3{ x, y, z });
			}
			else if (line.rfind("vt ", 0) == 0) {
				std::istringstream ls(line.substr(3));
				float u, v;
				ls >> u >> v;
				readTexCoords.push_back(Vec2{ u, v });
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
					// Split by '/'
					std::vector<std::string> parts;
					size_t start = 0;
					size_t pos = fe.find('/');
					while (pos != std::string::npos) {
						parts.push_back(fe.substr(start, pos - start));
						start = pos + 1;
						pos = fe.find('/', start);
					}
					parts.push_back(fe.substr(start)); // last part

					int vi = !parts[0].empty() ? std::stoi(parts[0]) : 0;
					int ti = (parts.size() > 1 && !parts[1].empty()) ? std::stoi(parts[1]) : 0;
					int ni = (parts.size() > 2 && !parts[2].empty()) ? std::stoi(parts[2]) : 0;

					if (ni <= 0)
						vb.hasNormals = false;
					if (ti <= 0)
						vb.hasUvs = false;

					vertexIndices.push_back(vi - 1);
					uvIndices.push_back(ti - 1);
					normalIndices.push_back(ni - 1);
				}

				for (size_t i = 0; i < vertexIndices.size() - 2; ++i) {
					uint32_t vi1 = vertexIndices[0];
					uint32_t vi2 = vertexIndices[1 + i];
					uint32_t vi3 = vertexIndices[2 + i];
					uint32_t ni1 = normalIndices[0];
					uint32_t ni2 = normalIndices[1 + i];
					uint32_t ni3 = normalIndices[2 + i];
					uint32_t ti1 = uvIndices[0];
					uint32_t ti2 = uvIndices[1 + i];
					uint32_t ti3 = uvIndices[2 + i];

					TriIdx tri;

					tri.verts = { vi1, vi2, vi3 };
					tri.normalIdx = { ni1, ni2, ni3 };
					tri.uvIdx = { ti1, ti2, ti3 };

					readTris.push_back(tri);
				}
			}
		}

		triangles.tris = readTris;
		vb.positions = readPositions;
		vb.normals = readNormals;
		vb.uvs = readTexCoords;

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