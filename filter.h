#pragma once

namespace gpet
{

struct Filter
{
	float addedHue = 0.0f;
	float addedSaturation = 0.0f;
	float addedValue = 0.0f;

	std::vector<std::pair<float, float>> colorKeys;

	float brightness = 1.0f;
	float redInstensity = 1.0f;
	float greenInstensity = 1.0f;
	float blueInstensity = 1.0f;
};

}