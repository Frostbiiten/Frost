#include <AssetBrowser.h>
#include <imgui.h>
#include <Debug.h>
#include <imgui_stdlib.h>
#include <AssetMan.h>
#include <Instrumentor.h>

namespace fl
{
	namespace Editor
	{
		// For size display
		constexpr unsigned long KILOBYTE = 1024;
		constexpr unsigned long MEGABYTE = 1024 * 1024;
		constexpr unsigned long GIGABYTE = 1024 * 1024 * 1024;

		inline std::string GetType(const std::filesystem::directory_entry& entry)
		{
			if (entry.is_directory()) return "Folder";
			if (entry.path().extension().string() == "") return "Unknown";
			for (auto& type : entryTypes)
			{
				for (auto& filter : type.second)
				{
					if (entry.path().extension() == filter)
					{
						return type.first;
					}
				}
			}
			return "Unknown";
		}

		inline std::string formatSize(unsigned long bytes)
		{
			if (bytes < KILOBYTE) return fmt::format("{} B", bytes);
			else if (bytes < MEGABYTE) return fmt::format("{:.{}f} kB", (float)bytes / KILOBYTE, 2);
			else if (bytes < GIGABYTE) return fmt::format("{:.{}f} MB", (float)bytes / MEGABYTE, 2);
			else return fmt::format("{:.{}f} GB", (float)bytes / GIGABYTE, 2);
		}
		void AssetBrowser::UpdateEntries(std::filesystem::path newPath, bool undoredo)
		{
			std::filesystem::path oldPath = currentPath;
			currentPath = newPath;
			directoryEntriesSorted.clear();
			selectedEntry = std::filesystem::directory_entry{};

			// Needs to be replaced with proper checking
			if (currentPath == "") currentPath = AssetMan::getBaseDirectory();

			if (currentFilter)
			{
				for (const auto& dirEntry : std::filesystem::directory_iterator(currentPath))
				{
					if (dirEntry.is_regular_file())
					{
						auto entryExtension = dirEntry.path().extension();

						for (std::size_t x = 0; x < filterPresets[currentFilter].second.size(); ++x)
						{
							if (entryExtension.string() == filterPresets[currentFilter].second[x])
							{
								directoryEntriesSorted.push_back(DirEntry(dirEntry, directoryEntriesSorted.size()));
								break;
							}
						}
					}
					else if (dirEntry.is_directory())
					{
						directoryEntriesSorted.push_back(DirEntry(dirEntry, directoryEntriesSorted.size()));
					}
				}
			}
			else
			{
				for (const auto& dirEntry : std::filesystem::directory_iterator(currentPath))
				{
					directoryEntriesSorted.push_back(DirEntry(dirEntry, directoryEntriesSorted.size()));
				}
			}

			currentDirectoryBuffer = currentPath.string();

			if (!undoredo)
			{
				if (oldPath != currentPath)
				{
					if (undoBuffer.size() < 16)
					{
						undoBuffer.push_back(oldPath);
					}
					else
					{
						std::rotate(undoBuffer.begin(), undoBuffer.begin() + 1, undoBuffer.end());
						undoBuffer[undoBuffer.size() - 1] = oldPath;
					}
				}

				// Clear redo buffer
				redoBuffer.clear();
			}
		}
		BrowseState AssetBrowser::GetState()
		{
			return currentState;
		}
		void AssetBrowser::Shortcuts()
		{
			for (std::size_t x = 0; x < pathButtons.size(); ++x)
			{
				ImGui::SameLine();
				if (ImGui::SmallButton(pathButtons[x].first))
				{
					UpdateEntries(pathButtons[x].second);
				}
			}
		}
		void AssetBrowser::UpButton()
		{
			if (ImGui::Button("Up"))
			{
				UpdateEntries(currentPath.parent_path());
			}
		}
		void AssetBrowser::BackButton()
		{
			bool backDisabled = !undoBuffer.size();
			if (backDisabled) ImGui::BeginDisabled();
			if (ImGui::Button("Back"))
			{
				// Move from undo buffer to redo buffer
				auto path = undoBuffer[undoBuffer.size() - 1];
				undoBuffer.pop_back();
				std::filesystem::path oldPath = currentPath;
				UpdateEntries(path, true);

				if (redoBuffer.size() < 16)
				{
					redoBuffer.push_back(oldPath);
				}
				else
				{
					std::rotate(redoBuffer.begin(), redoBuffer.begin() + 1, redoBuffer.end());
					redoBuffer[redoBuffer.size() - 1] = oldPath;
				}
			}
			if (backDisabled) ImGui::EndDisabled();
		}
		void AssetBrowser::ForwardButton()
		{
			bool forwardDisabled = !redoBuffer.size();
			if (forwardDisabled) ImGui::BeginDisabled();
			if (ImGui::Button("Forward"))
			{
				// Move from redo buffer to redo buffer
				auto path = redoBuffer[redoBuffer.size() - 1];
				redoBuffer.pop_back();
				std::filesystem::path oldPath = currentPath;
				UpdateEntries(path, true);

				if (undoBuffer.size() < 16)
				{
					undoBuffer.push_back(oldPath);
				}
				else
				{
					std::rotate(undoBuffer.begin(), undoBuffer.begin() + 1, undoBuffer.end());
					undoBuffer[undoBuffer.size() - 1] = oldPath;
				}
			}
			if (forwardDisabled) ImGui::EndDisabled();
		}
		void AssetBrowser::DirectoryField()
		{
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);

