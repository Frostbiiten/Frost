#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace fl
{
	namespace InputMan
	{
		struct inputMap
		{
			private:
			bool button1Old, button2Old, button3Old, button4Old;
			void processDeltaInput();

			public:
			//BUTTONS
			//Main directional input
			sf::Vector2f directionalInput;
			
			//Main mouse/pointer position
			sf::Vector2i pointerPos;
			bool isClicking;

			//Generic remappable buttons
			bool button1, button2, button3, button4;
			bool button1Down, button2Down, button3Down, button4Down;
			bool button1Up, button2Up, button3Up, button4Up;

			//Non-control buttons
			bool plus;
			bool minus;

			//If input is locked (timers can be used to stun player, etc.)
			bool locked;
			bool directionalLocked;

			int timerMs;
			int directionalTimerMs;
			bool directionalManualTick;

			//Lock and unlock input for ms milliseconds;
			void lockInput(int ms);

			//Lock and unlock directional input for ms milliseconds;
			void lockDirectionalInput(int ms);

			//Elapse the lock timer using the deltaTime between current and last frame
			void elapseTimer(int delta);

			//Defaults to keyboard style
			virtual void processInput();
		};

		//Inputmap for keyboard
		class keyboardMap : public inputMap
		{
			//Directional input
			sf::Keyboard::Key left;
			sf::Keyboard::Key right;
			sf::Keyboard::Key up;
			sf::Keyboard::Key down;

			//Directional input
			sf::Mouse::Button clickButton;

			//Control buttons input
			sf::Keyboard::Key button1Key;
			sf::Keyboard::Key button2Key;
			sf::Keyboard::Key button3Key;
			sf::Keyboard::Key button4Key;

			//Non-control buttons input
			sf::Keyboard::Key plusKey;
			sf::Keyboard::Key minusKey;

		public:
			keyboardMap
			(
				sf::Keyboard::Key left = sf::Keyboard::Key::A,
				sf::Keyboard::Key right = sf::Keyboard::Key::D,
				sf::Keyboard::Key up = sf::Keyboard::Key::W,
				sf::Keyboard::Key down = sf::Keyboard::Key::S,
				sf::Mouse::Button clickButton = sf::Mouse::Button::Left,
				sf::Keyboard::Key button1Key = sf::Keyboard::U,
				sf::Keyboard::Key button2Key = sf::Keyboard::I,
				sf::Keyboard::Key button3Key = sf::Keyboard::O,
				sf::Keyboard::Key button4Key = sf::Keyboard::P,
				sf::Keyboard::Key plusKey = sf::Keyboard::Enter,
				sf::Keyboard::Key minusKey = sf::Keyboard::RShift
			);

			~keyboardMap();

			void processInput();
		};
	}
}
