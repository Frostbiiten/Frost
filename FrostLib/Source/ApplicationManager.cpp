﻿#include <ApplicationManager.h>
#include <AnimationCurve.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include <InputMan.h>

//Application's current scene
fl::Scene currentScene{ "currentScene" };

sf::RenderWindow* windowPtr;

namespace fl
{
	namespace ApplicationManager
	{
		//20 milliseconds = 0.02 seconds ~ 50 times per frame
		sf::Time deltaTime;
		sf::Clock imguiClock;
		sf::Clock deltaTimeClock;

		//Pixel rendering
		sf::RenderTexture* buffer;

		void awake()
		{
			fl::Debug::log("Application running awake");
			currentScene.awake();
		}

		void start()
		{
			fl::Debug::log("Application running start");
			currentScene.start();
		}

		void drawScene(fl::Scene scene)
		{
			for (auto& element : scene.ui)
			{
				if (!element.parent)
				{
					element.renderElement(*windowPtr);
				}
			}
		}

		void update()
		{
			currentScene.update();
			//Do frame stuff
		}

		void fixedUpdate()
		{
			currentScene.fixedUpdate();
			//Runs on a fixed step
		}

		void debugGui()
		{
			ImGui::Begin("Debugger");

			std::stringstream fpsStream;
			fpsStream << std::fixed << std::setprecision(2) << fps;
			ImGui::Text("FPS: %s", fpsStream.str().c_str());

			ImGui::Text("");
			ImGui::Separator();
			ImGui::Text("");

			ImGui::Text("Player Input Manager");
			ImGui::Text("Directional Input:\n (%f, %f)", mainPlayer.directionalInput.x, mainPlayer.directionalInput.y);
			ImGui::Text("Mouse Position:\n (%d, %d)", mainPlayer.pointerPos.x, mainPlayer.pointerPos.y);

			ImGui::Text("");
			ImGui::Separator();
			ImGui::Text("");

			ImGui::Checkbox("Clicking", &mainPlayer.isClicking);
			ImGui::Checkbox("Button A", &mainPlayer.button1);
			ImGui::Checkbox("Button B", &mainPlayer.button2);
			ImGui::Checkbox("Button C", &mainPlayer.button3);
			ImGui::Checkbox("Button D", &mainPlayer.button4);
			ImGui::Checkbox("Special Button A", &mainPlayer.plus);
			ImGui::Checkbox("Special Button B", &mainPlayer.minus);

			ImGui::SetWindowSize(sf::Vector2f(200, 400));
			ImGui::SetWindowPos(sf::Vector2i(windowPtr->getSize().x - 230, windowPtr->getSize().y / 2 - 200));

			ImGui::End(); // end window
		}

		void init()
		{
			awake();

			//Initiating window and set viewsize
			windowPtr = new sf::RenderWindow(sf::VideoMode(1060, 600), applicationName, sf::Style::Default);
			sf::View view(sf::Vector2f(), pixelSize);
			windowPtr->setView(view);
			
			//Create view buffer and sprite
			sf::RenderTexture buf;
			buf.create(pixelSize.x, pixelSize.y);
			buffer = &buf;
			sf::Sprite bufferSprite = sf::Sprite(buf.getTexture());
			
			start();

			//Fixed timestep timer
			float fixedTimer = 0;

			//IMGUI
			ImGui::SFML::Init(*windowPtr);
			windowPtr->resetGLStates();

			while (windowPtr->isOpen())
			{
				// Process events
				sf::Event event;
				while (windowPtr->pollEvent(event))
				{
					if (event.type == sf::Event::Closed)
					{
						fl::Debug::log("Closing window");
						windowPtr->close();
					}
					if (event.type == sf::Event::Resized)
					{
						sf::Vector2u size = windowPtr->getSize();
						float heightRatio = 30.f / 53.f;
						float widthRatio = 53.f / 30.f;
						if (size.y * widthRatio <= size.x)
							size.x = size.y * widthRatio;
						else if (size.x * heightRatio <= size.y)
							size.y = size.x * heightRatio;
						windowPtr->setSize(size);
						currentScene.invalidateUIDimensions();
					}
				}

				// Handle Deltatime + fps
				deltaTime = deltaTimeClock.restart();
				fps = 1000000.f / deltaTime.asMicroseconds();

				// Handle Input
				mainPlayer.processInput();

				// Clear screen
				windowPtr->clear(backgroundColor);

				// Handle fixedUpdate
				while (fixedTimer > fixedTimestep)
				{
					fixedTimer -= fixedTimestep;
					fixedUpdate();
				}
				fixedTimer += deltaTime.asMilliseconds();

				// Run frame code 
				update();

				// Render after the scene's code has been run
				drawScene(currentScene);
				buffer->display();
				windowPtr->draw(bufferSprite);

				//IMGUI
				ImGui::SFML::Update(*windowPtr, imguiClock.restart());
				{
					debugGui();
				}
				ImGui::SFML::Render(*windowPtr);

				// Update the window
				windowPtr->display();
			}
		}

		sf::RenderWindow* getWindow()
		{
			return windowPtr;
		}
	}
}
