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

		enum DirEntryID
		{
			DirEntryID_Name,
			DirEntryID_Size,
		};

		// Perhaps nest in AssetBrowser class?
		struct DirEntry
		{
			std::filesystem::directory_entry entry;
			int id;
			DirEntry(std::filesystem::directory_entry entry, int id) : entry(entry), id(id) {}
		};

		const std::vector<std::pair<std::string, std::vector<std::filesystem::path>>> filterPresets =
		{
			{"All", {""}},
			{"Folder", {""}},
			{"Text", {".txt", ".doc", ".cpp", ".h"}},
			{"Images", {".png", ".jpg", ".jpeg", ".bmp"}},
		};

		class AssetBrowser
		{
			std::filesystem::path currentPath;
			int currentFilter;

			std::vector<std::pair<const char*, std::filesystem::path>> pathButtons =
			{
				std::make_pair("Common", "common"),
				std::make_pair("Scenes", "scenes")
			};

			std::vector<DirEntry> directoryEntriesSorted;
			std::string currentDirectoryBuffer;

			std::vector<std::filesystem::path> undoBuffer;
			std::vector<std::filesystem::path> redoBuffer;

			bool listDisplay, directoryButtonMode, fileSelectMode;

			const ImGuiTableSortSpecs* currentSortSpecs;
			std::filesystem::directory_entry selectedEntry;

		public:

			AssetBrowser(std::filesystem::path path, bool listDisplay = true) :
				currentPath(path), listDisplay(listDisplay), directoryButtonMode(true), fileSelectMode(true), currentFilter(0)
				{ UpdateEntries(currentPath); undoBuffer.reserve(16); redoBuffer.reserve(16); };

			void Shortcuts();
			void UpButton();
			void BackButton();
			void ForwardButton();
			void DirectoryField();
			void EntryList();

			void UpdateEntries(std::filesystem::path newPath, bool undoredo = false);
			void Draw();
		};
	}
}
