#include "Graph.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <execution>

namespace fl
{
	//Base class
	GraphNode::GraphNode()
	{
		point = sf::Vector2f(0, 0);
	}
	GraphNode::GraphNode(float x, float y)
	{
		point = sf::Vector2f(x, y);
	}
	GraphNode::GraphNode(sf::Vector2f point)
	{
		this->point = point;
	}
	sf::Vector2f GraphNode::getHandle(bool side)
	{
		return point;
	}
	sf::Vector2f GraphNode::getLerpPos(bool side, float t)
	{
		return point;
	}
	void GraphNode::setPosition(sf::Vector2f pos)
	{
		point = pos;
	}
	void GraphNode::setHandle(sf::Vector2f pos, bool side) { return; }

	//Beziernode
	BezierNode::BezierNode(sf::Vector2f point, sf::Vector2f handle)
	{
		this->point = point;
		this->handle = handle;
	}
	sf::Vector2f BezierNode::getHandle(bool side)
	{
		return side ? point + handle : point - handle;
	}
	sf::Vector2f BezierNode::getLerpPos(bool side, float t)
	{
		return lerp(point, getHandle(side), t);
	}
	void BezierNode::setPosition(sf::Vector2f pos)
	{
		point = pos;
	}
	void BezierNode::setHandle(sf::Vector2f pos, bool side)
	{
		side ? handle = pos : handle = -pos;
	}

	//Uneven bezier
	UnevenBezierNode::UnevenBezierNode(sf::Vector2f point, sf::Vector2f handleL, sf::Vector2f handleR)
	{
		this->point = point;
		this->handleL = handleL;
		this->handleR = handleR;
	}
	sf::Vector2f UnevenBezierNode::getHandle(bool side)
	{
		return side ? point + handleR : point + handleL;
	}
	sf::Vector2f UnevenBezierNode::getLerpPos(bool side, float t)
	{
		if (side)
			return lerp(point, getHandle(side), t);
		else
			return lerp(getHandle(side), point, t);
	}
	void UnevenBezierNode::setPosition(sf::Vector2f pos)
	{
		point = pos;
	}
	void UnevenBezierNode::setHandle(sf::Vector2f pos, bool side)
	{
		side ? handleR = pos : handleL = pos;
	}

