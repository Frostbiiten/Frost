#include <SplineTerrain.h>
#include <ApplicationManager.h>
#include <Physics.h>
#include <earcut.hpp>

namespace fl
{
	SplineTerrain::SplineTerrain(float simplifyThreshold)
	{
		uuid = uuids::uuid_random_generator{}();
		name = uuids::to_string(uuid);
		layer = Layer::Land;
		active = true;
		transform = sf::Transformable();
		spline.loop = true;
		this->simplifyThreshold = simplifyThreshold;
	}

	SplineTerrain::SplineTerrain(nlohmann::json json)
	{
		name = json["name"];
		uuid = uuids::uuid::from_string(json["uuid"].get<std::string>());
		layer = json["layer"];
		active = json["active"];
		transform = deserializeTransform(json["transform"]);
		spline = Graph(json["spline"]);
		simplifyThreshold = json["threshold"];
		generateOutline();
		transformOutline();
		simplifyOutline(simplifyThreshold);
		generateShape();
		generateCollider();
		setColor();
	}

	void SplineTerrain::generateOutline(float step)
	{
		for (std::size_t x = 0; x < spline.getNodeCount(); x++)
		{
			//Create first point
			outline.push_back(spline.getNodePos(x));
			int nextNode = x + 1;
			if (nextNode >= spline.getNodeCount()) nextNode = 0;

			//If the current and next node are not linear
			if (spline.getNodeType(x) != 0 && spline.getNodeType(nextNode) != 0)
			{
				//Don't start at zero because that node will already there
				float t = step;

				if (x < spline.getNodeCount() - 1)
				{
					//Distance from current to next node
					float distance = spline.getNodeLength(x);
					while (t < distance)
					{
						outline.push_back(evaluate(spline, x, x + 1, t / distance));
						t += step;
					}
				}
				else if (spline.loop)
				{
					//Don't start at zero because that node will already there
					float t = step;

					//Distance from current to next node
					float distance = getLength(spline.getNode(x), spline.getNode(0));

					while (t < distance)
					{
						outline.push_back(evaluate(spline, x, 0, t));
						t += step;
					}
				}
			}
		}

		/*
		OLD
		while (t < spline.length)
		{
			outline.push_back(spline.evaluateDistance(t));
			t += step;
		}
		*/
	}

	void SplineTerrain::transformOutline()
	{
		for (auto& point : outline)
		{
			point = transform.getTransform().transformPoint(point);
		}
	}

	void SplineTerrain::simplifyOutline(float threshold)
	{
		int indexA = 0;
		int indexB = 2;

		for (int i = 0; i < outline.size() - 2; i++)
		{
			sf::Vector2f closestPoint = Math::closestPointOnSegment(outline[i], outline[i + 2], outline[i + 1]);
			float sqrDist = Math::sqrMagnitude(outline[i + 1] - closestPoint);
			if (sqrDist < threshold * threshold)
			{
				outline.erase(outline.begin() + i + 1);
				i--;
			}
		}
	}

	void SplineTerrain::generateShape()
	{
		//Define vector with size 1 for main polygon
		std::vector<std::vector<std::array<float, 2>>> polygon(1);
		for (auto& point : outline)
			polygon[0].push_back({ point.x, point.y });

		//Prepare indicies
		std::vector<uint16_t> indices = mapbox::earcut<uint16_t>(polygon);

		//Reserve a third of the size of the indicies because a triangle has 3 verts
		shape.reserve(indices.size() / 3);

		//Iterate and add points
		for (std::size_t i = 0; i < indices.size(); i += 3)
		{
			shape.push_back(sf::ConvexShape(3));
			shape[shape.size() - 1].setPoint(0, sf::Vector2f(polygon[0][indices[i]][0], polygon[0][indices[i]][1]));
			shape[shape.size() - 1].setPoint(1, sf::Vector2f(polygon[0][indices[i + 1]][0], polygon[0][indices[i + 1]][1]));
			shape[shape.size() - 1].setPoint(2, sf::Vector2f(polygon[0][indices[i + 2]][0], polygon[0][indices[i + 2]][1]));
		}
	}

	void SplineTerrain::generateCollider()
	{
		components.push_back(std::make_unique<Physics::rigidBody>(this, b2Vec2(10, 10), b2BodyType::b2_staticBody));
		Physics::rigidBody* rb = dynamic_cast<Physics::rigidBody*>(components[0].get());
		//Removes default fixture
		rb->destroyFixture(0);

		b2ChainShape chain;

		std::unique_ptr<b2Vec2[]> verts(new b2Vec2[8192]);

		for (int i = outline.size() - 1; i >= 0; i--)
		{
			b2Vec2 b2Point = Physics::pixelToBox2dUnits(outline[i]);
			verts[i].Set(b2Point.x, b2Point.y);
		}

		chain.CreateLoop(verts.get(), outline.size());

		rb->getBody()->SetTransform(b2Vec2(), 0.f);
		rb->getBody()->CreateFixture(&chain, 100.f);
	}

	void SplineTerrain::setColor(sf::Color color)
	{
		for (auto& tri : shape)
		{
			tri.setFillColor(color);
		}
	}

	void SplineTerrain::update()
	{
		for (std::size_t i = 0; i < outline.size(); i++)
		{
			if (i == outline.size() - 1) Debug::drawLine(outline[i], outline[0]);
			else Debug::drawLine(outline[i], outline[i + 1]);
		}
	}

	nlohmann::json SplineTerrain::serialize()
	{
		nlohmann::json json = serializeBasic();
		json["transform"] = serializeTransform();
		json["spline"] = spline.serialize();
		json["type"] = "terrain";
		json["threshold"] = simplifyThreshold;

		for (auto& child : children)
		{
			json["children"].push_back(child->serialize());
		}

		return json;
	}
}
