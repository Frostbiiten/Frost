#pragma once
#include <filesystem>
#include <imgui.h>
#include <functional>
#include <Debug.h>
#include <any>

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
			DirEntryID_Type,
		};

		// Perhaps nest in AssetBrowser class?
		// TODO: implement caching (like storing type, file size) to reduce calls
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

		const std::vector<std::pair<std::string, std::vector<std::filesystem::path>>> entryTypes =
		{
			{"Folder", {""}},
			{"Text", {".txt", ".doc", ".cpp", ".h"}},
			{"Image", {".png", ".jpg", ".jpeg", ".bmp"}},
			{"C++", {".cpp", ".cc", ".h", ".hpp", ".inl"}},
		};

		enum class BrowseState
		{
			Inactive = 0,
			Browsing = 1,
			Selected = 2
		};

		struct AssetBrowserCallback
		{
			std::function<void(std::filesystem::directory_entry, std::any)> callback;
			std::any data;

			AssetBrowserCallback(std::function<void(std::filesystem::directory_entry, std::any)> callback = [](std::filesystem::directory_entry entry, std::any data) { Debug::log()->info("PATH: {} ({})", entry.path().string(), std::any_cast<std::string>(data)); }, std::any data = "Callback is not set!") : callback(callback), data(data) {};
			void Invoke(std::filesystem::directory_entry entry) { callback(entry, data); };
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

			void Shortcuts();
			void UpButton();
			void BackButton();
			void ForwardButton();
			void DirectoryField();
			void EntryList();

			BrowseState currentState = BrowseState::Inactive;
			AssetBrowserCallback callback;

		public:

			AssetBrowser(std::filesystem::path path, bool listDisplay = true) :
				currentPath(path), listDisplay(listDisplay), directoryButtonMode(true), fileSelectMode(true), currentFilter(0)
				{ UpdateEntries(currentPath); undoBuffer.reserve(16); redoBuffer.reserve(16); };

			void UpdateEntries(std::filesystem::path newPath, bool undoredo = false);

			std::filesystem::directory_entry GetSelection();
			BrowseState GetState();

			void Launch(AssetBrowserCallback callback, std::filesystem::path path = "common");

			void Draw();
		};
	}
}
