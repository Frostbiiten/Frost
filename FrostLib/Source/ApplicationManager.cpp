#include <ApplicationManager.h>

//Application's current scene
FrostLib::Scene currentScene{ "currentScene" };

sf::RenderWindow* windowPtr;

void ApplicationManager::awake()
{
	FrostLib::Debug::log("Application running awake");
}

void ApplicationManager::start()
{
	FrostLib::Debug::log("Application running start");
	//currentScene.loadScene("currentScene");
	currentScene.ui.push_back(FrostLib::UI::UIElement(nullptr, "test element", FrostLib::UI::ScalingType::Distanced, FrostLib::UI::PositionType::Fractional));
	currentScene.ui[currentScene.ui.size() - 1].size = sf::Vector2f(20, 20);
	currentScene.ui.push_back(FrostLib::UI::UIElement(&currentScene.ui.at(0), "test child", FrostLib::UI::ScalingType::Fractional, FrostLib::UI::PositionType::Fractional));
	currentScene.saveScene(4);
}

void ApplicationManager::drawScene(FrostLib::Scene scene)
{
	for (auto& element : scene.ui)
	{
		if (!element.parent)
		{
			element.renderElement(*windowPtr);
		}
	}
}

void ApplicationManager::update()
{
	drawScene(currentScene);
	//currentScene.render(*windowPtr);
	//do frame stuff
}

void ApplicationManager::init()
{
	awake();
	start();

	windowPtr = new sf::RenderWindow(sf::VideoMode(600, 600), applicationName, sf::Style::Default);

	while (windowPtr->isOpen())
	{
		// Process events
		sf::Event event;
		while (windowPtr->pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed)
			{
				FrostLib::Debug::log("Closing window");
				windowPtr->close();
			}
			if (event.type == sf::Event::Resized)
			{
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				windowPtr->setView(sf::View(visibleArea));
				currentScene.invalidateUIDimensions();
			}
		}

		// Clear screen
		windowPtr->clear(backgroundColor);

		// Run frame code 
		update();

		// Update the window
		windowPtr->display();
	}
}
