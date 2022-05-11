#pragma once
#include "image.h"

namespace gpet
{

struct Layer
{
	std::string name;

	gpet::Image image;
	gpet::Filter filter;
	bool filtered = true;
	bool visible = true;
	bool selected = false;

	float scale = 1.0f;
	sf::Vector2i offset { 0, 0 };

	char editableName[15];
	bool nameBeingEdited = false;

	Layer(std::string n, std::string filepath) :
		name(n)
	{
		strcpy(editableName, n.c_str());
		image.loadFromFile(filepath);
	}
};

class Project
{
	sf::Vector2u size;

	uint backgroundGridSize = 30;
	std::pair<uint, uint> backgroundGridColors = { 180, 240 };

	std::vector<uint8_t> pixels;

public:
	std::vector<gpet::Layer> layers;
	uint selectedLayer = 0;

	Project(sf::Vector2u s) :
		size(s)
	{
		pixels.resize(size.x * size.y * 4);
	}

	~Project()
	{}

	sf::Vector2u getSize()
	{
		return size;
	}

	std::vector<uint8_t> getPixels()
	{
		for (uint i = 0; i < size.x; i++)
			for (uint j = 0; j < size.y; j++)
			{
				pixels[(i + (j * size.x)) * 4 + 0] = (i % backgroundGridSize < backgroundGridSize / 2) ^ (j % backgroundGridSize < backgroundGridSize / 2) ? backgroundGridColors.first : backgroundGridColors.second;
				pixels[(i + (j * size.x)) * 4 + 1] = (i % backgroundGridSize < backgroundGridSize / 2) ^ (j % backgroundGridSize < backgroundGridSize / 2) ? backgroundGridColors.first : backgroundGridColors.second;
				pixels[(i + (j * size.x)) * 4 + 2] = (i % backgroundGridSize < backgroundGridSize / 2) ^ (j % backgroundGridSize < backgroundGridSize / 2) ? backgroundGridColors.first : backgroundGridColors.second;
				pixels[(i + (j * size.x)) * 4 + 3] = 255;
			}

		for (uint i = 0; i < layers.size(); i++)
		{
			gpet::Layer layer = layers[i];
			std::vector<uint8_t> imgPixels = layer.filtered ? layer.image.getPixelArray(layer.filter) : layer.image.getPixelArray();

			for (uint i = 0; i < size.x; i++)
				for (uint j = 0; j < size.y; j++)
				{
					int imgI = ((int)i - layer.offset.x) / layer.scale;
					int imgJ = ((int)j - layer.offset.y) / layer.scale;

					if (imgI < 0 || imgJ < 0 || imgI >= layer.image.getSize().x || imgJ >= layer.image.getSize().y)
						continue;

					float a = (float)imgPixels[(imgI + (imgJ * layer.image.getSize().x)) * 4 + 3] / 255;
					pixels[(i + (j * size.x)) * 4 + 0] = std::lerp(pixels[(i + (j * size.x)) * 4 + 0], imgPixels[(imgI + (imgJ * layer.image.getSize().x)) * 4 + 0], a);
					pixels[(i + (j * size.x)) * 4 + 1] = std::lerp(pixels[(i + (j * size.x)) * 4 + 1], imgPixels[(imgI + (imgJ * layer.image.getSize().x)) * 4 + 1], a);
					pixels[(i + (j * size.x)) * 4 + 2] = std::lerp(pixels[(i + (j * size.x)) * 4 + 2], imgPixels[(imgI + (imgJ * layer.image.getSize().x)) * 4 + 2], a);
					pixels[(i + (j * size.x)) * 4 + 3] = std::clamp(pixels[(i + (j * size.x)) * 4 + 3] + imgPixels[(imgI + (imgJ * layer.image.getSize().x)) * 4 + 3], 0, 255);
				}
		}

		return pixels;
	}

	void addLayer(std::string filepath)
	{
		layers.push_back(gpet::Layer("Layer " + std::to_string(layers.size() + 1), filepath));
	}
};
}