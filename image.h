#pragma once
#include "filter.h"
#include "rgb.h"

namespace gpet
{

class Image
{
	std::vector<uint8_t> pixels;
	std::vector<uint8_t> pixelsFiltered;
	sf::Vector2u size { 0, 0 };

public:
	Image()
	{}

	Image(gpet::Image* img) :
		size(sf::Vector2u(img->getSize().x, img->getSize().y))
	{
		pixels = img->pixels;
		pixelsFiltered = img->pixelsFiltered;
	}

	Image(std::string filepath)
	{
		loadFromFile(filepath);
	}

	~Image()
	{}

	void loadFromFile(std::string filepath)
	{
		sf::Image img;
		img.loadFromFile(filepath);
		const sf::Uint8* imgPixels = img.getPixelsPtr();

		size = img.getSize();

		pixels.resize(size.x * size.y * 4);
		for (uint i = 0; i < img.getSize().x * img.getSize().y * 4; i++)
			pixels[i] = imgPixels[i];

		pixelsFiltered = pixels;
	}

	void saveToFile(std::string filepath, bool filtered)
	{
		std::vector<uint8_t> pix = filtered ? pixelsFiltered : pixels;

		sf::Image img;
		img.create(size.x, size.y, &pix[0]);
		img.saveToFile(filepath);
	}

	//---

	sf::Vector2u getSize()
	{
		return size;
	}

	std::vector<uint8_t> getPixelArray()
	{
		return pixels;
	}
	std::vector<uint8_t> getPixelArray(gpet::Filter filter)
	{
		for (uint i = 0; i < size.x * size.y; i++)
		{
			gpet::RGB c(pixels[i * 4 + 0], pixels[i * 4 + 1], pixels[i * 4 + 2], pixels[i * 4 + 3]);
			gpet::HSV hsv = c.toHSV();

			//-----

			hsv.h = fmod(hsv.h + filter.addedHue, 360.0f);
			hsv.s = std::clamp(hsv.s + filter.addedSaturation, 0.0f, 1.0f);
			hsv.v = std::clamp(hsv.v + filter.addedValue, 0.0f, 1.0f);

			bool inKeys = true;
			for (auto key : filter.colorKeys)
			{
				float top = fmod(key.first + key.second, 360.0f);
				float bottom = fmod(key.first - key.second, 360.0f);
				if (bottom < 0)
					bottom += 360.0f;
				if (top < 0)
					top += 360.0f;

				if (top < bottom)
					if (!(hsv.h < bottom && hsv.h > top))
					{
						inKeys = false;
						continue;
					}

				if (hsv.h < top && hsv.h > bottom)
					inKeys = false;
			}

			//-
			c = gpet::RGB(hsv);
			//-

			if (!inKeys)
			{
				uint8_t greyValue = c.r * 0.299 + c.g * 0.587 + c.b * 0.114;
				c = gpet::RGB(greyValue, greyValue, greyValue);
			}

			c.r *= filter.redInstensity * filter.brightness;
			c.g *= filter.greenInstensity * filter.brightness;
			c.b *= filter.blueInstensity * filter.brightness;

			//-----

			pixelsFiltered[i * 4 + 0] = c.r;
			pixelsFiltered[i * 4 + 1] = c.g;
			pixelsFiltered[i * 4 + 2] = c.b;
			pixelsFiltered[i * 4 + 3] = c.a;
		}

		return pixelsFiltered;
	}

	gpet::RGB getPixel(uint x, uint y)
	{
		gpet::RGB c;

		c.r = pixels[(x + (y * size.x)) * 4 + 0];
		c.g = pixels[(x + (y * size.x)) * 4 + 1];
		c.b = pixels[(x + (y * size.x)) * 4 + 2];
		c.a = pixels[(x + (y * size.x)) * 4 + 3];

		return c;
	}
	gpet::RGB getPixelFiltered(uint x, uint y)
	{
		gpet::RGB c;

		c.r = pixelsFiltered[(x + (y * size.x)) * 4 + 0];
		c.g = pixelsFiltered[(x + (y * size.x)) * 4 + 1];
		c.b = pixelsFiltered[(x + (y * size.x)) * 4 + 2];
		c.a = pixelsFiltered[(x + (y * size.x)) * 4 + 3];

		return c;
	}

