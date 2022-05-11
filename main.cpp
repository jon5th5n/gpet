#include "../Platform/Platform.hpp"
#include "../imgui/imgui-SFML.h"
#include "../imgui/imgui.h"

#include "project.h"

int main()
{
	sf::RenderWindow window(sf::VideoMode(1920 * 0.8, 1080 * 0.8), "gpet");
	window.setVerticalSyncEnabled(true);
	ImGui::SFML::Init(window);

	sf::Vector2i previousMousePosition = sf::Mouse::getPosition(window);
	int mouseWheelDelta = 0;

	bool previouslyDrawing = false;

	//---
	// char imagePath[255] = "/home/jonathan/Downloads/testimg.jpg";
	char imagePath[255] = "/home/jonathan/Downloads/colortest.png";

	gpet::Project project({ 1000, 500 });
	project.addLayer("/home/jonathan/Downloads/testimg.jpg");
	sf::Texture texture;
	texture.create(1000, 500);
	texture.update(&project.getPixels()[0]);

	float addColorKey[2] = { 0.0f, 0.0f };

	float projectDisplayScale = 0.8;
	sf::Vector2f projectDisplayPosition { (int)window.getSize().x / 2 - (int)project.getSize().x * projectDisplayScale / 2, (int)window.getSize().y / 2 - (int)project.getSize().y * projectDisplayScale / 2 };
	uint8_t movingCounter = 0;

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
			else if (event.type == sf::Event::Resized)
			{
				sf::FloatRect view(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(view));
			}
			else if (event.type == sf::Event::MouseWheelMoved)
			{
				mouseWheelDelta = event.mouseWheel.delta;
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());

		//---

		ImGui::Begin("Project");

		if (ImGui::CollapsingHeader("Layers", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputTextWithHint("", "/home/llama/pictures/cat1.jpg", imagePath, 255);
			ImGui::SameLine();
			if (ImGui::Button("Add Layer"))
			{
				project.addLayer(std::string(imagePath));
				project.selectedLayer = project.layers.size() - 1;
				texture.update(&project.getPixels()[0]);
			}

			ImGui::BeginTable("Layers", 4);

			ImGui::TableSetupColumn("Selected");
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Move");
			ImGui::TableSetupColumn("Delete");
			ImGui::TableHeadersRow();

			for (int i = project.layers.size() - 1; i >= 0; i--)
			{
				ImGui::PushID(i);

				ImGui::TableNextColumn();
				ImGui::Checkbox("", &project.layers[i].selected);
				ImGui::TableNextColumn();
				if (!project.layers[i].nameBeingEdited)
				{
					if (ImGui::Selectable(project.layers[i].name.c_str(), project.selectedLayer == i))
						project.selectedLayer = i;
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
					{
						project.layers[i].nameBeingEdited = true;
					}
				}
				else
				{
					if (ImGui::InputText("", project.layers[i].editableName, 15, 0b110000) || i != project.selectedLayer)
					{
						project.layers[i].name = project.layers[i].editableName;
						project.layers[i].nameBeingEdited = false;
					}
				}
				ImGui::TableNextColumn();
				if (ImGui::Button("^"))
				{
					std::iter_swap(project.layers.begin() + i, project.layers.begin() + std::clamp(i + 1, 0, (int)project.layers.size() - 1));
					texture.update(&project.getPixels()[0]);
				}
				ImGui::SameLine();
				if (ImGui::Button("v"))
				{
					std::iter_swap(project.layers.begin() + i, project.layers.begin() + std::clamp(i - 1, 0, (int)project.layers.size() - 1));
					texture.update(&project.getPixels()[0]);
				}
				ImGui::TableNextColumn();
				if (ImGui::Button("X"))
				{
					project.layers.erase(std::next(project.layers.begin(), i));
					texture.update(&project.getPixels()[0]);
				}

				ImGui::PopID();
			}

			ImGui::EndTable();
		}

		ImGui::End();

		ImGui::Begin("Utilities");

		if (ImGui::CollapsingHeader("Transforms"))
		{
			if (ImGui::Button("Flip Horizontally"))
			{
				project.layers[project.selectedLayer].image.flipHorizontal();
				texture.update(&project.getPixels()[0]);
			}
			if (ImGui::Button("Flip Vertically"))
			{
				project.layers[project.selectedLayer].image.flipVertical();
				texture.update(&project.getPixels()[0]);
			}

			if (ImGui::Button("Rotate Left"))
			{
				project.layers[project.selectedLayer].image.rotateLeft();
				texture.update(&project.getPixels()[0]);
			}
			ImGui::SameLine();
			if (ImGui::Button("Rotate Right"))
			{
				project.layers[project.selectedLayer].image.rotateRight();
				texture.update(&project.getPixels()[0]);
			}
			if (ImGui::Button("Rotate 180°"))
			{
				project.layers[project.selectedLayer].image.rotate180();
				texture.update(&project.getPixels()[0]);
			}
		}

		if (ImGui::CollapsingHeader("Color Manipulation"))
		{
			if (ImGui::Button("Make Greyscale"))
			{
				project.layers[project.selectedLayer].image.makeGreyScale();
				texture.update(&project.getPixels()[0]);
			}
			if (ImGui::Button("Invert"))
			{
				project.layers[project.selectedLayer].image.invert();
				texture.update(&project.getPixels()[0]);
			}
		}

		if (ImGui::CollapsingHeader("Filter"))
		{
			if (ImGui::Checkbox("Show Filtered", &project.layers[project.selectedLayer].filtered))
			{
				texture.update(&project.getPixels()[0]);
			}

			ImGui::Separator();

			if (ImGui::SliderFloat("Hue", &project.layers[project.selectedLayer].filter.addedHue, 0.0f, 360.0f))
			{
				texture.update(&project.getPixels()[0]);
			}
			if (ImGui::SliderFloat("Saturation", &project.layers[project.selectedLayer].filter.addedSaturation, -1.0f, 1.0f))
			{
				texture.update(&project.getPixels()[0]);
			}
			if (ImGui::SliderFloat("Value", &project.layers[project.selectedLayer].filter.addedValue, -1.0f, 1.0f))
			{
				texture.update(&project.getPixels()[0]);
			}

			ImGui::Separator();

			ImGui::InputFloat2("", addColorKey, "%.1f");
			ImGui::SameLine();
			if (ImGui::Button("Add Key"))
			{
				project.layers[project.selectedLayer].filter.colorKeys.push_back({ std::clamp(addColorKey[0], 0.0f, 360.0f), std::clamp(addColorKey[1], -180.0f, 180.0f) });
				texture.update(&project.getPixels()[0]);
			}

			ImGui::BeginTable("Color Keys", 3);

			ImGui::TableSetupColumn("Hue");
			ImGui::TableSetupColumn("Margin");
			ImGui::TableSetupColumn("");
			ImGui::TableHeadersRow();

			for (uint i = 0; i < project.layers[project.selectedLayer].filter.colorKeys.size(); i++)
			{
				ImGui::TableNextColumn();
				ImGui::Text("%.1f", project.layers[project.selectedLayer].filter.colorKeys[i].first);
				ImGui::TableNextColumn();
				ImGui::Text("%.1f", project.layers[project.selectedLayer].filter.colorKeys[i].second);
				ImGui::TableNextColumn();

				ImGui::PushID(i);
				if (ImGui::Button("Delete"))
				{
					project.layers[project.selectedLayer].filter.colorKeys.erase(std::next(project.layers[project.selectedLayer].filter.colorKeys.begin(), i));
					texture.update(&project.getPixels()[0]);
				}
				ImGui::PopID();
			}

			ImGui::EndTable();

			ImGui::Separator();

			if (ImGui::SliderFloat("Brightness", &project.layers[project.selectedLayer].filter.brightness, 0.0f, 1.0f))
			{
				texture.update(&project.getPixels()[0]);
			}

			if (ImGui::SliderFloat("Red", &project.layers[project.selectedLayer].filter.redInstensity, 0.0f, 1.0f))
			{
				texture.update(&project.getPixels()[0]);
			}
			if (ImGui::SliderFloat("Green", &project.layers[project.selectedLayer].filter.greenInstensity, 0.0f, 1.0f))
			{
				texture.update(&project.getPixels()[0]);
			}
			if (ImGui::SliderFloat("Blue", &project.layers[project.selectedLayer].filter.blueInstensity, 0.0f, 1.0f))
			{
				texture.update(&project.getPixels()[0]);
			}
		}

		if (ImGui::CollapsingHeader("Drawing"))
		{
			ImGui::ColorEdit4("Drawing Color", drawingColor);
			ImGui::Combo("Brush", &currentBrush, brushes, IM_ARRAYSIZE(brushes));
			ImGui::SliderInt("Brush Size", &brushSize, 1, 100, "%d", 0);
		}

		ImGui::End();

		// ---

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				projectDisplayPosition.x += sf::Mouse::getPosition(window).x - previousMousePosition.x;
				projectDisplayPosition.y += sf::Mouse::getPosition(window).y - previousMousePosition.y;
			}

			projectDisplayScale += (float)mouseWheelDelta * 0.05 * projectDisplayScale;
			if (projectDisplayScale < 0.01)
				projectDisplayScale = 0.01;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				project.layers[project.selectedLayer].offset.x += sf::Mouse::getPosition(window).x - previousMousePosition.x;
				project.layers[project.selectedLayer].offset.y += sf::Mouse::getPosition(window).y - previousMousePosition.y;
			}

			project.layers[project.selectedLayer].scale += (float)mouseWheelDelta * 0.05 * project.layers[project.selectedLayer].scale;
			if (project.layers[project.selectedLayer].scale < 0.01)
				project.layers[project.selectedLayer].scale = 0.01;

			movingCounter += 1;
			if (movingCounter % 10 == 0)
				texture.update(&project.getPixels()[0]);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
		{
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				if (previouslyDrawing)
				{
					uint x = (sf::Mouse::getPosition(window).x - projectDisplayPosition.x) / projectDisplayScale;
					uint y = (sf::Mouse::getPosition(window).y - projectDisplayPosition.y) / projectDisplayScale;
					uint lastX = (previousMousePosition.x - projectDisplayPosition.x) / projectDisplayScale;
					uint lastY = (previousMousePosition.y - projectDisplayPosition.y) / projectDisplayScale;

					project.layers[project.selectedLayer].image.draw(x, y, lastX, lastY, currentBrush, brushSize, sf::Color(drawingColor[0] * 255, drawingColor[1] * 255, drawingColor[2] * 255, drawingColor[3] * 255));
					texture.update(&project.getPixels()[0]);
				}

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
		sprite.setPosition(projectDisplayPosition);
		sprite.setScale(projectDisplayScale, projectDisplayScale);
		window.draw(sprite);

		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
}
