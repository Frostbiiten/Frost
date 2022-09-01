#pragma once
#include <filesystem>
#include <imgui.h>

namespace fl
{
	namespace Editor
	{

		#ifndef IMGUI_CDECL
		#ifdef _MSC_VER
		#define IMGUI_CDECL __cdecl
		#else
		#define IMGUI_CDECL
		#endif
		#endif

		enum MyItemColumnID
		{
			MyItemColumnID_Name,
			MyItemColumnID_Size,
		};

		struct DirEntry
		{
			std::filesystem::directory_entry entry;
			int id;
			DirEntry(std::filesystem::directory_entry entry, int id) : entry(entry), id(id) {}
		};

		class AssetBrowser
		{
			std::filesystem::path currentPath;
			std::vector<std::filesystem::path> filter;

			std::vector<std::pair<const char*, std::filesystem::path>> pathButtons =
			{
				std::make_pair("Common", "common"),
				std::make_pair("Scenes", "scenes")
			};

			std::vector<std::filesystem::directory_entry> directoryEntries;
			std::vector<DirEntry> directoryEntriesSorted;
			std::string currentDirectoryBuffer;

			std::vector<std::filesystem::path> undoBuffer;
			std::vector<std::filesystem::path> redoBuffer;

			bool listDisplay;
			bool directoryButtonMode; // If directory field is in button mode

			const ImGuiTableSortSpecs* currentSortSpecs;

		public:

			AssetBrowser(std::filesystem::path path, bool listDisplay = true) : currentPath(path), listDisplay(listDisplay), directoryButtonMode(true) { UpdateEntries(currentPath); undoBuffer.reserve(16); redoBuffer.reserve(16); };

			void Shortcuts();
			void UpButton();
			void BackButton();
			void ForwardButton();
			void DirectoryField();

			void UpdateEntries(std::filesystem::path newPath, bool undoredo = false);
			void SetFilter(std::vector<std::filesystem::path> filter);
			void Draw();
		};
	}
}
