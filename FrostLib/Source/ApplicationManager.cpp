#include <ApplicationManager.h>
#include <imgui.h>
#include <imgui-SFML.h>

//Application's current scene
fl::Scene currentScene{ "currentScene" };

sf::RenderWindow* windowPtr;

namespace fl
{
	namespace ApplicationManager
	{
		//20 milliseconds = 0.02 seconds ~ 50 times per frame
		constexpr int fixedTimestep = 20;
		sf::Time deltaTime;
		sf::Clock imguiClock;
		sf::Clock deltaTimeClock;

		void awake()
		{
			fl::Debug::log("Application running awake");
		}

		void start()
		{
			fl::Debug::log("Application running start");
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
			//Do frame stuff
		}

		void fixedUpdate()
		{
			//Runs on a fixed step
		}

		void init()
		{
			awake();
			start();

			windowPtr = new sf::RenderWindow(sf::VideoMode(900, 600), applicationName, sf::Style::Default);

			//IMGUI
			ImGui::SFML::Init(*windowPtr);
			windowPtr->resetGLStates();

			float fixedTimer = 0;
			while (windowPtr->isOpen())
			{
				// Process events
				sf::Event event;
				while (windowPtr->pollEvent(event))
				{
					// Close window: exit
					if (event.type == sf::Event::Closed)
					{
						fl::Debug::log("Closing window");
						windowPtr->close();
					}
					if (event.type == sf::Event::Resized)
					{
						sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
						windowPtr->setView(sf::View(visibleArea));
						currentScene.invalidateUIDimensions();
					}
				}

				// Handle Deltatime
				deltaTime = deltaTimeClock.restart();

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

				//IMGUI
				ImGui::SFML::Update(*windowPtr, imguiClock.restart());
				{
					ImGui::Begin("Testing ui");

					// Window title text edit
					//ImGui::InputText("Window title", "hello", 255);

					if (ImGui::Button("Button"))
					{

					}

					ImGui::SetWindowSize("Testing ui", sf::Vector2f(500, 200));

					ImGui::End(); // end window
				}

				ImGui::SFML::Render(*windowPtr);

				// Update the window
				windowPtr->display();
			}
		}
	}
}
