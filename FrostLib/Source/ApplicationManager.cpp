#include <ApplicationManager.h>
#include <AnimationCurve.h>
#include <Physics.h>
#include <SpriteAnimator.h>
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

		sf::Time deltaTime;
		sf::Clock imguiClock;
		sf::Clock deltaTimeClock;
		sf::Clock appClock;
		unsigned long long frameCount;

		namespace fpsCounter
		{
			namespace
			{
				constexpr float interval = 1.f;
				unsigned long long referenceFrame;
				sf::Clock clock;
			}

			void init()
			{
				while (windowPtr->isOpen())
				{
					fps = (frameCount - referenceFrame) / interval;

					//Reset clock, set current frame and sleep
					sf::Time elapsed = clock.restart();
					referenceFrame = frameCount;
					sf::sleep(sf::seconds(interval));
				}
			}
		}

		namespace loadingScreen
		{
			sf::RectangleShape fadeRect;
			sf::Sprite loadingAnimation;
			SpriteAnimator loadingAnimationAnimator;
			bool solid = true;
			float step = 0.1f;

			namespace
			{
				void fadeOpacity(sf::Uint8 newOpacity, float step)
				{
					auto color = fadeRect.getFillColor();
					fadeRect.setFillColor(sf::Color(color.r, color.g, color.b, Math::lerp(color.a, newOpacity, step)));
				}
			}

			void init()
			{
				//Window size x 2 for safe measures
				fadeRect.setSize(sf::Vector2f(windowPtr->getSize()) * 2.f);
				fadeRect.setOrigin(fadeRect.getSize() / 2.f);
				fadeRect.setPosition(sf::Vector2f(windowPtr->getSize()) / 2.f);
				fadeRect.setFillColor(sf::Color::Black);
				solid = true;
			}

			void fade(bool toBlack, float fadeStep = 0.01f)
			{
				solid = toBlack;
				step = fadeStep;
			}

			void tickFade(float step)
			{
				if (solid)
				{
					if (fadeRect.getFillColor().a == 255) return;
					fadeOpacity(255, step);
				}
				else
				{
					if (fadeRect.getFillColor().a == 0) return;
					fadeOpacity(0, step);
				}
			}

			float logoTimerBegin;
			sf::Sprite loadingImage;
			AnimationCurve loadingImageAnimationGraph;

			void displayLoadingScreen()
			{
				std::string output;
				loadingImage.setTexture(*ResourceMan::getTexture("Common/", "Loading.png", true));
				loadingScreen::loadingImage.setOrigin(loadingScreen::loadingImage.getTexture()->getSize().x / 2, loadingScreen::loadingImage.getTexture()->getSize().y / 2);
				AssetMan::readFile("logoCurve.anim", output, true, "Common/");
				loadingImageAnimationGraph = AnimationCurve(nlohmann::json::parse(output));
			}
		}

		//Pixel rendering
		sf::RenderTexture* buffer;

		std::vector<std::string> imguiDebugBuffer;

		//Thread for loading
		std::thread loadThread;

		void update()
		{
			currentScene.update();
			//Do frame stuff
			 
			loadingScreen::tickFade(deltaTime.asSeconds());
			windowPtr->draw(loadingScreen::fadeRect);
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

		namespace debugGui
		{
			void init()
			{
				ImGui::SFML::Init(*windowPtr);
				windowPtr->resetGLStates();
			}

			void displayDebugGui()
			{
				ImGui::Begin("Debugger");
				ImGui::SetWindowFontScale(1.f);

				std::stringstream fpsStream;
				fpsStream << std::fixed << std::setprecision(2) << fps;
				ImGui::Text("FPS: %s", fpsStream.str().c_str());

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

			void update()
			{
				ImGui::SFML::Update(*windowPtr, imguiClock.restart());
				{
					debugGui::displayDebugGui();
					imguiDebugBuffer.clear();
				}
				ImGui::SFML::Render(*windowPtr);
			}
		}

		//Load the entry scene
		void loadEntryScene()
		{
			loadThread = std::thread(&scene::loadScene, &currentScene, "currentScene");
			loadingScreen::displayLoadingScreen();
			loadingScreen::logoTimerBegin = appClock.getElapsedTime().asSeconds();
		}

		//Process window events
		void pollWindowEvents()
		{
			sf::Event event;
			while (windowPtr->pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					fl::Debug::log("Closing window");
					windowPtr->close();
					currentScene.clearScene();
					return;
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
		}

		//Process input from the user
		void processInput()
		{
			// Handle Input
			mainPlayer.processInput();
			mainPlayer.elapseTimer(deltaTime.asMilliseconds());
		}

		//Clear the screen for drawing
		void clearScreen()
		{
			windowPtr->clear(backgroundColor);
			buffer->clear(sf::Color::Transparent);
		}

		//Fixed timestep timer
		float fixedTimer = 0;
		void processFixedUpdate()
		{
			while (fixedTimer > fixedTimestep)
			{
				fixedTimer -= fixedTimestep;
				fixedUpdate();
			}
			fixedTimer += deltaTime.asMilliseconds();
		}

		//Finalize screen display
		void render(sf::RenderTexture& buf, sf::Sprite& bufferSprite)
		{
			// Render after the scene's code has been run
			buffer->display();
			bufferSprite.setOrigin(pixelSize / 2.f);
			windowDrawElementRelative(bufferSprite);

			// Display the window
			windowPtr->display();
		}

		void init()
		{
			appClock.restart();

			//Initiating window and set viewsize
			windowPtr = new sf::RenderWindow(sf::VideoMode(1060, 600), applicationName, sf::Style::Default);
			sf::View view(sf::Vector2f(), pixelSize);
			windowPtr->setView(view);

			//Create view buffer and sprite
			sf::RenderTexture buf;
			buf.create((int)pixelSize.x, (int)pixelSize.y);
			buffer = &buf;
			sf::Sprite bufferSprite = sf::Sprite(buf.getTexture());

			debugGui::init();
			loadEntryScene();
			std::thread fpsThread(fpsCounter::init);
			loadingScreen::init();

			//Loading cycle
			while (currentScene.loading && windowPtr->isOpen())
			{
				frameCount++;
				pollWindowEvents();
				deltaTime = deltaTimeClock.restart(); //Process deltaTime
				processInput();
				clearScreen();
				processFixedUpdate();
				
				// Render after the scene's code has been run
				float size = loadingScreen::loadingImageAnimationGraph.evaluate(appClock.getElapsedTime().asSeconds() - loadingScreen::logoTimerBegin);
				loadingScreen::loadingImage.setScale(sf::Vector2f(size, size));
				windowPtr->draw(loadingScreen::loadingImage);
				render(buf, bufferSprite);
			}
			loadThread.join(); //Join loading thread when finished playing
			loadingScreen::fade(false);
			while (windowPtr->isOpen())
			{
				frameCount++;
				pollWindowEvents();
				deltaTime = deltaTimeClock.restart();
				processInput();
				clearScreen();
				processFixedUpdate();
				update();

				// Render after the scene's code has been run
				buffer->display();
				bufferSprite.setOrigin(pixelSize / 2.f);
				windowDrawElementRelative(bufferSprite);
				debugGui::update();
				render(buf, bufferSprite);
			}
			fpsThread.join();
			ResourceMan::purgeResources();
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
