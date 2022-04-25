#include "../Platform/Platform.hpp"
#include "../imgui/imgui-SFML.h"
#include "../imgui/imgui.h"

#include "hsv.h"
#include "image.h"
#include "rgb.h"

int main()
{
	sf::RenderWindow window(sf::VideoMode(1920 / 2, 1080 / 2), "gpet");
	window.setVerticalSyncEnabled(true);
	ImGui::SFML::Init(window);

	sf::Vector2i previousMousePosition = sf::Mouse::getPosition(window);
	int mouseWheelDelta = 0;

	bool previouslyDrawing = false;

	//---
	// char imagePath[255] = "/home/jonathan/Downloads/testimg.jpg";
	char imagePath[255] = "/home/jonathan/Downloads/colortest.png";

	gpet::Image image;
	sf::Texture texture;

	bool saveImageFiltered = true;
	float addColorKey[2] = { 0.0f, 0.0f };

	bool showImagedFiltered = false;
	float imageDisplayScale = 0.5;
	sf::Vector2f imageDisplayPosition(0, 0);

	float drawingColor[4] = { 0, 0, 0, 1 };
	int brushSize = 5;
	const char* brushes[] = { "Square", "Circle" };
	int currentBrush = 1;

	//---

	window.resetGLStates();
	sf::Clock deltaClock;
	while (window.isOpen())
	{
		mouseWheelDelta = 0;

		sf::Event event;
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			if (event.type == sf::Event::MouseWheelMoved)
			{
				mouseWheelDelta = event.mouseWheel.delta;
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		//---

		ImGui::Begin("Utilities");

		if (ImGui::CollapsingHeader("Load/Save"))
		{
			ImGui::InputTextWithHint("File Path", "/home/llama/pictures/cat1.jpg", imagePath, 255);
			if (ImGui::Button("Load Image"))
			{
				image.loadFromFile(std::string(imagePath));
				texture.create(image.getSize().x, image.getSize().y);
				texture.update(image.getPixelArray(showImagedFiltered));
			}
			ImGui::SameLine();
			if (ImGui::Button("Save Image"))
			{
				image.saveToFile(std::string(imagePath), saveImageFiltered);
			}
			ImGui::SameLine();
			ImGui::Checkbox("filtered", &saveImageFiltered);
		}

		ImGui::Separator();

		if (ImGui::CollapsingHeader("Transforms"))
		{
			if (ImGui::Button("Flip Horizontally"))
			{
				image.flipHorizontal();
				texture.update(image.getPixelArray(showImagedFiltered));
			}
			if (ImGui::Button("Flip Vertically"))
			{
				image.flipVertical();
				texture.update(image.getPixelArray(showImagedFiltered));
			}

			if (ImGui::Button("Rotate Left"))
			{
				image.rotateLeft();
				texture.create(image.getSize().x, image.getSize().y);
				texture.update(image.getPixelArray(showImagedFiltered));
			}
			ImGui::SameLine();
			if (ImGui::Button("Rotate Right"))
			{
				image.rotateRight();
				texture.create(image.getSize().x, image.getSize().y);
				texture.update(image.getPixelArray(showImagedFiltered));
			}
			if (ImGui::Button("Rotate 180°"))
			{
				image.rotate180();
				texture.update(image.getPixelArray(showImagedFiltered));
			}
		}

		if (ImGui::CollapsingHeader("Color Manipulation"))
		{
			if (ImGui::Button("Make Greyscale"))
			{
				image.makeGreyScale();
				texture.update(image.getPixelArray(showImagedFiltered));
			}
		}

		if (ImGui::CollapsingHeader("Filter"))
		{
			if (ImGui::Checkbox("Show Filtered", &showImagedFiltered))
			{
				texture.update(image.getPixelArray(showImagedFiltered));
			}

			ImGui::Separator();

			if (ImGui::SliderFloat("Hue", &image.addedHue, 0.0f, 360.0f))
			{
				texture.update(image.getPixelArray(showImagedFiltered));
			}
			if (ImGui::SliderFloat("Saturation", &image.addedSaturation, -1.0f, 1.0f))
			{
				texture.update(image.getPixelArray(showImagedFiltered));
			}
			if (ImGui::SliderFloat("Value", &image.addedValue, -1.0f, 1.0f))
			{
				texture.update(image.getPixelArray(showImagedFiltered));
			}

			ImGui::Separator();

			ImGui::InputFloat2("", addColorKey, "%.1f");
			ImGui::SameLine();
			if (ImGui::Button("Add Key"))
			{
				image.colorKeys.push_back({ std::clamp(addColorKey[0], 0.0f, 360.0f), std::clamp(addColorKey[1], -180.0f, 180.0f) });
				texture.update(image.getPixelArray(showImagedFiltered));
			}

			ImGui::BeginTable("Color Keys", 3);

			ImGui::TableSetupColumn("Hue");
			ImGui::TableSetupColumn("Margin");
			ImGui::TableSetupColumn("");
			ImGui::TableHeadersRow();

			for (uint i = 0; i < image.colorKeys.size(); i++)
			{
				ImGui::TableNextColumn();
				ImGui::Text("%.1f", image.colorKeys[i].first);
				ImGui::TableNextColumn();
				ImGui::Text("%.1f", image.colorKeys[i].second);
				ImGui::TableNextColumn();

				ImGui::PushID(i);
				if (ImGui::Button("Delete"))
				{
					image.colorKeys.erase(std::next(image.colorKeys.begin(), i));
					texture.update(image.getPixelArray(showImagedFiltered));
				}
				ImGui::PopID();
			}

			ImGui::EndTable();
		}

		if (ImGui::CollapsingHeader("Drawing"))
		{
			ImGui::ColorEdit4("Drawing Color", drawingColor);
			ImGui::Combo("Brush", &currentBrush, brushes, IM_ARRAYSIZE(brushes));
			ImGui::SliderInt("Brush Size", &brushSize, 1, 100, "%d", 0);
		}

		ImGui::End();

		//---

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				imageDisplayPosition.x += sf::Mouse::getPosition(window).x - previousMousePosition.x;
				imageDisplayPosition.y += sf::Mouse::getPosition(window).y - previousMousePosition.y;
			}

			imageDisplayScale += (float)mouseWheelDelta * 0.05 * imageDisplayScale;
			if (imageDisplayScale < 0.01)
				imageDisplayScale = 0.01;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				if (previouslyDrawing)
					image.draw((sf::Mouse::getPosition(window).x - imageDisplayPosition.x) / imageDisplayScale, (sf::Mouse::getPosition(window).y - imageDisplayPosition.y) / imageDisplayScale, (previousMousePosition.x - imageDisplayPosition.x) / imageDisplayScale, (previousMousePosition.y - imageDisplayPosition.y) / imageDisplayScale, currentBrush, brushSize, sf::Color(drawingColor[0] * 255, drawingColor[1] * 255, drawingColor[2] * 255, drawingColor[3] * 255));
				texture.update(image.getPixelArray(showImagedFiltered));

				previouslyDrawing = true;
			}
			else
				previouslyDrawing = false;
		}
		else
			previouslyDrawing = false;

		previousMousePosition = sf::Mouse::getPosition(window);

		//---

		window.clear(sf::Color::Black);

		sf::Sprite sprite;
		sprite.setTexture(texture);
		sprite.setPosition(imageDisplayPosition);
		sprite.setScale(imageDisplayScale, imageDisplayScale);
		window.draw(sprite);

		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
}
