#include "menu.h"
#include <string>
#include <vector>
#include <Windows.h>
#include "overlay/imgui/imgui.h"
#include "config.h"
#include "globals.h"
#include "keybinds.hpp"
#include "data_cache.h"
#include <unordered_set>
#include "util.hpp"
#include <iomanip>

using namespace globals;
using namespace config;

int selected_tab = 1;

// Global containers for item names and item data mapping
std::vector<std::string> item_names;
std::unordered_map<std::string, u_data_item*> unique_item_data;

void MenuTooltip(const char* text) {
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(450.0f);
		ImGui::TextUnformatted(text);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void PopulateUniqueItems() {
	std::unordered_set<std::string> seen_names;

	// Loop through cached world items
	for (auto& item : world_item_cache) {
		if (!item) continue;

		auto item_data = item->get_data();
		std::string name = item_data->get_name().read_string();

		// If name is unique, store in the map
		if (seen_names.insert(name).second) {  // Insert and check uniqueness
			unique_item_data[name] = item_data;
			item_names.push_back(name);
		}
	}
}

void menu::draw()
{
	static std::string selected_item_name = "";

	static bool menu_open = true;

	if (GetAsyncKeyState(config::menu_hotkey) & 1) {
		menu_open = !menu_open;
	}

	if (menu_open) {
		ImVec2 startPosition = ImVec2(20, 20);

		ImGui::SetNextWindowPos(startPosition, true ? ImGuiCond_Once : ImGuiCond_Always);

		ImGui::Begin("Hawk Tuah Protocol - Oni Edition");

		auto cursor_position = util::cursor_position();
		ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(cursor_position.x, cursor_position.y), 5.f, IM_COL32(255, 255, 255, 255));

		ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.1059f, 0.3765f, 0.6510f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.1059f, 0.3765f, 0.6510f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.1059f, 0.3765f, 0.6510f, 1.0f));

		float calculatedHeight = 0.0f;
		float itemHeight = ImGui::GetFrameHeightWithSpacing();
		float headerPadding = 15.0f;

		ImGui::BeginChild("Sidebar", ImVec2(75, 0), true);
		if (ImGui::Selectable("ESP", selected_tab == 1)) selected_tab = 1;
		if (ImGui::Selectable("PLAYER", selected_tab == 2)) selected_tab = 2;
		if (ImGui::Selectable("WEAPON", selected_tab == 3)) selected_tab = 3;
		if (ImGui::Selectable("ITEMS", selected_tab == 4)) selected_tab = 4;
		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::BeginChild("Content");

		// ESP
		if (selected_tab == 1) {
			calculatedHeight += itemHeight + headerPadding;

			if (ImGui::Checkbox("ESP##Master", &esp_enabled)) {
				player_esp = esp_enabled;
				weapon_esp = esp_enabled;
				task_object_esp = esp_enabled;
				primary_object_esp = esp_enabled;
				secondary_object_esp = esp_enabled;
			}
			ImGui::SameLine();
			ImHotkey("##ESPHotkey", &config::esp_hotkey);

			calculatedHeight += itemHeight;

			if (esp_enabled) {

				// Tabs for each ESP category
				if (ImGui::BeginTabBar("ESPTabBar")) {

					// Player ESP Tab
					if (ImGui::BeginTabItem("Player")) {
						ImGui::Checkbox("Enabled##PlayerESP", &player_esp);
						if (ImGui::CollapsingHeader("Details##PlayerESPDetails", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::Checkbox("Distance##PlayerDistance", &player_distance);
							calculatedHeight += itemHeight;
						}
						ImGui::EndTabItem();
					}

					// World ESP Tab
					if (ImGui::BeginTabItem("World")) {
						ImGui::Checkbox("Enabled##WorldESP", &task_object_esp);
						ImGui::SameLine(); MenuTooltip("Only works in self hosted lobbies.");
						calculatedHeight += itemHeight;

						// Begin custom side-by-side child sections
						float halfWidth = (ImGui::GetContentRegionAvail().x) / 2;

						// Details Section
						ImGui::BeginChild("DetailsSection", ImVec2(halfWidth, 0), true);
						if (ImGui::CollapsingHeader("Details##WorldESPDetails", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::Checkbox("Task State##TaskItemState", &task_object_state);
							calculatedHeight += itemHeight;
							ImGui::Checkbox("Distance##TaskDistance", &task_object_distance);
							calculatedHeight += itemHeight;
						}
						ImGui::EndChild();

						ImGui::SameLine();

						// Task Locations Section
						ImGui::BeginChild("TaskLocationsSection", ImVec2(halfWidth, 0), true);
						if (ImGui::CollapsingHeader("Task Locations##WorldESPTasks", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::Checkbox("Delivery##TaskLocation", &task_delivery);
							calculatedHeight += itemHeight;
							ImGui::Checkbox("Pressure##TaskLocation", &task_machine);
							calculatedHeight += itemHeight;
							ImGui::Checkbox("Vents##TaskLocation", &task_vent);
							calculatedHeight += itemHeight;
							ImGui::Checkbox("Alimentations##TaskLocation", &task_alim);
							calculatedHeight += itemHeight;
							ImGui::Checkbox("Pizzushi##TaskLocation", &task_pizzushi);
							calculatedHeight += itemHeight;
							ImGui::Checkbox("Computers##TaskLocation", &task_computers);
							calculatedHeight += itemHeight;
						}
						ImGui::EndChild();

						calculatedHeight += itemHeight;
						ImGui::EndTabItem();
					}

					// Weapon ESP Tab
					if (ImGui::BeginTabItem("Weapon")) {
						ImGui::Checkbox("Enabled##WeaponESP", &weapon_esp);
						if (ImGui::CollapsingHeader("Details##WeaponESPDetails", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::Checkbox("Item State##WeaponItemState", &weapon_item_state);
							calculatedHeight += itemHeight;
							ImGui::Checkbox("Distance##WeaponDistance", &weapon_distance);
							calculatedHeight += itemHeight;
						}
						ImGui::EndTabItem();
					}

					// Primary Object ESP Tab
					if (ImGui::BeginTabItem("Object")) {
						ImGui::Checkbox("Enabled##PrimaryObjectESP", &primary_object_esp);
						if (ImGui::CollapsingHeader("Details##PrimaryObjectDetails", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::Checkbox("Item State##PrimaryItemState", &primary_item_state);
							calculatedHeight += itemHeight;
							ImGui::Checkbox("Distance##PrimaryDistance", &primary_distance);
							calculatedHeight += itemHeight;
						}
						ImGui::EndTabItem();
					}

					// Secondary Object ESP Tab
					if (ImGui::BeginTabItem("Other Object")) {
						ImGui::Checkbox("Enabled##SecondaryObjectESP", &secondary_object_esp);
						if (ImGui::CollapsingHeader("Details##SecondaryObjectDetails", ImGuiTreeNodeFlags_DefaultOpen)) {
							ImGui::Checkbox("Item State##SecondaryItemState", &secondary_item_state);
							calculatedHeight += itemHeight;
							ImGui::Checkbox("Distance##SecondaryDistance", &secondary_distance);
							calculatedHeight += itemHeight;
						}
						ImGui::EndTabItem();
					}

					ImGui::EndTabBar();
				}
			}
		}
		// PLAYER
		else if (selected_tab == 2) {
			calculatedHeight += itemHeight + headerPadding;
			ImGui::Checkbox("Speedhack", &speedhack); calculatedHeight += itemHeight;
			ImGui::SameLine();
			ImHotkey("##SpeedhackHotkey", &config::speedhack_hotkey);

			ImGui::Checkbox("God Mode", &god_mode); calculatedHeight += itemHeight;
			ImGui::SameLine();
			ImHotkey("##GodmodeHotkey", &config::god_mode_hotkey);

			ImGui::Checkbox("Infinite Stamina", &infinite_stamina); calculatedHeight += itemHeight;
			ImGui::SameLine();
			ImHotkey("##InfstamHotkey", &config::infinite_stamina_hotkey);

			ImGui::Checkbox("Revive", &living_state); calculatedHeight += itemHeight;
		}
		// WEAPON
		else if (selected_tab == 3) {
			if (ImGui::CollapsingHeader("MELEE", ImGuiTreeNodeFlags_DefaultOpen)) {
				calculatedHeight += itemHeight + headerPadding;
				ImGui::Checkbox("Fast Melee", &fast_melee); calculatedHeight += itemHeight;
				ImGui::SameLine();
				ImHotkey("##FastMeleeHotkey", &config::fast_melee_hotkey);

				ImGui::Checkbox("Infinite Melee Range", &infinite_melee_range); calculatedHeight += itemHeight;
				ImGui::SameLine();
				ImHotkey("##InfMeleeRangeHotkey", &config::infinite_melee_range_hotkey);
			}
			calculatedHeight += itemHeight;
			if (ImGui::CollapsingHeader("GUN", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::Checkbox("Auto Fire", &auto_fire); calculatedHeight += itemHeight;
				ImGui::SameLine();
				ImHotkey("##AutoFireHotkey", &config::auto_fire_hotkey);

				ImGui::Checkbox("Rapid Fire", &rapid_fire); calculatedHeight += itemHeight;
				ImGui::SameLine();
				ImHotkey("##RapidFireHotkey", &config::rapid_fire_hotkey);

				ImGui::Checkbox("No Recoil", &no_recoil); calculatedHeight += itemHeight;
				ImGui::SameLine();
				ImHotkey("##NoRecoilHotkey", &config::no_recoil_hotkey);

				ImGui::Checkbox("Max Damage", &max_damage); calculatedHeight += itemHeight;
				ImGui::SameLine();
				ImHotkey("##MaxDamageHotkey", &config::max_damage_hotkey);

				ImGui::Checkbox("Infinite Ammo", &infinite_ammo); calculatedHeight += itemHeight;
			}
			calculatedHeight += itemHeight;
		}
		// ITEMS
		else if (selected_tab == 4) {
			auto hand_item = local_mec->get_hand_item();
			auto bag_item = local_mec->get_bag_item();
			auto hand_inventory = hand_item->get_can_inventory();

			if (ImGui::CollapsingHeader("INVENTORY", ImGuiTreeNodeFlags_DefaultOpen)) {
				if (hand_item) {
					std::string hand_item_name = hand_item->get_name().read_string();

					std::string display_name;
					if (hand_item_name == "NAME") {
						display_name = "PIZZUSHI";
					}
					else {
						display_name = hand_item_name;
					}
					ImGui::Text("Hand Item: %s", display_name.c_str());
					ImGui::SameLine();
					if (ImGui::BeginCombo("##ChangeHandItem", selected_item_name.empty() ? "Select Item" : selected_item_name.c_str())) {
						for (const auto& item_name : item_names) {
							if (ImGui::Selectable(item_name.c_str(), item_name == selected_item_name)) {
								selected_item_name = item_name;

								// Set the hand item based on the selected item name
								auto item_data = unique_item_data[item_name];
								if (item_data) {
									local_mec->set_hand_item(item_data);
								}

								// Reset combo box selection after changing the item
								selected_item_name = "";
							}
						}
						ImGui::EndCombo();
					}
					calculatedHeight += itemHeight;
					auto hand_state = local_mec->get_hand_state();
					if (hand_item_name == "GAZ BOTTLE") {
						const char* colors[] = { "Yellow", "Red", "Blue" };
						int current_value = hand_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Color", &current_value, colors, IM_ARRAYSIZE(colors))) {
							hand_state.Value_8 = current_value;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item_name == "PACKAGE") {
						const char* package_types[] = { "Security", "Computers", "Botanic", "Restaurant", "Medical", "Tutorial" , "Machine" };
						int package_value = hand_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##PackageTypes", &package_value, package_types, IM_ARRAYSIZE(package_types))) {
							hand_state.Value_8 = package_value + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item_name == "FISH") {
						const char* fish_types[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
						int fish_value = hand_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##FishType", &fish_value, fish_types, IM_ARRAYSIZE(fish_types))) {
							hand_state.Value_8 = fish_value + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item_name == "RICE") {
						const char* rice_types[] = { "White", "Brown", "Black" };
						int rice_value = hand_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##RiceType", &rice_value, rice_types, IM_ARRAYSIZE(rice_types))) {
							hand_state.Value_8 = rice_value + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item_name == "CONTAINER") {
						const char* colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red", "White Rice", "Brown Rice", "Black Rice" };
						int current_value = hand_state.Value_8;
						int selected_index = current_value;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Color", &selected_index, colors, IM_ARRAYSIZE(colors))) {
							if (selected_index >= 6) {
								hand_state.Value_8 = 6;
								switch (selected_index) {
								case 6: hand_state.Time_15 = 1; break;
								case 7: hand_state.Time_15 = 2; break;
								case 8: hand_state.Time_15 = 3; break;
								}
							}
							else {
								hand_state.Value_8 = selected_index;
								hand_state.Time_15 = 0;
							}
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item_name == "SAMPLE") {
						const char* colors[] = { "Green", "Yellow", "Blue", "White", "Red" };
						int current_index = hand_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##SampleColor", &current_index, colors, IM_ARRAYSIZE(colors))) {
							hand_state.Value_8 = current_index + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item_name == "FUSE") {
						const char* fuse_colors[] = { "Red", "Yellow", "Blue" };
						int time_color = hand_state.Time_15 - 1;
						int value_color = hand_state.Value_8 - 1;

						float half_width = (ImGui::GetContentRegionAvail().x) / 2;

						ImGui::SetNextItemWidth(half_width);
						if (ImGui::Combo("##ValueColor", &value_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
							hand_state.Value_8 = value_color + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;

						ImGui::SameLine();
						ImGui::SetNextItemWidth(half_width);
						if (ImGui::Combo("##TimeColor", &time_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
							hand_state.Time_15 = time_color + 1;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item_name == "NAME") {
						const char* rice_options[] = { "White Rice", "Brown Rice", "Black Rice" };
						const char* fish_options[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
						const char* container_colors[] = { "Green", "Yellow", "Blue", "White", "Red" };

						int value = hand_state.Value_8;
						int rice_value = value / 100;
						int fish_value = (value / 10) % 10;
						int container_value = value % 10;

						int rice_index = rice_value - 1;
						int fish_index = fish_value - 1;
						int container_index = container_value - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Rice", &rice_index, rice_options, IM_ARRAYSIZE(rice_options))) {
							rice_value = rice_index + 1;
						}
						calculatedHeight += itemHeight;
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Fish", &fish_index, fish_options, IM_ARRAYSIZE(fish_options))) {
							fish_value = fish_index + 1;
						}
						calculatedHeight += itemHeight;
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Container Color", &container_index, container_colors, IM_ARRAYSIZE(container_colors))) {
							container_value = container_index + 1;
						}
						calculatedHeight += itemHeight;

						hand_state.Value_8 = (rice_value * 100) + (fish_value * 10) + container_value;
						local_mec->set_hand_state(hand_state);
					} // Pizzushi
					else if (hand_item_name == "VENT FILTER") {
						int clean_percentage = hand_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderInt("##%", &clean_percentage, 0, 100, "Clean: %d%")) {
							hand_state.Value_8 = clean_percentage;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item_name == "BATTERY") {
						int charge_percentage = hand_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderInt("##%", &charge_percentage, 0, 100, "Charge: %d%")) {
							hand_state.Value_8 = charge_percentage;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (hand_item_name == "REVOLVER" || hand_item_name == "PISTOL" || hand_item_name == "SHORTY" || hand_item_name == "SMG" || hand_item_name == "RIFLE" || hand_item_name == "SHOTGUN") {
						int ammo = hand_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderInt("##Ammo", &ammo, 0, 100, "Ammo: %d%")) {
							hand_state.Value_8 = ammo;
							local_mec->set_hand_state(hand_state);
						}
						calculatedHeight += itemHeight;
					}
				}
				else {
					ImGui::Text("Hand Item: AIR");
					ImGui::SameLine();
					if (ImGui::BeginCombo("##ChangeHandItem", selected_item_name.empty() ? "Select Item" : selected_item_name.c_str())) {
						for (const auto& item_name : item_names) {
							if (ImGui::Selectable(item_name.c_str(), item_name == selected_item_name)) {
								selected_item_name = item_name;

								// Set the hand item based on the selected item name
								auto item_data = unique_item_data[item_name];
								if (item_data) {
									local_mec->set_hand_item(item_data);
								}

								// Reset combo box selection after changing the item
								selected_item_name = "";
							}
						}
						ImGui::EndCombo();
					}
					calculatedHeight += itemHeight;
				}
				ImGui::Separator();
				if (bag_item) {
					std::string bag_item_name = bag_item->get_name().read_string();

					std::string display_name;
					if (bag_item_name == "NAME") {
						display_name = "PIZZUSHI";
					}
					else {
						display_name = bag_item_name;
					}
					ImGui::Text("Bag Item: %s", display_name.c_str());
					ImGui::SameLine();
					if (ImGui::BeginCombo("##ChangeBagItem", selected_item_name.empty() ? "Select Item" : selected_item_name.c_str())) {
						for (const auto& item_name : item_names) {
							if (ImGui::Selectable(item_name.c_str(), item_name == selected_item_name)) {
								selected_item_name = item_name;

								// Set the hand item based on the selected item name
								auto item_data = unique_item_data[item_name];
								if (item_data) {
									local_mec->set_bag_item(item_data);
								}
							}
							// Reset combo box selection after changing the item
							selected_item_name = "";
						}
						ImGui::EndCombo();
					}
					calculatedHeight += itemHeight;

					auto bag_state = local_mec->get_bag_state();
					if (bag_item_name == "GAZ BOTTLE") {
						const char* colors[] = { "Yellow", "Red", "Blue" };
						int current_value = bag_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Color", &current_value, colors, IM_ARRAYSIZE(colors))) {
							bag_state.Value_8 = current_value;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item_name == "PACKAGE") {
						const char* package_types[] = { "Security", "Computers", "Botanic", "Restaurant", "Medical", "Tutorial" , "Machine" };
						int package_value = bag_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##PackageTypes", &package_value, package_types, IM_ARRAYSIZE(package_types))) {
							bag_state.Value_8 = package_value + 1;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item_name == "FISH") {
						const char* fish_types[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
						int fish_value = bag_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##FishType", &fish_value, fish_types, IM_ARRAYSIZE(fish_types))) {
							bag_state.Value_8 = fish_value + 1;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item_name == "RICE") {
						const char* rice_types[] = { "White", "Brown", "Black" };
						int rice_value = bag_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##RiceType", &rice_value, rice_types, IM_ARRAYSIZE(rice_types))) {
							bag_state.Value_8 = rice_value + 1;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item_name == "CONTAINER") {
						const char* colors[] = { "Empty", "Green", "Yellow", "Blue", "White", "Red", "White Rice", "Brown Rice", "Black Rice" };
						int current_value = bag_state.Value_8;
						int selected_index = current_value;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Color", &selected_index, colors, IM_ARRAYSIZE(colors))) {
							if (selected_index >= 6) {
								bag_state.Value_8 = 6;
								switch (selected_index) {
								case 6: bag_state.Time_15 = 1; break;
								case 7: bag_state.Time_15 = 2; break;
								case 8: bag_state.Time_15 = 3; break;
								}
							}
							else {
								bag_state.Value_8 = selected_index;
								bag_state.Time_15 = 0;
							}
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item_name == "SAMPLE") {
						const char* colors[] = { "Green", "Yellow", "Blue", "White", "Red" };
						int current_index = bag_state.Value_8 - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##SampleColor", &current_index, colors, IM_ARRAYSIZE(colors))) {
							bag_state.Value_8 = current_index + 1;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item_name == "FUSE") {
						const char* fuse_colors[] = { "Red", "Yellow", "Blue" };
						int time_color = bag_state.Time_15 - 1;
						int value_color = bag_state.Value_8 - 1;

						float half_width = (ImGui::GetContentRegionAvail().x) / 2;

						ImGui::SetNextItemWidth(half_width);
						if (ImGui::Combo("##ValueColor", &value_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
							bag_state.Value_8 = value_color + 1;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;

						ImGui::SameLine();
						ImGui::SetNextItemWidth(half_width);
						if (ImGui::Combo("##TimeColor", &time_color, fuse_colors, IM_ARRAYSIZE(fuse_colors))) {
							bag_state.Time_15 = time_color + 1;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item_name == "NAME") { // Pizzushi
						const char* rice_options[] = { "White Rice", "Brown Rice", "Black Rice" };
						const char* fish_options[] = { "Salmon", "Tuna", "Cod", "Shrimp" };
						const char* container_colors[] = { "Green", "Yellow", "Blue", "White", "Red" };

						int value = bag_state.Value_8;
						int rice_value = value / 100;
						int fish_value = (value / 10) % 10;
						int container_value = value % 10;

						int rice_index = rice_value - 1;
						int fish_index = fish_value - 1;
						int container_index = container_value - 1;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Rice", &rice_index, rice_options, IM_ARRAYSIZE(rice_options))) {
							rice_value = rice_index + 1;
						}
						calculatedHeight += itemHeight;
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Fish", &fish_index, fish_options, IM_ARRAYSIZE(fish_options))) {
							fish_value = fish_index + 1;
						}
						calculatedHeight += itemHeight;
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::Combo("##Container Color", &container_index, container_colors, IM_ARRAYSIZE(container_colors))) {
							container_value = container_index + 1;
						}
						calculatedHeight += itemHeight;

						bag_state.Value_8 = (rice_value * 100) + (fish_value * 10) + container_value;
						local_mec->set_bag_state(bag_state);
					}
					else if (bag_item_name == "VENT FILTER") {
						int clean_percentage = bag_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderInt("##%", &clean_percentage, 0, 100, "Clean: %d%")) {
							bag_state.Value_8 = clean_percentage;
							local_mec->set_bag_state(bag_state);
						}
						if (!ImGui::IsItemDeactivatedAfterEdit())
							calculatedHeight += itemHeight;
					}
					else if (bag_item_name == "BATTERY") {
						int charge_percentage = bag_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderInt("##%", &charge_percentage, 0, 100, "Charge: %d%")) {
							bag_state.Value_8 = charge_percentage;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
					else if (bag_item_name == "REVOLVER" || bag_item_name == "PISTOL" || bag_item_name == "SHORTY" || bag_item_name == "SMG" || bag_item_name == "RIFLE" || bag_item_name == "SHOTGUN") {
						int ammo = bag_state.Value_8;

						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::SliderInt("##Ammo", &ammo, 0, 100, "Ammo: %d%")) {
							bag_state.Value_8 = ammo;
							local_mec->set_bag_state(bag_state);
						}
						calculatedHeight += itemHeight;
					}
				}
				else {
					ImGui::Text("Bag Item: EMPTY");
					ImGui::SameLine();
					if (ImGui::BeginCombo("##ChangeBagItem", selected_item_name.empty() ? "Select Item" : selected_item_name.c_str())) {
						for (const auto& item_name : item_names) {
							if (ImGui::Selectable(item_name.c_str(), item_name == selected_item_name)) {
								selected_item_name = item_name;

								// Set the hand item based on the selected item name
								auto item_data = unique_item_data[item_name];
								if (item_data) {
									local_mec->set_bag_item(item_data);
								}
								// Reset combo box selection after changing the item
								selected_item_name = "";
							}
						}
						ImGui::EndCombo();
					}
					calculatedHeight += itemHeight + 10;
				}
				calculatedHeight += itemHeight;
			}
			calculatedHeight += itemHeight;

			if (hand_item) {
				if (!hand_inventory) {
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::Button("Can Inventory", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
						if (hand_item) {
							hand_item->set_can_inventory(!hand_inventory);
						}
					}
					calculatedHeight += itemHeight;
				}
				else {
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::Button("Can't Inventory", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
						if (hand_item) {
							hand_item->set_can_inventory(!hand_inventory);
						}
					}
					calculatedHeight += itemHeight;
				}
			}

			if (ImGui::CollapsingHeader("ITEM THROW DATA")) {
				calculatedHeight += itemHeight;
				auto itemThrow = hand_item->get_throw_type();
				auto itemThrowForce = itemThrow->get_throw_force();
				auto itemVerticalForce = itemThrow->get_vertical_force();
				auto itemVerticalOffset = itemThrow->get_vertical_offset();
				auto itemRestitution = itemThrow->get_restitution();
				auto itemGravity = itemThrow->get_gravity();
				auto itemDrag = itemThrow->get_drag();
				auto itemRadius = itemThrow->get_radius();

				// First column (left side)
				ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
				if (ImGui::SliderInt("##ThrowForce", &itemThrowForce, 0, 10000, "Throw Force: %d%")) {
					itemThrow->set_throw_force(itemThrowForce);
				}
				ImGui::SameLine();
				int intRestitution = static_cast<int>(itemRestitution * 100); // Scale double to int representation
				if (ImGui::SliderInt("##Restitution", &intRestitution, 0, 1000, "Restitution: %d")) {
					itemRestitution = intRestitution / 100.0; // Convert back to double
					itemThrow->set_restitution(itemRestitution);
				}
				calculatedHeight += itemHeight;

				if (ImGui::SliderInt("##VerticalForce", &itemVerticalForce, 0, 10000, "Vertical Force: %d%")) {
					itemThrow->set_vertical_force(itemVerticalForce);
				}
				ImGui::SameLine();
				int intGravity = static_cast<int>(itemGravity * 100); // Scale double to int representation
				if (ImGui::SliderInt("##Gravity", &intGravity, -10000, 10000, "Gravity: %d")) {
					itemGravity = intGravity / 100.0; // Convert back to double
					itemThrow->set_gravity(itemGravity);
				}
				calculatedHeight += itemHeight;

				if (ImGui::SliderInt("##VerticalOffset", &itemVerticalOffset, -100, 100, "Vertical Offset: %d%")) {
					itemThrow->set_vertical_offset(itemVerticalOffset);
				}
				ImGui::SameLine();
				int intDrag = static_cast<int>(itemDrag * 100); // Scale double to int representation
				if (ImGui::SliderInt("##Drag", &intDrag, 0, 1000, "Drag: %d")) {
					itemDrag = intDrag / 100.0; // Convert back to double
					itemThrow->set_drag(itemDrag);
				}
				calculatedHeight += itemHeight;

				int intRadius = static_cast<int>(itemRadius * 100); // Scale double to int representation
				if (ImGui::SliderInt("##Radius", &intRadius, -1000, 1000, "Radius: %d")) {
					itemRadius = intRadius / 100.0; // Convert back to double
					itemThrow->set_radius(itemRadius);
				}
				calculatedHeight += itemHeight;
				// Restore original width
				ImGui::PopItemWidth();
			} ImGui::SameLine(); MenuTooltip("Only works in self hosted lobbies.");
		}

		ImGui::EndChild();

		//ImGui::SetWindowSize(ImVec2(-1, calculatedHeight));
		if (calculatedHeight <= 200.0f) {
			ImGui::SetWindowSize(ImVec2(500.f, 200.0f));
		}
		else {
			ImGui::SetWindowSize(ImVec2(500.f, calculatedHeight));
		}

		ImGui::PopStyleColor(3);

		ImGui::End();
	}
}