			std::stringstream ss(currentDirectoryBuffer);
			std::vector<std::string> folders;
			std::string buf;

			while (getline(ss, buf, '\\'))
			{
				folders.push_back(std::move(buf));
				buf.clear();
			}

			if (directoryButtonMode)
			{
				for (std::size_t x = 0; x < folders.size(); ++x)
				{
					if (x != 0)
					{
						ImGui::SameLine();
						ImGui::TextUnformatted("/");
						ImGui::SameLine();
					}

					// Go up directories if buttons clicked (cannot be current directory)
					ImGui::SmallButton(folders[x].c_str());

					if (ImGui::IsItemHovered())
					{
						if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && x != folders.size() - 1)
						{
							std::filesystem::path newPath = currentPath;
							for (std::size_t y = 0; y < folders.size() - x - 1; ++y)
							{
								newPath = newPath.parent_path();
							}
							UpdateEntries(newPath);
						}
						else if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
						{
							directoryButtonMode = false;
						}
					}
				}
			}
			else
			{
				if (ImGui::InputText("##", &currentDirectoryBuffer, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					std::filesystem::path path(currentDirectoryBuffer);
					if (std::filesystem::exists(path))
					{
						UpdateEntries(path);
					}
					else
					{
						currentDirectoryBuffer = currentPath.string();
					}
					directoryButtonMode = true;
				}
			}
		}
		void AssetBrowser::EntryList()
		{
			if (listDisplay)
			{
				ImGuiTableFlags flags =
				ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
				| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
				| ImGuiTableFlags_ScrollY;

				if (ImGui::BeginTable("DirectoryList", 3, flags, ImVec2(0, ImGui::GetContentRegionAvail().y - (30 * fileSelectMode))))
				{
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort, 0.0f, DirEntryID_Name);
					ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_DefaultSort, 0.0f, DirEntryID_Size);
					ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_DefaultSort, 0.0f, DirEntryID_Type);
					ImGui::TableSetupScrollFreeze(0, 1);
					ImGui::TableHeadersRow();

					for (std::size_t x = 0; x < directoryEntriesSorted.size(); x++)
					{
						ImGui::TableNextRow();

						// NAME
						ImGui::TableSetColumnIndex(0);
						ImGui::Text(directoryEntriesSorted[x].entry.path().filename().string().c_str());

						// SIZE
						ImGui::TableSetColumnIndex(1);
						if (directoryEntriesSorted[x].entry.is_regular_file()) ImGui::Text(formatSize(directoryEntriesSorted[x].entry.file_size()).c_str());
						else ImGui::TextDisabled("-");

						// Type
						ImGui::TableSetColumnIndex(2);
						ImGui::Text(GetType(directoryEntriesSorted[x].entry).c_str());

						// Allow row to be selected
						ImGui::SameLine();
						ImGui::Selectable("", selectedEntry == directoryEntriesSorted[x].entry, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick);

						if (ImGui::IsItemHovered())
						{
							ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, 1);

							if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && fileSelectMode)
							{
								selectedEntry = directoryEntriesSorted[x].entry;
							}

