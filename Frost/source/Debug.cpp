#include <Debug.h>

/*
#include <ApplicationManager.h>
#include <Physics.h>
#include <box2d/box2d.h>
*/
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <time.h>

// fmt
#include <fmt/color.h>

// spdlog
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/logger.h>
#include <spdlog/async.h>
#include <spdlog/spdlog.h>

// Vector2 printing
#include <spdlog/fmt/fmt.h>

template<> struct fmt::formatter<sf::Vector2f>
{
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.end(); }

	template <typename FormatContext>
	auto format(const sf::Vector2f& input, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "({}, {})", sf::Vector2f.x, sf::Vector2f.y);
	}
};

namespace fl
{
	// If log has been initialized
	bool isInit = false;

	// Main log, outputs to cout & file
	std::shared_ptr<spdlog::logger> mainLog;

	void Debug::init()
	{
		if (isInit) return;

		struct tm newtime;
		time_t now = time(0);
		localtime_s(&newtime, &now);
		std::ostringstream dateStream;
		dateStream << "logs/" << std::put_time(&newtime, "%d-%m-%Y %H-%M-%S") << ".log";
		std::string logName = dateStream.str();

		checkFolder();

		try
		{
			spdlog::init_thread_pool(8192, 1);

			// Create file and cout sink
			auto coutSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logName, 5242880, 5, false);

			std::vector<spdlog::sink_ptr> sinks {coutSink, fileSink};

			mainLog = std::make_shared<spdlog::async_logger>("main", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
			spdlog::register_logger(mainLog);

			isInit = true;
		}
		catch (const spdlog::spdlog_ex& ex)
		{
			fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::crimson), "Failed to init log: {}\n", ex.what());
		}
	}

	void Debug::checkFolder()
	{
		if (!std::filesystem::exists("./Logs"))
		{
			std::filesystem::create_directory("./Logs");
			fmt::print(fmt::emphasis::bold | fmt::fg(fmt::color::gold), "Created log folder...\n");
		}
	}

	std::string Debug::getType(const type_info& type)
	{
		if (type == typeid(std::string)) return "string";
		return type.name();
	}

	spdlog::logger* Debug::log()
	{
		return mainLog.get();
	}

	/*
	void Debug::drawCircle(sf::Vector2f position, float radius, float thickness, sf::Color color)
	{
		sf::CircleShape circle{radius};
		circle.setPosition(position);
		circle.setFillColor(sf::Color::Transparent);
		circle.setOutlineColor(color);
		circle.setOutlineThickness(thickness);
		ApplicationManager::windowDrawElement(circle);
	}
	*/

	/*
	void Debug::drawRectangle(sf::Vector2f position, sf::Vector2f size, float rotation, float thickness, sf::Color color)
	{
		sf::RectangleShape rect{size};
		rect.setPosition(position);
		rect.setRotation(rotation);
		rect.setFillColor(sf::Color::Transparent);
		rect.setOutlineColor(color);
		rect.setOutlineThickness(thickness);
		ApplicationManager::windowDrawElement(rect);
	}
	*/

	/*
	void Debug::drawLine(sf::Vector2f begin, sf::Vector2f end, sf::Color color)
	{
		sf::Vertex line[] = {sf::Vertex(begin), sf::Vertex(end)};
		line[0].color = color;
		line[1].color = color;
		ApplicationManager::getWindow()->draw(line, 2, sf::Lines);
	}
	*/

	/*
	void Debug::drawLineThick(sf::Vector2f begin, sf::Vector2f end, float thickness, sf::Color color)
	{
		sf::Vertex vertices[4];
		sf::Vector2f direction = end - begin;
		sf::Vector2f unitDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
		sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

		sf::Vector2f offset = (thickness / 2.f) * unitPerpendicular;

		vertices[0].position = begin + offset;
		vertices[1].position = end + offset;
		vertices[2].position = begin - offset;
		vertices[3].position = end - offset;

		vertices[0].color = color;
		vertices[1].color = color;
		vertices[2].color = color;
		vertices[3].color = color;

		ApplicationManager::getWindow()->draw(vertices, 4, sf::Quads);
	}
	*/

	/*
	void Debug::drawRay(sf::Vector2f begin, sf::Vector2f direction, sf::Color color)
	{
		sf::Vertex line[] = {sf::Vertex(begin), sf::Vertex(direction)};
		line[0].color = color;
		line[1].color = color;
		ApplicationManager::getWindow()->draw(line, 2, sf::Lines);
	}
	*/

	/*
	void Debug::drawRayThick(sf::Vector2f begin, sf::Vector2f direction, float thickness, sf::Color color)
	{
		sf::Vertex vertices[4];
		sf::Vector2f unitDirection = direction / std::sqrt(direction.x * direction.x + direction.y * direction.y);
		sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

		sf::Vector2f offset = (thickness / 2.f) * unitPerpendicular;

		vertices[0].position = begin + offset;
		vertices[1].position = begin + direction + offset;
		vertices[2].position = begin + direction - offset;
		vertices[3].position = begin - offset;

		vertices[0].color = color;
		vertices[1].color = color;
		vertices[2].color = color;
		vertices[3].color = color;

		ApplicationManager::getWindow()->draw(vertices, 4, sf::Quads);
	}
	*/

	/*
	void Debug::DumpPhysicsWorld()
	{
		Debug::log("");
		Debug::log("Physics world debugger");
		Debug::log("-----------------------");

		Debug::log("\tPhysics world body list");
		Debug::log("\t-----------------------");
		auto element = Physics::physicsWorld.GetBodyList();
		for (int i = 0; i < Physics::physicsWorld.GetBodyCount(); ++i)
		{
			Physics::rigidBody* comp = static_cast<Physics::rigidBody*>(element->GetUserData().data);
			Debug::log("\t\t" + std::to_string(i) + ": " + comp->owner->name);
			element = element->GetNext();
			sf::Vector2f position = comp->owner->transform.getPosition();
			sf::Vector2f scale = comp->owner->transform.getScale();
			Debug::log("\t\t Position: " + std::to_string(position.x) + ", " + std::to_string(position.y));
			Debug::log("\t\t Rotation: " + std::to_string(comp->owner->transform.getRotation()));
			Debug::log("\t\t Scale: " + std::to_string(scale.x) + ", " + std::to_string(scale.y));
		}
	}
	*/
}
