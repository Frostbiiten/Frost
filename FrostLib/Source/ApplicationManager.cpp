#include <ApplicationManager.h>
#include <AnimationCurve.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include <InputMan.h>
#include <thread>

namespace fl
{
	namespace ApplicationManager
	{
		//Application's current scene
		fl::scene currentScene{ "currentScene" };
		sf::RenderWindow* windowPtr;

		//20 milliseconds = 0.02 seconds ~ 50 times per frame
		sf::Time deltaTime;
		sf::Clock imguiClock;
		sf::Clock deltaTimeClock;

		//Pixel rendering
		sf::RenderTexture* buffer;

		std::vector<std::string> imguiDebugBuffer;

		void awake()
		{
			fl::Debug::log("Application running awake");
			currentScene.loadScene("currentScene");
			currentScene.awake();
		}

		void start()
		{
			fl::Debug::log("Application running start");
			currentScene.start();
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

		void imGuiText(std::string text)
		{
			imguiDebugBuffer.push_back(text);
		}

		void debugGui()
		{
			ImGui::Begin("Debugger");
			ImGui::SetWindowFontScale(1.f);

			std::stringstream fpsStream;
			fpsStream << std::fixed << std::setprecision(2) << fps;
			ImGui::Text("FPS: %s", fpsStream.str().c_str());

			ImGui::Text("");
			ImGui::Separator();
			ImGui::Text("");


			for (auto& item : imguiDebugBuffer)
			{
				ImGui::Text(item.c_str());
			}

			ImGui::SetWindowSize(sf::Vector2f(220, 400));
			ImGui::SetWindowPos(sf::Vector2i(windowPtr->getSize().x - 230, windowPtr->getSize().y / 2 - 200));

			ImGui::End(); // end window
		}

		void init()
		{
			//Run awake
			awake();

			//Initiating window and set viewsize
			windowPtr = new sf::RenderWindow(sf::VideoMode(1060, 600), applicationName, sf::Style::Default);
			sf::View view(sf::Vector2f(), pixelSize);
			windowPtr->setView(view);

			//Create view buffer and sprite
			sf::RenderTexture buf;
			buf.create((int)pixelSize.x, (int)pixelSize.y);
			buffer = &buf;
			sf::Sprite bufferSprite = sf::Sprite(buf.getTexture());

			//Run start
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
						constexpr float heightRatio = 30.f / 53.f;
						constexpr float widthRatio = 53.f / 30.f;
						#pragma warning(push)
						#pragma warning(disable: 4244)
						if (size.y * widthRatio <= size.x)
							size.x = size.y * widthRatio;
						else if (size.x * heightRatio <= size.y)
							size.y = size.x * heightRatio;
						#pragma warning(pop)
						windowPtr->setSize(size);
					}
				}

				// Handle Deltatime + fps
				deltaTime = deltaTimeClock.restart();
				fps = 1000000.f / deltaTime.asMicroseconds();

				// Handle Input
				mainPlayer.processInput();
				mainPlayer.elapseTimer(deltaTime.asMilliseconds());

				// Clear screen
				windowPtr->clear(backgroundColor);
				buffer->clear(sf::Color::Transparent);

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
				buffer->display();
				bufferSprite.setOrigin(pixelSize / 2.f);
				windowDrawElementRelative(bufferSprite);

				//IMGUI
				ImGui::SFML::Update(*windowPtr, imguiClock.restart());
				{
					debugGui();
					imguiDebugBuffer.clear();
				}
				ImGui::SFML::Render(*windowPtr);

				// Display the window
				windowPtr->display();
			}
		}

		sf::RenderWindow* getWindow()
		{
			return windowPtr;
		}
		void windowDrawElement(sf::Drawable& drawable)
		{
			windowPtr->draw(drawable);
		}
		void windowDrawElementRelative(sf::Drawable& drawable)
		{
			//Keep track of old view
			sf::View cachedView = windowPtr->getView();

			//Change view to default view
			windowPtr->setView(sf::View(sf::Vector2f(), pixelSize));

			//Render drawable to window
			windowPtr->draw(drawable);

			//Change back to old view
			windowPtr->setView(cachedView);
		}

		sf::RenderTexture* getBuffer()
		{
			return buffer;
		}
		void bufferDrawElement(sf::Drawable& drawable)
		{
			buffer->draw(drawable);
		}
		void bufferDrawElementRelative(sf::Drawable& drawable)
		{
			//Keep track of old view
			sf::View cachedView = buffer->getView();

			//Change view to default view
			buffer->setView(sf::View(sf::Vector2f(), pixelSize));

			//Render drawable to window
			buffer->draw(drawable);

			//Change back to old view
			buffer->setView(cachedView);
		}
	}
}