	void setPixel(uint x, uint y, gpet::RGB c)
	{
		pixels[(x + (y * size.x)) * 4 + 0] = c.r;
		pixels[(x + (y * size.x)) * 4 + 1] = c.g;
		pixels[(x + (y * size.x)) * 4 + 2] = c.b;
		pixels[(x + (y * size.x)) * 4 + 3] = c.a;
	}

	//---

	void flipHorizontal()
	{
		gpet::Image imgcpy(this);

		for (uint i = 0; i < size.x; i++)
			for (uint j = 0; j < size.y; j++)
				setPixel(i, j, imgcpy.getPixel(size.x - 1 - i, j));
	}

	void flipVertical()
	{
		gpet::Image imgcpy(this);

		for (uint i = 0; i < size.x; i++)
			for (uint j = 0; j < size.y; j++)
				setPixel(i, j, imgcpy.getPixel(i, size.y - 1 - j));
	}

	void rotateRight()
	{
		gpet::Image imgcpy(this);

		size.x = imgcpy.getSize().y;
		size.y = imgcpy.getSize().x;

		for (uint i = 0; i < size.x; i++)
			for (uint j = 0; j < size.y; j++)
				setPixel(i, j, imgcpy.getPixel(j, size.x - 1 - i));
	}
	void rotateLeft()
	{
		gpet::Image imgcpy(this);

		size.x = imgcpy.getSize().y;
		size.y = imgcpy.getSize().x;

		for (uint i = 0; i < size.x; i++)
			for (uint j = 0; j < size.y; j++)
				setPixel(i, j, imgcpy.getPixel(imgcpy.getSize().x - 1 - j, i));
	}
	void rotate180()
	{
		gpet::Image imgcpy(this);

		for (uint i = 0; i < size.x; i++)
			for (uint j = 0; j < size.y; j++)
				setPixel(i, j, imgcpy.getPixel(size.x - 1 - i, size.y - 1 - j));
	}

	//---

	void makeGreyScale()
	{
		for (uint i = 0; i < size.x; i++)
			for (uint j = 0; j < size.y; j++)
				setPixel(i, j, getPixel(i, j).convertToGreyNatural());
	}

	void invert()
	{
		for (uint i = 0; i < size.x; i++)
			for (uint j = 0; j < size.y; j++)
				setPixel(i, j, getPixel(i, j).invert());
	}

	//---

	void draw(uint x, uint y, uint brush, uint brushSize, gpet::RGB color)
	{
		switch (brush)
		{
			case 0: {
				uint n = 2 * brushSize + 1;

				for (uint i = 0; i < n; i++)
					for (uint j = 0; j < n; j++)
					{
						if ((x - brushSize) + i >= size.x || (y - brushSize) + j >= size.y)
							continue;

						setPixel((x - brushSize) + i, (y - brushSize) + j, color);
					}

				break;
			}

			case 1: {
				uint n = 2 * brushSize + 1;

				for (uint i = 0; i < n; i++)
					for (uint j = 0; j < n; j++)
					{
						if ((x - brushSize) + i >= size.x || (y - brushSize) + j >= size.y)
							continue;

						if ((i - brushSize) * (i - brushSize) + (j - brushSize) * (j - brushSize) <= brushSize * brushSize + 1)
							setPixel((x - brushSize) + i, (y - brushSize) + j, color);
					}

				break;
			}

			default:
				break;
		}
	}
	void draw(uint x, uint y, uint lastX, uint lastY, uint brush, uint brushSize, gpet::RGB color)
	{
		uint numberPoints = abs((int)x - (int)lastX) > abs((int)y - (int)lastY) ? abs((int)x - (int)lastX) / 2 : abs((int)y - (int)lastY) / 2;

		for (uint i = 0; i <= numberPoints; i++)
		{
			draw(lastX + ((float)((int)x - (int)lastX) / (numberPoints + 1)) * i, lastY + ((float)((int)y - (int)lastY) / (numberPoints + 1)) * i, brush, brushSize, color);
		}
	}
};

}