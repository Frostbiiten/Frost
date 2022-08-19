#include <AppMan.h>
#include <Transform.h>
#include <SceneMan.h>

namespace fl
{
	namespace AppMan
	{
		// Respective window and pixel rendering targets
		sf::RenderWindow* windowPtr;
		sf::RenderTexture* buffer;

		// Clock to measure app time
		sf::Clock appClock;

		//Process window events
		void PollWindowEvents()
		{
			sf::Event event;
			while (windowPtr->pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					Debug::log()->info("Closing window");
					windowPtr->close();
					return;
				}
				if (event.type == sf::Event::Resized)
				{
					sf::Vector2u size = windowPtr->getSize();

					// Constant aspect ratio resizing
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

		//Clear the screen for drawing
		void ClearScreen()
		{
			windowPtr->clear(backgroundColor);
			buffer->clear(sf::Color::Transparent);
		}

		//Finalize screen display
		void Render(sf::RenderTexture& buf, sf::Sprite& bufferSprite)
		{
			// Render after the scene's code has been run
			buffer->display();
			bufferSprite.setOrigin(pixelSize / 2.f);
			windowDrawElementRelative(bufferSprite);

			// Display the window
			windowPtr->display();
		}

		// ENTRY POINT
		void Init()
		{
			Debug::init();

			// Reset app clock
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

			// Ready loops
			SceneMan::Reset(*windowPtr, *buffer);

			// Main loop
			while (windowPtr->isOpen())
			{
				PollWindowEvents();
				ClearScreen();

				SceneMan::Tick();

				// Render BUFFER after the scene's code has been run
				Render(buf, bufferSprite);
			}

			ResourceMan::purgeResources();
		}

		// Drawing
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
