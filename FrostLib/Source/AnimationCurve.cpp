#include <AnimationCurve.h>
#include <MathUtil.h>
#include <iostream>

namespace fl
{
	AnimationCurve::AnimationCurve(Math::ClampMode clampMode)
	{
		this->clampMode = clampMode;
		loop = false;
		length = 0;	
	}
	AnimationCurve::AnimationCurve(nlohmann::json json)
	{
		length = 0.f;
		loop = false;
		clampMode = json["clampMode"];
		for (nlohmann::json& node : json["nodes"])
		{
			addNode(node);
		}

		updateLength();
		updateRange();
	}

	int AnimationCurve::getIndex(sf::Vector2f positon)
	{
		for (int x = 0; x < graph.size(); x++)
		{
			if (graph[x]->point.x > positon.x)
				return x;
		}
	}
	void AnimationCurve::updateRange()
	{
		xRange.x = graph[0]->point.x;
		xRange.y = graph[graph.size() - 1]->point.x;
	}
	float AnimationCurve::getRange()
	{
		return xRange.y - xRange.x;
	}
	void AnimationCurve::addNode(nlohmann::json json)
	{
		int index = getIndex(sf::Vector2f(json["point"].get<std::array<float, 2>>()[0], json["point"].get<std::array<float, 2>>()[1]));
		if (graph.size() < 1)
		{
			if (json.contains("handleL"))
				graph.push_back(std::make_unique<UnevenBezierNode>(json));
			else if (json.contains("handle"))
				graph.push_back(std::make_unique<BezierNode>(json));
			else
				graph.push_back(std::make_unique<GraphNode>(json));
			length = 0;
			return;
		}

		//Clamp to range of vector
		index = std::max(std::min(index, (int)graph.size()), 0);

		if (graph.size() < 2)
		{
			if (json.contains("handleL"))
				graph.insert(graph.begin() + index, std::make_unique<UnevenBezierNode>(json));
			else if (json.contains("handle"))
				graph.insert(graph.begin() + index, std::make_unique<BezierNode>(json));
			else
				graph.insert(graph.begin() + index, std::make_unique<GraphNode>(json));

			nodeLengths.push_back(getLength(*graph[0], *graph[1]));
			length = nodeLengths[0];
			return;
		}

		int prevIndex = index - 1;
		int nextIndex = index + 1;

		if (json.contains("handleL"))
			graph.insert(graph.begin() + index, std::make_unique<UnevenBezierNode>(json));
		else if (json.contains("handle"))
			graph.insert(graph.begin() + index, std::make_unique<BezierNode>(json));
		else
			graph.insert(graph.begin() + index, std::make_unique<GraphNode>(json));

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
		updateRange();
	}
	void AnimationCurve::addNode(sf::Vector2f position)
	{
		int index = getIndex(position);
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
		updateRange();
	}
	void AnimationCurve::addNode(sf::Vector2f position, sf::Vector2f handle)
	{
		int index = getIndex(position);
		//Make sure the handle is not facing backwards
		handle.x = std::max(handle.x, 0.f);

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
		updateRange();
	}
	void AnimationCurve::addNode(sf::Vector2f position, sf::Vector2f handleL, sf::Vector2f handleR)
	{
		int index = getIndex(position);
		//Make sure the handle is not facing backwards
		handleR.x = std::max(handleR.x, 0.f);

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
		updateRange();
	}
	void AnimationCurve::moveNode(sf::Vector2f newPosition, int index)
	{
		graph[index]->setPosition(newPosition);
		if (index > 0)
			nodeLengths[index - 1] = getLength(*graph[index - 1], *graph[index]);
		if (index < graph.size() - 1)
			nodeLengths[index] = getLength(*graph[index], *graph[index + 1]);
		updateRange();
	}
	void AnimationCurve::moveHandle(sf::Vector2f newPosition, int index, bool side)
	{
		graph[index]->setHandle(newPosition, side);
		if (index > 0)
			nodeLengths[index - 1] = getLength(*graph[index - 1], *graph[index]);
		if (index < graph.size() - 1)
			nodeLengths[index] = getLength(*graph[index], *graph[index + 1]);
	}
	float AnimationCurve::evaluate(float t, float step)
	{
		switch (clampMode)
		{
		case fl::Math::ClampMode::Clamp:
			if (t < xRange.x) return graph[0]->point.y;
			if (t > xRange.y) return graph[graph.size() - 1]->point.y;
			break;
		default:
			t = Math::clamp(xRange.x, xRange.y, t, clampMode);
			break;
		}

		//Get previous node to t
		int index = 0;
		for (int x = 1; x < graph.size(); x++)
		{
			if (graph[x]->point.x > t)
			{
				index = x - 1;
			}
		}

		//Get two points it should be between
		sf::Vector2f oldPos = graph[index]->point;
		sf::Vector2f pos = oldPos;
		for (float localT = 0.f; localT < 1.f; localT += step)
		{
			pos = fl::evaluate(*graph[index], *graph[index + 1], localT);
			if (pos.x > t) break;
			oldPos = pos;
		}
		
		//Approximate where between the 2 points
		return fl::lerp(oldPos, pos, fl::inverseLerp(oldPos.x, pos.x, t)).y;
	}
	nlohmann::json AnimationCurve::serialize()
	{
		nlohmann::json json;
		json["clampMode"] = clampMode;
		for (size_t x = 0; x < graph.size(); x++)
		{
			json["nodes"].push_back(graph[x]->serialize());
		}
		return json;
	}
}
