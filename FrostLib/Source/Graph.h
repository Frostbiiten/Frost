#pragma once
#include <json.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>

namespace fl
{
	struct GraphNode
	{
		sf::Vector2f point;
		GraphNode();
		GraphNode(sf::Vector2f point);
		GraphNode(float x, float y);
		GraphNode(nlohmann::json json);

		virtual sf::Vector2f getHandle(bool side);
		virtual sf::Vector2f getLerpPos(bool side, float t);
		virtual void setPosition(sf::Vector2f pos);
		virtual void setHandle(sf::Vector2f pos, bool side = true);
		virtual nlohmann::json serialize();
		virtual int GetType();
	};

	//Default BezierNode is symmetric
	struct BezierNode : public GraphNode
	{
		//Handle is relative to the main point and considered as the right side
		sf::Vector2f handle;
		BezierNode(sf::Vector2f point, sf::Vector2f handle);
		BezierNode(nlohmann::json json);
		sf::Vector2f getHandle(bool side);
		sf::Vector2f getLerpPos(bool side, float t);
		void setPosition(sf::Vector2f pos);
		void setHandle(sf::Vector2f pos, bool side = true);
		nlohmann::json serialize();
		int GetType();
	};

	struct UnevenBezierNode : public GraphNode
	{
		//Handle is relative to the main point and considered as the right side
		sf::Vector2f handleL;
		sf::Vector2f handleR;
		UnevenBezierNode(sf::Vector2f point, sf::Vector2f handleL, sf::Vector2f handleR);
		UnevenBezierNode(nlohmann::json json);
		sf::Vector2f getHandle(bool side);
		sf::Vector2f getLerpPos(bool side, float t);
		void setPosition(sf::Vector2f pos);
		void setHandle(sf::Vector2f pos, bool side = true);
		nlohmann::json serialize();
		int GetType();
	};

	class Graph
	{
	protected:
		//Must use pointers for polymorphism
		std::vector<std::unique_ptr<GraphNode>> graph;

		//The lengths from each node the next
		std::vector<float> nodeLengths;

		//Lower step = more precision/slower
		float getLength(GraphNode& a, GraphNode& b, float step = 0.001f);
		void updateLength();
	public:
		//length of whole graph
		float length;

		//If the graph is closed
		bool loop;

		Graph(bool loop = false);
		Graph(nlohmann::json json);
		int getNodeCount();
		GraphNode& getNode(int pos);
		sf::Vector2f getNodePos(int pos);
		float getNodeLength(int index);
		int getNodeType(int index);
		void addNode(nlohmann::json json, int index);
		void addNode(sf::Vector2f position, int index);
		void addNode(sf::Vector2f position, sf::Vector2f handle, int index);
		void addNode(sf::Vector2f position, sf::Vector2f handleL, sf::Vector2f handleR, int index);
		void moveNode(sf::Vector2f newPosition, int index);
		void moveHandle(sf::Vector2f newPosition, int index, bool side);
		sf::Vector2f evaluateDistance(float distance);
		sf::Vector2f evaluateT(float t);

		virtual nlohmann::json serialize();
	};

	inline sf::Vector2f lerp(const sf::Vector2f& a, const sf::Vector2f& b, float t);
	inline float inverseLerp(const float& a, const float& b, const float& value);
	float getLength(GraphNode& a, GraphNode& b, float step = 0.001f);
	sf::Vector2f evaluate(GraphNode& a, GraphNode& b, float t);
	sf::Vector2f evaluate(Graph& graph, int a, int b, float t);
}
