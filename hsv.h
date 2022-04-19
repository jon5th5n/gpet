#pragma once

namespace gpet
{

struct HSV
{
	float h, s, v;
	uint8_t a;

	HSV() :
		h(0.0f),
		s(0.0f),
		v(0.0f),
		a(255)
	{}

	HSV(float h, float s, float v, uint8_t a = 255) :
		h(h),
		s(s),
		v(v),
		a(a)
	{}
};

}