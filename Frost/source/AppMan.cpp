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
		int fps;
		sf::Uint64 deltaTime, fpsUpdateTimer = 0, fpsFrameCount = 0, fpsSampleCount = 1000000, fpsBufferInterval = 40000, fpsBufferTimer = 0;
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
			// Soft Cherry style by Patitotective from ImThemes (modified)
			ImGuiStyle& style = ImGui::GetStyle();

			style.Alpha = 0.8;
			style.DisabledAlpha = 0.4000000059604645;
			style.WindowPadding = ImVec2(10.0, 10.0);
			style.WindowRounding = 4.0;
			style.WindowBorderSize = 0.0;
			style.WindowMinSize = ImVec2(50.0, 50.0);
			style.WindowTitleAlign = ImVec2(0.5, 0.5);
			style.WindowMenuButtonPosition = ImGuiDir_Left;
			style.ChildRounding = 0.0;
			style.ChildBorderSize = 1.0;
			style.PopupRounding = 1.0;
			style.PopupBorderSize = 1.0;
			style.FramePadding = ImVec2(5.0, 3.0);
			style.FrameRounding = 3.0;
			style.FrameBorderSize = 0.0;
			style.ItemSpacing = ImVec2(6.0, 6.0);
			style.ItemInnerSpacing = ImVec2(3.0, 2.0);
			style.CellPadding = ImVec2(3.0, 3.0);
			style.IndentSpacing = 6.0;
			style.ColumnsMinSpacing = 6.0;
			style.ScrollbarSize = 13.0;
			style.ScrollbarRounding = 16.0;
			style.GrabMinSize = 20.0;
			style.GrabRounding = 4.0;
			style.TabRounding = 4.0;
			style.TabBorderSize = 1.0;
			style.TabMinWidthForCloseButton = 0.0;
			style.ColorButtonPosition = ImGuiDir_Right;
			style.ButtonTextAlign = ImVec2(0.5, 0.5);
			style.SelectableTextAlign = ImVec2(0.0, 0.0);

			style.Colors[ImGuiCol_Text] = ImVec4(0.8588235378265381, 0.929411768913269, 0.886274516582489, 1.0);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5215686559677124, 0.5490196347236633, 0.5333333611488342, 1.0);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1294117718935013, 0.1372549086809158, 0.168627455830574, 1.0);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.1490196138620377, 0.1568627506494522, 0.1882352977991104, 1.0);
			style.Colors[ImGuiCol_PopupBg] = ImVec4(0.2000000029802322, 0.2196078449487686, 0.2666666805744171, 1.0);
			style.Colors[ImGuiCol_Border] = ImVec4(0.1372549086809158, 0.1137254908680916, 0.1333333402872086, 1.0);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0, 0.0, 0.0, 1.0);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.168627455830574, 0.1843137294054031, 0.2313725501298904, 1.0);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4549019634723663, 0.196078434586525, 0.2980392277240753, 1.0);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.4549019634723663, 0.196078434586525, 0.2980392277240753, 1.0);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.2313725501298904, 0.2000000029802322, 0.2705882489681244, 1.0);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.501960813999176, 0.07450980693101883, 0.2549019753932953, 1.0);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2000000029802322, 0.2196078449487686, 0.2666666805744171, 1.0);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2000000029802322, 0.2196078449487686, 0.2666666805744171, 1.0);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.239215686917305, 0.239215686917305, 0.2196078449487686, 1.0);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3882353007793427, 0.3882353007793427, 0.3725490272045135, 1.0);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.6941176652908325, 0.6941176652908325, 0.686274528503418, 1.0);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.6941176652908325, 0.6941176652908325, 0.686274528503418, 1.0);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.658823549747467, 0.1372549086809158, 0.1764705926179886, 1.0);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.6509804129600525, 0.1490196138620377, 0.3450980484485626, 1.0);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.7098039388656616, 0.2196078449487686, 0.2666666805744171, 1.0);
			style.Colors[ImGuiCol_Button] = ImVec4(0.6509804129600525, 0.1490196138620377, 0.3450980484485626, 1.0);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.4549019634723663, 0.196078434586525, 0.2980392277240753, 1.0);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4549019634723663, 0.196078434586525, 0.2980392277240753, 1.0);
			style.Colors[ImGuiCol_Header] = ImVec4(0.4549019634723663, 0.196078434586525, 0.2980392277240753, 1.0);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.6509804129600525, 0.1490196138620377, 0.3450980484485626, 1.0);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.501960813999176, 0.07450980693101883, 0.2549019753932953, 1.0);
			style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154, 0.4274509847164154, 0.4980392158031464, 1.0);
			style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.09803921729326248, 0.4000000059604645, 0.7490196228027344, 1.0);
			style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.09803921729326248, 0.4000000059604645, 0.7490196228027344, 1.0);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.6509804129600525, 0.1490196138620377, 0.3450980484485626, 1.0);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4549019634723663, 0.196078434586525, 0.2980392277240753, 1.0);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4549019634723663, 0.196078434586525, 0.2980392277240753, 1.0);
			style.Colors[ImGuiCol_Tab] = ImVec4(0.1764705926179886, 0.3490196168422699, 0.5764706134796143, 1.0);
			style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2588235437870026, 0.5882353186607361, 0.9764705896377563, 1.0);
			style.Colors[ImGuiCol_TabActive] = ImVec4(0.196078434586525, 0.407843142747879, 0.6784313917160034, 1.0);
			style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428, 0.1019607856869698, 0.1450980454683304, 1.0);
			style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086, 0.2588235437870026, 0.4235294163227081, 1.0);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(0.8588235378265381, 0.929411768913269, 0.886274516582489, 1.0);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.4549019634723663, 0.196078434586525, 0.2980392277240753, 1.0);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.3098039329051971, 0.7764706015586853, 0.196078434586525, 1.0);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.4549019634723663, 0.196078434586525, 0.2980392277240753, 1.0);
			style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104, 0.1882352977991104, 0.2000000029802322, 1.0);
			style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971, 0.3098039329051971, 0.3490196168422699, 1.0);
			style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832, 0.2274509817361832, 0.2470588237047195, 1.0);
			style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0, 0.0, 0.0, 1.0);
			style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0, 1.0, 1.0, 1.0);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.3843137323856354, 0.6274510025978088, 0.9176470637321472, 1.0);
			style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0, 1.0, 0.0, 1.0);
			style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026, 0.5882353186607361, 0.9764705896377563, 1.0);
			style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0, 1.0, 1.0, 1.0);
			style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929, 0.800000011920929, 0.800000011920929, 1.0);
			style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929, 0.800000011920929, 0.800000011920929, 0.300000011920929);
		}

		void ImGuiDebug()
		{
			ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
			ImGui::SetWindowPos(ImVec2(windowPtr->getSize().x - 160, 15));
			ImGui::Text(fmt::format("FPS: {}", fps).c_str());

			ImGui::PlotLines("", fpsBuffer.data(), fpsBuffer.size(), 0, 0, 60, 3.4028235E38F, ImVec2(130, 100));

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
			ImGuiIO.Fonts->AddFontFromFileTTF("common/fonts/Inter/Inter-Medium.ttf", 20.f);
			ImGui::SFML::UpdateFontTexture();
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
