#include <AssetBrowser.h>
#include <imgui.h>
#include <Debug.h>
#include <imgui_stdlib.h>
#include <AssetMan.h>

namespace fl
{
	namespace Editor
	{
		// For size display
		constexpr unsigned long KILOBYTE = 1024;
		constexpr unsigned long MEGABYTE = 1024 * 1024;
		constexpr unsigned long GIGABYTE = 1024 * 1024 * 1024;

		std::string formatSize(unsigned long bytes)
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
			directoryEntries.clear();

			if (filter.size())
			{
				for (const auto& dirEntry : std::filesystem::directory_iterator(currentPath))
				{
					if (dirEntry.is_regular_file())
					{
						auto entryExtension = dirEntry.path().extension();

						for (auto& extension : filter)
						{
							if (entryExtension == extension)
							{
								directoryEntries.push_back(dirEntry);
								directoryEntriesSorted.push_back(DirEntry(dirEntry, directoryEntriesSorted.size()));
								break;
							}
						}
					}
					else if (dirEntry.is_directory())
					{
						directoryEntries.push_back(dirEntry);
						directoryEntriesSorted.push_back(DirEntry(dirEntry, directoryEntriesSorted.size()));
					}
				}
			}
			else
			{
				try
				{
					if (currentPath == "") currentPath = AssetMan::getBaseDirectory();
					for (const auto& dirEntry : std::filesystem::directory_iterator(currentPath))
					{
						directoryEntries.push_back(dirEntry);
						directoryEntriesSorted.push_back(DirEntry(dirEntry, directoryEntriesSorted.size()));
					}
				}
				catch (std::filesystem::filesystem_error e)
				{
					Debug::log()->info("{}, {}, {}, {}, {}", e.code().value(), e.code().message(), e.what(), e.path1().string(), e.path2().string());
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
		void AssetBrowser::SetFilter(std::vector<std::filesystem::path> filter)
		{
			this->filter = filter;
			UpdateEntries(currentPath);
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

		void AssetBrowser::Draw()
		{
			ImGui::Begin("Asset Browser", NULL);

			ImGui::BeginChild("Shortcuts", ImVec2(0, 35), true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_HorizontalScrollbar);

			ImGui::SameLine();
			UpButton();
			ImGui::SameLine();
			BackButton();
			ImGui::SameLine();
			ForwardButton();
			Shortcuts();

			ImGui::EndChild();

			DirectoryField();

			// Main Pane
			if (listDisplay)
			{
				ImGuiTableFlags flags =
				ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
				| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
				| ImGuiTableFlags_ScrollY;

				if (ImGui::BeginTable("DirectoryList", 3, flags))
				{
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort, 0.0f, MyItemColumnID_Name);
					ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_DefaultSort, 0.0f, MyItemColumnID_Size);
					ImGui::TableSetupColumn("????", ImGuiTableColumnFlags_DefaultSort, 0.0f, MyItemColumnID_Name);
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
						if (directoryEntriesSorted[x].entry.is_regular_file())
						{
							ImGui::Text(formatSize(directoryEntriesSorted[x].entry.file_size()).c_str());
						}
						else
						{
							ImGui::TextDisabled("-");
						}

						// ???
						ImGui::TableSetColumnIndex(2);
						ImGui::Text("IDK");

						// Allow row to be selected
						ImGui::SameLine();
						ImGui::Selectable(std::to_string(x).c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick);

						if (ImGui::IsItemHovered())
						{
							ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, 1);

							if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
							{
								// TODO: Set selected
							}

							if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
							{
								if (directoryEntries[x].is_directory())
								{
									UpdateEntries(directoryEntries[x]);
								}
								else if (directoryEntries[x].is_regular_file())
								{
									Debug::log()->error("Not implemented!");
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
											case MyItemColumnID_Name:
											{
												delta = a.entry.path().string().compare(b.entry.path().string());
												break;
											}
											case MyItemColumnID_Size:
											{
												unsigned long aSize = a.entry.is_regular_file() ? a.entry.file_size() : 0;
												unsigned long bSize = b.entry.is_regular_file() ? b.entry.file_size() : 0;
												delta = aSize - bSize;
												break;
											}
											default: IM_ASSERT(0); break;
										}

										if (delta != 0) return ((delta > 0) == (sort_spec->SortDirection == ImGuiSortDirection_Ascending));
										else
										{
											return ((a.id > b.id) == ImGuiSortDirection_Ascending);
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
				for (std::size_t x = 0; x < directoryEntries.size(); x++)
				{
					ImGui::PushID(x);
					bool clicked = ImGui::Button(directoryEntries[x].path().filename().string().c_str(), buttonSize);
					float last_button_x2 = ImGui::GetItemRectMax().x;
					float next_button_x2 = last_button_x2 + style.ItemSpacing.x + buttonSize.x; // Expected position if next button was on same line
					if (x + 1 < directoryEntries.size() && next_button_x2 < window_visible_x2) ImGui::SameLine();
					ImGui::PopID();

					if (clicked)
					{
						UpdateEntries(directoryEntries[x]);
					}
				}
			}

			// TODO: Path field
			std::string yea;
			ImGui::InputText("Test", &yea);

			ImGui::End();
		}
	}
}
