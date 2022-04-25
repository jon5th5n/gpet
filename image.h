#pragma once
#include "rgb.h"

namespace gpet
{

class Image
{
	uint8_t* pixels;
	uint8_t* pixelsFiltered;
	sf::Vector2u size;

public:
	float addedHue = 0.0f;
	float addedSaturation = 0.0f;
	float addedValue = 0.0f;
	std::vector<std::pair<float, float>> colorKeys;

	Image() :
		pixels(new uint8_t[0]),
		pixelsFiltered(new uint8_t[0]),
		size(sf::Vector2u(0, 0))
	{}

	Image(gpet::Image* img) :
		pixels(new uint8_t[img->getSize().x * img->getSize().y * 4]),
		pixelsFiltered(new uint8_t[img->getSize().x * img->getSize().y * 4]),
		size(sf::Vector2u(img->getSize().x, img->getSize().y))
	{
		std::copy(&(img->getPixelArray()[0]), &(img->getPixelArray()[(img->getSize().x * img->getSize().y * 4) - 1]), &pixels[0]);
		std::copy(&(img->getPixelArray()[0]), &(img->getPixelArray()[(img->getSize().x * img->getSize().y * 4) - 1]), &pixelsFiltered[0]);
	}

	~Image()
	{
		delete[] pixels;
		delete[] pixelsFiltered;
	}

	void loadFromFile(std::string filepath)
	{
		sf::Image img;
		img.loadFromFile(filepath);

		delete[] pixels;
		pixels = new uint8_t[img.getSize().x * img.getSize().y * 4];
		for (uint i = 0; i < img.getSize().x * img.getSize().y * 4; i++)
			pixels[i] = img.getPixelsPtr()[i];

		delete[] pixelsFiltered;
		pixelsFiltered = new uint8_t[img.getSize().x * img.getSize().y * 4];
		std::copy(&(pixels[0]), &(pixels[(img.getSize().x * img.getSize().y * 4) - 1]), &pixelsFiltered[0]);

		size = img.getSize();
	}

	void saveToFile(std::string filepath, bool filtered)
	{
		uint8_t* pix = filtered ? pixelsFiltered : pixels;

		sf::Image img;
		img.create(size.x, size.y, pix);
		img.saveToFile(filepath);
	}

	//---

	sf::Vector2u getSize()
	{
		return size;
	}

	uint8_t* getPixelArray()
	{
		return pixels;
	}
	uint8_t* getPixelArrayFiltered()
	{
		for (uint i = 0; i < size.x * size.y; i++)
		{
			gpet::RGB c(pixels[i * 4 + 0], pixels[i * 4 + 1], pixels[i * 4 + 2], pixels[i * 4 + 3]);
			gpet::HSV hsv = c.toHSV();

			//-----

			hsv.h = fmod(hsv.h + addedHue, 360.0f);
			hsv.s = std::clamp(hsv.s + addedSaturation, 0.0f, 1.0f);
			hsv.v = std::clamp(hsv.v + addedValue, 0.0f, 1.0f);

			bool inKeys = true;
			for (auto key : colorKeys)
			{
				float top = fmod(key.first + key.second, 360.0f);
				float bottom = fmod(key.first - key.second, 360.0f);
				if (bottom < 0)
					bottom += 360.0f;

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

			//-----

			pixelsFiltered[i * 4 + 0] = c.r;
			pixelsFiltered[i * 4 + 1] = c.g;
			pixelsFiltered[i * 4 + 2] = c.b;
			pixelsFiltered[i * 4 + 3] = c.a;
		}

		return pixelsFiltered;
	}
	uint8_t* getPixelArray(bool filtered)
	{
		return filtered ? getPixelArrayFiltered() : pixels;
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
			{
				uint8_t greyValue = getPixel(i, j).r * 0.299 + getPixel(i, j).g * 0.587 + getPixel(i, j).b * 0.114;
				setPixel(i, j, gpet::RGB { greyValue, greyValue, greyValue });
			}
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