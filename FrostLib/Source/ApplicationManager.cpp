#include <ApplicationManager.h>
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
		constexpr int fixedTimestep = 20;
		sf::Time deltaTime;
		sf::Clock imguiClock;
		sf::Clock deltaTimeClock;

		inline fl::InputMan::inputMap mainPlayer{ fl::InputMan::keyboardMap{} };
		//std::vector<inputMap> otherPlayers;

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

		void debugInput()
		{
			ImGui::Begin("Input Debugger");

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

			ImGui::SetWindowSize(sf::Vector2f(200, 320));
			ImGui::SetWindowPos(sf::Vector2i(windowPtr->getSize().x - 230, windowPtr->getSize().y / 2 - 160));

			ImGui::End(); // end window
		}

		void init()
		{
			awake();
			start();

			windowPtr = new sf::RenderWindow(sf::VideoMode(1024, 576), applicationName, sf::Style::Default);

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

				//IMGUI
				ImGui::SFML::Update(*windowPtr, imguiClock.restart());
				{
					debugInput();
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
