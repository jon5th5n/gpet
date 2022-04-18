#pragma once

namespace gpet
{

class Image
{
	uint8_t* pixels;
	sf::Vector2u size;

public:
	Image() :
		pixels(new uint8_t[0]),
		size(sf::Vector2u(0, 0))
	{}

	Image(gpet::Image* img) :
		pixels(new uint8_t[img->getSize().x * img->getSize().y * 4]),
		size(sf::Vector2u(img->getSize().x, img->getSize().y))
	{
		std::copy(&(img->getPixelArray()[0]), &(img->getPixelArray()[(img->getSize().x * img->getSize().y * 4) - 1]), &pixels[0]);
	}

	~Image()
	{
		delete[] pixels;
	}

	void loadFromFile(std::string filepath)
	{
		sf::Image img;
		img.loadFromFile(filepath);

		delete[] pixels;
		pixels = new uint8_t[img.getSize().x * img.getSize().y * 4];
		for (uint i = 0; i < img.getSize().x * img.getSize().y * 4; i++)
			pixels[i] = img.getPixelsPtr()[i];

		size = img.getSize();
	}

	void saveToFile(std::string filepath)
	{
		sf::Image img;
		img.create(size.x, size.y, pixels);
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

	sf::Color getPixel(uint x, uint y)
	{
		sf::Color c;

		c.r = pixels[(x + (y * size.x)) * 4 + 0];
		c.g = pixels[(x + (y * size.x)) * 4 + 1];
		c.b = pixels[(x + (y * size.x)) * 4 + 2];
		c.a = pixels[(x + (y * size.x)) * 4 + 3];

		return c;
	}

	void setPixel(uint x, uint y, sf::Color c)
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

	void makeGreyScale()
	{
		for (uint i = 0; i < size.x; i++)
			for (uint j = 0; j < size.y; j++)
			{
				uint8_t greyValue = getPixel(i, j).r * 0.299 + getPixel(i, j).g * 0.587 + getPixel(i, j).b * 0.114;
				setPixel(i, j, sf::Color { greyValue, greyValue, greyValue });
			}
	}

	//---

	void draw(uint x, uint y, uint brush, uint brushSize, sf::Color color)
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
	void draw(uint x, uint y, uint lastX, uint lastY, uint brush, uint brushSize, sf::Color color)
	{
		uint numberPoints = abs((int)x - (int)lastX) > abs((int)y - (int)lastY) ? abs((int)x - (int)lastX) / 2 : abs((int)y - (int)lastY) / 2;

		for (uint i = 0; i <= numberPoints; i++)
		{
			draw(lastX + ((float)((int)x - (int)lastX) / (numberPoints + 1)) * i, lastY + ((float)((int)y - (int)lastY) / (numberPoints + 1)) * i, brush, brushSize, color);
		}
	}
};

}