	//Graph
	Graph::Graph(bool loop)
	{
		this->loop = loop;
		length = 0;
	}
	void Graph::addNode(sf::Vector2f position, int index)
	{
		if (graph.size() < 1)
		{
			graph.push_back(std::make_unique<GraphNode>(position));
			length = 0;
			return;
		}

		//Clamp to range of vector
		index = std::max(std::min(index, (int)graph.size()), 0);

		if (graph.size() < 2)
		{
			graph.insert(graph.begin() + index, std::make_unique<GraphNode>(position));
			nodeLengths.push_back(getLength(*graph[0], *graph[1]));
			length = nodeLengths[0];
			return;
		}

		int prevIndex = index - 1;
		int nextIndex = index + 1;

		graph.insert(graph.begin() + index, std::make_unique<GraphNode>(position));

		//If at end or beginning of vector
		if (index == 0)
		{
			nodeLengths.insert(nodeLengths.begin(), getLength(*graph[nextIndex], *graph[index]));
			updateLength();
			return;
		}
		if (index == graph.size() - 1)
		{
			nodeLengths.insert(nodeLengths.end(), getLength(*graph[prevIndex], *graph[index]));
			updateLength();
			return;
		}

		//If not at beginning nor end
		nodeLengths[prevIndex] = getLength(*graph[prevIndex], *graph[index]);
		nodeLengths.insert(nodeLengths.begin() + index, getLength(*graph[nextIndex], *graph[index]));
		updateLength();
	}
	void Graph::addNode(sf::Vector2f position, sf::Vector2f handle, int index)
	{
		if (graph.size() < 1)
		{
			graph.push_back(std::make_unique<BezierNode>(position, handle));
			length = 0;
			return;
		}

		//Clamp to range of vector
		index = std::max(std::min(index, (int)graph.size()), 0);

		if (graph.size() < 2)
		{
			graph.insert(graph.begin() + index, std::make_unique<BezierNode>(position, handle));
			nodeLengths.push_back(getLength(*graph[0], *graph[1]));
			length = nodeLengths[0];
			return;
		}

		int prevIndex = index - 1;
		int nextIndex = index + 1;

		graph.insert(graph.begin() + index, std::make_unique<BezierNode>(position, handle));

		//If at end or beginning of vector
		if (index == 0)
		{
			nodeLengths.insert(nodeLengths.begin(), getLength(*graph[nextIndex], *graph[index]));
			updateLength();
			return;
		}
		if (index == graph.size() - 1)
		{
			nodeLengths.insert(nodeLengths.end(), getLength(*graph[prevIndex], *graph[index]));
			updateLength();
			return;
		}

		//If not at beginning nor end
		nodeLengths[prevIndex] = getLength(*graph[prevIndex], *graph[index]);
		nodeLengths.insert(nodeLengths.begin() + index, getLength(*graph[nextIndex], *graph[index]));
		updateLength();
	}
	void Graph::addNode(sf::Vector2f position, sf::Vector2f handleL, sf::Vector2f handleR, int index)
	{
		if (graph.size() < 1)
		{
			graph.push_back(std::make_unique<UnevenBezierNode>(position, handleL, handleR));
			length = 0;
			return;
		}

		//Clamp to range of vector
		index = std::max(std::min(index, (int)graph.size()), 0);

		if (graph.size() < 2)
		{
			graph.insert(graph.begin() + index, std::make_unique<UnevenBezierNode>(position, handleL, handleR));
			nodeLengths.push_back(getLength(*graph[0], *graph[1]));
			length = nodeLengths[0];
			return;
		}

		int prevIndex = index - 1;
		int nextIndex = index + 1;

		graph.insert(graph.begin() + index, std::make_unique<UnevenBezierNode>(position, handleL, handleR));

		//If at end or beginning of vector
		if (index == 0)
		{
			nodeLengths.insert(nodeLengths.begin(), getLength(*graph[nextIndex], *graph[index]));
			updateLength();
			return;
		}
		if (index == graph.size() - 1)
		{
			nodeLengths.insert(nodeLengths.end(), getLength(*graph[prevIndex], *graph[index]));
			updateLength();
			return;
		}

		//If not at beginning nor end
		nodeLengths[prevIndex] = getLength(*graph[prevIndex], *graph[index]);
		nodeLengths.insert(nodeLengths.begin() + index, getLength(*graph[nextIndex], *graph[index]));
		updateLength();
	}
	void Graph::moveNode(sf::Vector2f newPosition, int index)
	{
		graph[index]->setPosition(newPosition);
	}
	void Graph::moveHandle(sf::Vector2f newPosition, int index, bool side)
	{
		graph[index]->setHandle(newPosition, side);
	}
	sf::Vector2f Graph::evaluateDistance(float distance)
	{
		float accumulatedDist = 0.f;
		int index = 0;
		for (size_t x = 1; x < graph.size(); x++)
		{
			if (x != 0)
				accumulatedDist += nodeLengths[x - 1];
			if (accumulatedDist > distance)
			{
				index = x;
				break;
			}
			if (x == graph.size() - 1)
			{
				if (loop)
				{
					x = 0;
					accumulatedDist += getLength(*graph[x], *graph[0]);
				}
				else
					return graph[x]->point;
			}
		}

		int prevIndex = index - 1;
		
		//For looping
		if (prevIndex < 0)
			prevIndex = graph.size() - 1;

		float nodeADist = std::reduce(nodeLengths.begin(), nodeLengths.begin() + prevIndex);
		float nodeBDist = std::reduce(nodeLengths.begin(), nodeLengths.begin() + index);

		if (loop && accumulatedDist > length)
		{
			accumulatedDist = fmodf(length, accumulatedDist);
		}

		float t = inverseLerp(nodeADist, nodeBDist, distance);
		return evaluate(*graph[prevIndex], *graph[index], t);
	}
	void Graph::updateLength()
	{
		length = loop ? std::reduce(std::execution::par, nodeLengths.begin(), nodeLengths.end()) + getLength(*graph[0], *graph[graph.size() - 1])
			: std::reduce(std::execution::par, nodeLengths.begin(), nodeLengths.end());
	}

	sf::Vector2f lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t)
	{
		return a + (b - a) * t;
	}
	float inverseLerp(const float& a, const float& b, const float& value)
	{
		return (value - a) / (b - a);
	}
	float Graph::getLength(GraphNode& a, GraphNode& b, float step)
	{
		float t = 0;
		sf::Vector2f currentPos = a.point;
		sf::Vector2f oldPos = currentPos;
		float length = 0;
		while (t < 1)
		{
			currentPos = evaluate(a, b, t);
			auto a1 = powf(oldPos.x - currentPos.x, 2) + powf(oldPos.y - currentPos.y, 2);
			auto a2 = sqrtf(a1);
			length += sqrtf(powf(oldPos.x - currentPos.x, 2) + powf(oldPos.y - currentPos.y, 2));
			oldPos = currentPos;
			t += step;
		}
		return length;
	}
	sf::Vector2f evaluate(GraphNode& a, GraphNode& b, float t)
	{
		auto p0 = lerp(a.getHandle(true), b.getHandle(false), t);
		auto p1 = lerp(a.getLerpPos(true, t), p0, t);
		auto p2 = lerp(p0, b.getLerpPos(false, t), t);
		return lerp(p1, p2, t);
	}
}
