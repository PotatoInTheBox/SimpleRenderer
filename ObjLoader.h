#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "Vec.h"
#include "Mesh.h"

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
		std::vector<VertexData> allVertexData;

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

				int64_t startSize = allVertexData.size();

				// Triangulate polygon if more than 3 vertices
				for (size_t i = 0; i < faceElements.size(); ++i) {
					std::string& fe = faceElements[i];
					std::replace(fe.begin(), fe.end(), '/', ' '); // replace / with space

					std::istringstream feStream(fe);
					int vi = 0, ti = 0, ni = 0;
					feStream >> vi >> ti >> ni;

					VertexData vert{};
					if (vi > 0) vert.position = positions[vi - 1];
					if (ti > 0) vert.texCoord = texCoords[ti - 1];
					if (ni > 0) vert.normal = normals[ni - 1];

					// Fan triangulation for polygons with >3 vertices
					if (i >= 3) {
						int64_t prev = allVertexData.size() - 1;
						allVertexData.push_back(allVertexData[startSize]);
						allVertexData.push_back(allVertexData[prev]);
					}

					allVertexData.push_back(vert);
				}
			}
		}

		// Fill Mesh
		VertexBuffers vb;
		TriIdxBuffer tb;

		// TODO optimization is possible. Quads are always getting duplicated
		// vertices. We can reuse them though.
		for (size_t i = 0; i < allVertexData.size(); ++i) {
			vb.positions.push_back(allVertexData[i].position);
			vb.normals.push_back(allVertexData[i].normal);
			vb.uvs.push_back(allVertexData[i].texCoord);

			if ((i + 1) % 3 == 0) {
				uint32_t a = static_cast<uint32_t>(i - 2);
				uint32_t b = static_cast<uint32_t>(i - 1);
				uint32_t c = static_cast<uint32_t>(i);
				tb.add(a, b, c);
			}
		}

		return MyMesh{ vb, tb };
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