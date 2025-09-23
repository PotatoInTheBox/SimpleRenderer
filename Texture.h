#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include "Vec.h"


class Texture {
public:
	Texture() = default;

	// Load from raw bytes file
	Texture(const std::string& filepath) {
		loadFromFile(filepath);
	}

	void loadFromFile(const std::string& filepath) {
		std::ifstream file(filepath, std::ios::binary);
		if (!file.is_open())
			throw std::runtime_error("Failed to open texture file: " + filepath);

		std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(file)),
			std::istreambuf_iterator<char>());

		if (bytes.size() < 4)
			throw std::runtime_error("Texture file too small: " + filepath);

		width = bytes[0] | (bytes[1] << 8);
		height = bytes[2] | (bytes[3] << 8);

		if (bytes.size() < 4 + width * height * 3)
			throw std::runtime_error("Texture file incomplete: " + filepath);

		image.resize(width * height);

		size_t index = 4;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				float r = bytes[index + 0] / 255.0f;
				float g = bytes[index + 1] / 255.0f;
				float b = bytes[index + 2] / 255.0f;
				index += 3;
				image[y * width + x] = Vec3(r, g, b);
			}
		}
	}

	const Vec3& getPixel(int x, int y) const {
		return image[y * width + x];
	}

	int getWidth() const { return width; }
	int getHeight() const { return height; }

private:
	int width = 0;
	int height = 0;
	std::vector<Vec3> image; // row-major order
};