							if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							{
								if (directoryEntriesSorted[x].entry.is_directory())
								{
									UpdateEntries(directoryEntriesSorted[x].entry);
								}
								else if (directoryEntriesSorted[x].entry.is_regular_file())
								{
									currentState = BrowseState::Selected;
									this->callback.Invoke(directoryEntriesSorted[x].entry);
									callback = AssetBrowserCallback();
								}
							}
						}
					}

					// Sort table data
					if (ImGuiTableSortSpecs* sorts_specs = ImGui::TableGetSortSpecs())
					{
						if (sorts_specs->SpecsDirty)
						{
							currentSortSpecs = sorts_specs; // Store in variable accessible by the sort function.
							if (directoryEntriesSorted.size())
							{
								std::sort(directoryEntriesSorted.begin(), directoryEntriesSorted.end(),
								[&](const DirEntry& a, const DirEntry& b)
								{
									for (std::size_t n = 0; n < currentSortSpecs->SpecsCount; n++)
									{
										const ImGuiTableColumnSortSpecs* sort_spec = &currentSortSpecs->Specs[n];
										int delta = 0;
										switch (sort_spec->ColumnUserID)
										{
											case DirEntryID_Name:
											{
												delta = a.entry.path().string().compare(b.entry.path().string());
												break;
											}
											case DirEntryID_Size:
											{
												unsigned long aSize = a.entry.is_regular_file() ? a.entry.file_size() : 0;
												unsigned long bSize = b.entry.is_regular_file() ? b.entry.file_size() : 0;
												delta = aSize - bSize;
												break;
											}
											case DirEntryID_Type:
											{
												delta = GetType(a.entry).compare(GetType(b.entry));
												break;
											}
											default: IM_ASSERT(0); break;
										}

										if (delta != 0) return ((delta > 0) != (sort_spec->SortDirection == ImGuiSortDirection_Ascending));
										else
										{
											return ((a.id > b.id) != (sort_spec->SortDirection == ImGuiSortDirection_Ascending));
										}
									}
								});
							}

							currentSortSpecs = NULL;
							sorts_specs->SpecsDirty = false;
						}
					}

					ImGui::EndTable();
				}
			}
			else
			{
				// TODO: add icons, replace buttons with selectable + double click to open
				float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
				auto& style = ImGui::GetStyle();
				ImVec2 buttonSize = ImVec2(100, 100);
				for (std::size_t x = 0; x < directoryEntriesSorted.size(); x++)
				{
					ImGui::PushID(x);
					bool clicked = ImGui::Button(directoryEntriesSorted[x].entry.path().filename().string().c_str(), buttonSize);
					float last_button_x2 = ImGui::GetItemRectMax().x;
					float next_button_x2 = last_button_x2 + style.ItemSpacing.x + buttonSize.x; // Expected position if next button was on same line
					if (x + 1 < directoryEntriesSorted.size() && next_button_x2 < window_visible_x2) ImGui::SameLine();
					ImGui::PopID();

					if (clicked)
					{
						UpdateEntries(directoryEntriesSorted[x].entry);
					}
				}
			}
		}

		void AssetBrowser::Launch(AssetBrowserCallback callback, std::filesystem::path path)
		{
			this->currentPath = path;
			this->callback = callback;
			currentState = BrowseState::Browsing;
		}
		void AssetBrowser::Draw()
		{
			if (currentState == BrowseState::Inactive || currentState == BrowseState::Selected) return;

			PROFILE_SCOPE("Asset Browser");
			ImGui::Begin("Asset Browser", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

			ImGui::BeginChild("Shortcuts", ImVec2(0, 35), true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::SameLine();
			UpButton();
			ImGui::SameLine();
			BackButton();
			ImGui::SameLine();
			ForwardButton();
			Shortcuts();
			ImGui::EndChild(); // ---------- top bar ------------
			DirectoryField(); // 2nd top bar
			EntryList();

			if (fileSelectMode)
			{
				std::string fileSelectTextBuf = selectedEntry.path().filename().string();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 150);
				if (ImGui::InputText("##", &fileSelectTextBuf, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					if (selectedEntry.path().filename().string() != fileSelectTextBuf)
					{
						std::filesystem::directory_entry newSelection(currentPath / fileSelectTextBuf);
						if (std::filesystem::exists(newSelection))
						{
							selectedEntry = newSelection;
						}
					}
				}

				// Filter
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				ImGui::PushID(0);
				if (ImGui::BeginCombo("##", filterPresets[currentFilter].first.c_str(), ImGuiComboFlags_HeightSmall))
				{
					for (std::size_t x = 0; x < filterPresets.size(); ++x)
					{
						if (ImGui::Selectable(filterPresets[x].first.c_str()))
						{
							currentFilter = x;
							UpdateEntries(currentPath);
						}
					}
					ImGui::EndCombo();
				}
				ImGui::PopID();
			}

			ImGui::End();
		}

		std::filesystem::directory_entry AssetBrowser::GetSelection()
		{
			return selectedEntry;
		}
	}
}
