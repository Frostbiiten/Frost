#include <InputMan.h>
#include <ApplicationManager.h>

namespace fl
{
	namespace InputMan
	{
		void inputMap::processInput()
		{
			//Directional
			directionalInput = sf::Vector2f(0, 0);
			directionalInput.x += sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
			directionalInput.x -= sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
			directionalInput.y += sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
			directionalInput.y -= sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);

			//Position relative to window
			pointerPos = sf::Mouse::getPosition(*(fl::ApplicationManager::getWindow()));

			//Clicking
			isClicking = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

			//4 main buttons
			button1 = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::U);
			button2 = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::I);
			button3 = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O);
			button4 = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P);

			//Non-control buttons
			plus = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter);
			minus = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);
		}

		keyboardMap::keyboardMap
		(
			sf::Keyboard::Key left,
			sf::Keyboard::Key right,
			sf::Keyboard::Key up,
			sf::Keyboard::Key down,
			sf::Mouse::Button clickButton,
			sf::Keyboard::Key button1Key,
			sf::Keyboard::Key button2Key,
			sf::Keyboard::Key button3Key,
			sf::Keyboard::Key button4Key,
			sf::Keyboard::Key plusKey,
			sf::Keyboard::Key minusKey
		)
		{
			this->left = left;
			this->right = right;
			this->up = up;
			this->down = down;
			this->clickButton = clickButton;
			this->button1Key = button1Key;
			this->button2Key = button2Key;
			this->button3Key = button3Key;
			this->button4Key = button4Key;
			this->plusKey = plusKey;
			this->minusKey = minusKey;
		}

		void keyboardMap::processInput()
		{
			//Directional
			directionalInput = sf::Vector2f(0, 0);
			directionalInput.x += sf::Keyboard::isKeyPressed(right);
			directionalInput.x -= sf::Keyboard::isKeyPressed(left);
			directionalInput.y += sf::Keyboard::isKeyPressed(up);
			directionalInput.y -= sf::Keyboard::isKeyPressed(down);

			//Position relative to window
			pointerPos = sf::Mouse::getPosition(*(fl::ApplicationManager::getWindow()));

			//Clicking
			isClicking = sf::Mouse::isButtonPressed(clickButton);

			//4 main buttons
			button1 = sf::Keyboard::isKeyPressed(button1Key);
			button2 = sf::Keyboard::isKeyPressed(button2Key);
			button3 = sf::Keyboard::isKeyPressed(button3Key);
			button4 = sf::Keyboard::isKeyPressed(button4Key);

			//Non-control buttons
			plus = sf::Keyboard::isKeyPressed(plusKey);
			minus = sf::Keyboard::isKeyPressed(minusKey);
		}

		keyboardMap::~keyboardMap()
		{
			//Destructor
		}
	}
}