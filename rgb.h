#pragma once

namespace gpet
{
struct HSV;

struct RGB
{
	uint8_t r, g, b, a;

	RGB() :
		r(0),
		g(0),
		b(0),
		a(255)
	{}

	RGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) :
		r(r),
		g(g),
		b(b),
		a(a)
	{}

	RGB(sf::Color c) :
		r(c.r),
		g(c.g),
		b(c.b),
		a(c.a)
	{}

	RGB(gpet::HSV color) :
		a(color.a)
	{
		float h, s, v;

		h = color.h;
		s = color.s;
		v = color.v;

		float c, x;

		c = v * s;
		x = c * (1.0f - fabs(fmod((h / 60.0f), 2) - 1));

		float r_, g_, b_;

		if (h < 60.0f)
		{
			r_ = c;
			g_ = x;
			b_ = 0;
		}
		else if (h < 120.0f)
		{
			r_ = x;
			g_ = c;
			b_ = 0;
		}
		else if (h < 180.0f)
		{
			r_ = 0;
			g_ = c;
			b_ = x;
		}
		else if (h < 240.0f)
		{
			r_ = 0;
			g_ = x;
			b_ = c;
		}
		else if (h < 300.0f)
		{
			r_ = x;
			g_ = 0;
			b_ = c;
		}
		else if (h < 360.0f)
		{
			r_ = c;
			g_ = 0;
			b_ = x;
		}

		float m = v - c;

		r = (r_ + m) * 255;
		g = (g_ + m) * 255;
		b = (b_ + m) * 255;
	}

	gpet::HSV toHSV()
	{
		gpet::HSV result(0, 0, 0, a);

		float r_ = r / 255.0f;
		float g_ = g / 255.0f;
		float b_ = b / 255.0f;

		float cmax = std::max({ r_, g_, b_ });
		float delta = cmax - std::min({ r_, g_, b_ });

		if (delta == 0.0f)
		{
			result.h = 0.0f;
		}
		else if (cmax == r_)
		{
			result.h = 60 * fmod((g_ - b_) / delta, 6);
		}
		else if (cmax == g_)
		{
			result.h = 60 * ((b_ - r_) / delta + 2);
		}
		else if (cmax == b_)
		{
			result.h = 60 * ((r_ - g_) / delta + 4);
		}

		if (result.h < 0.0f)
			result.h += 360.0f;

		result.s = cmax == 0.0f ? 0.0f : (delta / cmax);
		result.v = cmax;

		return result;
	}
};
}