#include <cstdlib>
#include <AppMan.h>
#include <Transform.h>
#include <SceneMan.h>
#include <SerializedTypes.h>
#include <Instrumentor.h>

#include <imgui.h>
#include <imgui-SFML.h>

namespace fl
{
	namespace AppMan
	{
		// Respective window and pixel rendering targets
		sf::RenderWindow* windowPtr;
		sf::RenderTexture* buffer;

		// Clock to measure app time, delta time and imgui delta time
		sf::Clock appClock, deltaTimer, ImGuiClock;

		// Fps data
		float fps, minFps = std::numeric_limits<float>::max(), maxFps = 0, avgFps;
		sf::Uint64 deltaTime, fpsUpdateTimer = 0, fpsFrameCount = 0, fpsSampleCount = 1000000, fpsBufferInterval = 40000, fpsBufferTimer = 0, lastAvgUpdate;
		std::array<float, 50> fpsBuffer;

		//Process window events
		void PollWindowEvents()
		{
			sf::Event event;
			while (windowPtr->pollEvent(event))
			{
				ImGui::SFML::ProcessEvent(event);
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
			// Display buffer onto sprite
			buffer->display();

			// Draw buffersprite directly onto window (ignore transformations)
			auto windowView = windowPtr->getView();
			windowPtr->setView(sf::View(sf::Vector2f(), pixelSize));
			windowPtr->draw(bufferSprite);
			windowPtr->setView(windowView);

			// Render ImGui
			ImGui::SFML::Render(*windowPtr);

			// Display the window
			windowPtr->display();
		}

		void Terminate()
		{
			// End session on terminate
			Instrumentor::Instance().endSession();
			ImGui::SFML::Shutdown();
		}

		void SkinImGui()
		{
			// Deep Dark style by janekb04 from ImThemes (modified)

			ImGuiStyle& style = ImGui::GetStyle();

			style.Alpha = 0.8;
			style.DisabledAlpha = 0.6000000238418579;
			style.WindowPadding = ImVec2(8.0, 8.0);
			style.WindowRounding = 3.0;
			style.WindowBorderSize = 1.0;
			style.WindowMinSize = ImVec2(32.0, 32.0);
			style.WindowTitleAlign = ImVec2(0.0, 0.5);
			style.WindowMenuButtonPosition = ImGuiDir_Left;
			style.ChildRounding = 4.0;
			style.ChildBorderSize = 1.0;
			style.PopupRounding = 4.0;
			style.PopupBorderSize = 1.0;
			style.FramePadding = ImVec2(5.0, 2.0);
			style.FrameRounding = 3.0;
			style.FrameBorderSize = 1.0;
			style.ItemSpacing = ImVec2(6.0, 6.0);
			style.ItemInnerSpacing = ImVec2(6.0, 6.0);
			style.CellPadding = ImVec2(6.0, 6.0);
			style.IndentSpacing = 25.0;
			style.ColumnsMinSpacing = 6.0;
			style.ScrollbarSize = 15.0;
			style.ScrollbarRounding = 9.0;
			style.GrabMinSize = 10.0;
			style.GrabRounding = 3.0;
			style.TabRounding = 4.0;
			style.TabBorderSize = 1.0;
			style.TabMinWidthForCloseButton = 0.0;
			style.ColorButtonPosition = ImGuiDir_Right;
			style.ButtonTextAlign = ImVec2(0.5, 0.5);
			style.SelectableTextAlign = ImVec2(0.0, 0.0);

			style.Colors[ImGuiCol_Text] = ImVec4(1.0, 1.0, 1.0, 1.0);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464, 0.4980392158031464, 0.4980392158031464, 1.0);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09803921729326248, 0.09803921729326248, 0.09803921729326248, 1.0);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0, 0.0, 0.0, 0.0);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1882352977991104, 0.1882352977991104, 0.1882352977991104, 0.9200000166893005);
			style.Colors[ImGuiCol_Border] = ImVec4(0.1882352977991104, 0.1882352977991104, 0.1882352977991104, 0.2899999916553497);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0, 0.0, 0.0, 0.239999994635582);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0470588244497776, 0.0470588244497776, 0.0470588244497776, 0.5400000214576721);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1882352977991104, 0.1882352977991104, 0.1882352977991104, 0.5400000214576721);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2000000029802322, 0.2196078449487686, 0.2274509817361832, 1.0);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.05882352963089943, 0.05882352963089943, 0.05882352963089943, 1.0);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0, 0.0, 0.0, 1.0);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158, 0.1372549086809158, 0.1372549086809158, 1.0);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0470588244497776, 0.0470588244497776, 0.0470588244497776, 0.5400000214576721);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3372549116611481, 0.3372549116611481, 0.3372549116611481, 0.5400000214576721);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4000000059604645, 0.4000000059604645, 0.4000000059604645, 0.5400000214576721);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5568627715110779, 0.5568627715110779, 0.5568627715110779, 0.5400000214576721);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.3294117748737335, 0.6666666865348816, 0.8588235378265381, 1.0);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3372549116611481, 0.3372549116611481, 0.3372549116611481, 0.5400000214576721);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5568627715110779, 0.5568627715110779, 0.5568627715110779, 0.5400000214576721);
			style.Colors[ImGuiCol_Button] = ImVec4(0.0470588244497776, 0.0470588244497776, 0.0470588244497776, 0.5400000214576721);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1882352977991104, 0.1882352977991104, 0.1882352977991104, 0.5400000214576721);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2000000029802322, 0.2196078449487686, 0.2274509817361832, 1.0);
			style.Colors[ImGuiCol_Header] = ImVec4(0.0, 0.0, 0.0, 0.5199999809265137);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0, 0.0, 0.0, 0.3600000143051147);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2000000029802322, 0.2196078449487686, 0.2274509817361832, 0.3300000131130219);
			style.Colors[ImGuiCol_Separator] = ImVec4(0.2784313857555389, 0.2784313857555389, 0.2784313857555389, 0.2899999916553497);
			style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4392156898975372, 0.4392156898975372, 0.4392156898975372, 0.2899999916553497);
			style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.4000000059604645, 0.4392156898975372, 0.4666666686534882, 1.0);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2784313857555389, 0.2784313857555389, 0.2784313857555389, 0.2899999916553497);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4392156898975372, 0.4392156898975372, 0.4392156898975372, 0.2899999916553497);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4000000059604645, 0.4392156898975372, 0.4666666686534882, 1.0);
			style.Colors[ImGuiCol_Tab] = ImVec4(0.0, 0.0, 0.0, 0.5199999809265137);
			style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1372549086809158, 0.1372549086809158, 0.1372549086809158, 1.0);
			style.Colors[ImGuiCol_TabActive] = ImVec4(0.2000000029802322, 0.2000000029802322, 0.2000000029802322, 0.3600000143051147);
			style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0, 0.0, 0.0, 0.5199999809265137);
			style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1372549086809158, 0.1372549086809158, 0.1372549086809158, 1.0);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(1.0, 0.0, 0.0, 1.0);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0, 0.0, 0.0, 1.0);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0, 0.0, 0.0, 1.0);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0, 0.0, 0.0, 1.0);
			style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0, 0.0, 0.0, 0.5199999809265137);
			style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0, 0.0, 0.0, 0.5199999809265137);
			style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2784313857555389, 0.2784313857555389, 0.2784313857555389, 0.2899999916553497);
			style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0, 0.0, 0.0, 0.0);
			style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0, 1.0, 1.0, 0.05999999865889549);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2000000029802322, 0.2196078449487686, 0.2274509817361832, 1.0);
			style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.3294117748737335, 0.6666666865348816, 0.8588235378265381, 1.0);
			style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.0, 0.0, 0.0, 1.0);
			style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0, 0.0, 0.0, 0.699999988079071);
			style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.0, 0.0, 0.0, 0.2000000029802322);
			style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0, 0.0, 0.0, 0.3499999940395355);
		}

		void ImGuiDebug()
		{
			ImGui::Begin("Debug", NULL, ImGuiWindowFlags_NoScrollbar);

			if (ImGui::BeginTable("FPS", 2, ImGuiTableFlags_Resizable))
			{
				ImGui::TableNextColumn();
				ImGui::Text(fmt::format("FPS: {}", (int)fps).c_str());
				ImGui::TextColored(ImVec4(0.93, 0.1, 0.3, 1), fmt::format("MIN: {}", (int)minFps).c_str());
				ImGui::TextColored(ImVec4(1, 0.85, 0.3, 1), fmt::format("AVG: {}", (int)avgFps).c_str());
				ImGui::TextColored(ImVec4(0.1, 0.93, 0.3, 1), fmt::format("MAX: {}", (int)maxFps).c_str());
				ImGui::TextDisabled(fmt::format("{} ms", (1000.f / fps)).c_str());

				ImGui::TableNextColumn();
				ImGui::TextDisabled("Frametime");
				ImGui::PlotLines("", fpsBuffer.data(), fpsBuffer.size(), 0, 0, 30, 3.4028235E38F, ImVec2(ImGui::GetContentRegionAvail().x, 100));
				ImGui::EndTable();
			}

			ImGui::End();
		}

		void UpdateFPS()
		{
			++fpsFrameCount;
			deltaTime = deltaTimer.restart().asMicroseconds();
			fpsUpdateTimer += deltaTime;
			if (fpsUpdateTimer > fpsSampleCount)
			{
				fps = (double)fpsFrameCount / (double)fpsUpdateTimer * fpsSampleCount;
				if (fps > maxFps) maxFps = fps;
				if (fps < minFps) minFps = fps;

				sf::Uint64 currentTime = appClock.getElapsedTime().asMicroseconds();
				float proportion = (double)lastAvgUpdate / (double)currentTime;
				avgFps = (avgFps * proportion) + (fps * (1 - proportion));
				lastAvgUpdate = currentTime;

				fpsFrameCount = 0;
				fpsUpdateTimer = 0;
			}

			fpsBufferTimer += deltaTime;
			if (fpsBufferTimer >= fpsBufferInterval)
			{
				std::rotate(fpsBuffer.begin(), fpsBuffer.begin() + 1, fpsBuffer.end());
				fpsBuffer[fpsBuffer.size() - 1] = 1000000.0 / deltaTime;
				fpsBufferTimer = 0;
			}
		}

		void UpdateBuffer()
		{
			// Update buffer view
			auto bufferView = windowPtr->getView();
			bufferView.setCenter(bufferView.getCenter() + sf::Vector2f(windowPtr->getSize().x / -2, windowPtr->getSize().y / -2));
			buffer->setView(bufferView);
		}

		// ENTRY POINT
		void Init()
		{
			std::atexit(Terminate);
			Debug::init();

			// Reset app clock
			appClock.restart();

			// Initiating window and set viewsize
			windowPtr = new sf::RenderWindow(sf::VideoMode(1060, 600), applicationName, sf::Style::Default);
			sf::View view(sf::Vector2f(), pixelSize);
			windowPtr->setView(view);

			// Init ImGui
			ImGui::SFML::Init(*windowPtr, false);
			ImGuiIO& ImGuiIO = ImGui::GetIO();
			ImGuiIO.Fonts->AddFontFromFileTTF("common/fonts/Inter/Inter-Medium.ttf", 18.f);
			ImGui::SFML::UpdateFontTexture();
			ImGuiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			SkinImGui();

			// Create view buffer and sprite
			sf::RenderTexture buf;
			buf.create((int)pixelSize.x, (int)pixelSize.y);
			buffer = &buf;
			sf::Sprite bufferSprite = sf::Sprite(buf.getTexture());
			bufferSprite.setOrigin(pixelSize / 2.f);

			// Register types for serialization
			SerializedTypes::RegisterTypes();

			// Ready loops
			SceneMan::Reset(*windowPtr, *buffer);

			// Reset fps buffer
			std::fill_n(fpsBuffer.begin(), fpsBuffer.size(), 0);

			// Main loop
			while (windowPtr->isOpen())
			{
				PollWindowEvents();
				ImGui::SFML::Update(*windowPtr, ImGuiClock.restart());
				ClearScreen();
				UpdateBuffer();

				SceneMan::Tick();

				UpdateFPS();
				ImGuiDebug();

				// Render BUFFER after the scene's code has been run
				Render(buf, bufferSprite);
			}

			ImGui::SFML::Shutdown();
			ResourceMan::purgeResources();
		}

		sf::RenderWindow* getWindow()
		{
			return windowPtr;
		}
		sf::RenderTexture* getBuffer()
		{
			return buffer;
		}
	}
}
