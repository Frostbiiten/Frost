#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <uuid.h>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <time.h>

extern sf::RenderWindow* windowPtr;

namespace fl
{
#define STRINGIFY(X) #X
	
	inline sf::Vector2f getWindowSize()
	{
		return sf::Vector2f(windowPtr->getSize());
	}

	template<typename T>
	inline sf::Vector2<T> multiply(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
	{
		return sf::Vector2<T>(lhs.x * rhs.x, lhs.y * rhs.y);
	}

	template<typename T>
	inline sf::Vector2<T> multiply(const sf::Vector2<T>& lhs, const int& rhs)
	{
		return sf::Vector2<T>(lhs.x * rhs, lhs.y * rhs);
	}

	template<typename T>
	inline sf::Vector2<T> divide(const sf::Vector2<T>& lhs, const sf::Vector2<T>& rhs)
	{
		return sf::Vector2<T>(lhs.x / rhs.x, lhs.y / rhs.y);
	}

	inline std::string generateUUID()
	{
		uuids::uuid_random_generator gen;
		uuids::uuid guid = gen();
		return uuids::to_string(guid);
	}

	//https://stackoverflow.com/a/57399634
	template <typename t>
	void move(std::vector<t>& v, size_t oldIndex, size_t newIndex)
	{
		if (oldIndex > newIndex)
			std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
		else
			std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
	}

	//Bitwise operations
	template<class T> inline T operator~ (T a) { return (T)~(int)a; }
	template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
	template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
	template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
	template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
	template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
	template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }
}