#pragma once
#include "enet/enet.h"
#include <Windows.h>
#include <vector>
#include <chrono>
#include <string>
#include <experimental/filesystem>
using namespace std;

inline void server_base_load() {
	ifstream ifs("config/main.json");
	if (!ifs.is_open()) {
		Server_alert("ERROR: Cannot load main.json for server_base!");
		while (1);
		exit(EXIT_FAILURE);
	}
	try {
		json j;
		ifs >> j;
		ifs.close();
		SERVER_PORT = j["port"].get<string>(), SERVER_IP = j["ip"].get<string>(), SERVER_CDN = j["cdn"].get<string>(), SERVER_GAME_VERSION = j["version"].get<string>();
		Server_alert("Config Loaded!\n[INFO] Hosting on: " + SERVER_IP + ":" + SERVER_PORT + "\n[INFO] Server Game Version: " + SERVER_GAME_VERSION + "");
		j.clear();
	}
	catch (...) {
		Server_alert("ERROR: Invalid Config!");
		while (1);
		exit(EXIT_FAILURE);
	}
}
inline void build_item_base() {
	int c_t = -1;
	{
		ifstream file("items.dat", std::ios::binary | std::ios::ate);
		itemsDatSize = (int)file.tellg();
		itemsDat = new BYTE[60 + itemsDatSize];
		string asdf = "0400000010000000FFFFFFFF000000000800000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
		for (int i = 0; i < (int)asdf.length(); i += 2) {
			char x = ch2n(asdf[i]);
			x = x << 4;
			x += ch2n(asdf[i + 1]);
			memcpy(itemsDat + (i / 2), &x, 1);
			if (asdf.length() > 60 * 2) throw 0;
		}
		memcpy(itemsDat + 56, &itemsDatSize, 4);
		file.seekg(0, std::ios::beg);
		if (file.read((char*)(itemsDat + 60), itemsDatSize)) {
			uint8_t* pData;
			int size = 0;
			const char filename[] = "items.dat";
			size = (int)filesize(filename);
			pData = getA((string)filename, &size, false, false);
			Server_alert("Updating items data success! Hash: " + to_string(HashString((unsigned char*)pData, size)) + "");
			itemdathash = HashString((unsigned char*)pData, size);
			file.close();
		} else {
			Server_alert("Updating item data failed!");
			while (1);
			exit(EXIT_FAILURE);
		}
	}
	ifstream i_e("config/CoreData.txt");
	if (not i_e.is_open()) {
		Server_alert("ERROR: Cannot load CoreData.txt for server_base!");
		while (1);
		exit(EXIT_FAILURE);
	}
	for (string line; getline(i_e, line);) {
		if (line.length() >= 9 && line[0] != '/' && line[1] != '/') {
			vector<string> ex = explode("|", line);
			ItemDefinition def;
			vector<string> properties = explode(",", ex[3]);
			string b_type = ex[4];
			def.id = atoi(ex[0].c_str());
			def.name = ex[1];
			def.rarity = atoi(ex[2].c_str());
			def.breakHits = atoi(ex[7].c_str());
			def.growTime = atoi(ex[8].c_str());
			def.properties = Property_Zero;
			for (auto& prop : properties) {
				if (prop == "NoSeed") {
					def.properties += Property_NoSeed;
				} else if (prop == "NoBlock") {
					def.properties += Property_NoBlock;
				} else if (prop == "Dropless") {
					def.properties += Property_Dropless;
				} else if (prop == "Beta") {
					def.properties += Property_Beta;
				} else if (prop == "Mod") {
					def.properties += Property_Mod;
				} else if (prop == "Untradable") {
					def.properties += Property_Untradable;
				} else if (prop == "Wrenchable") {
					def.properties += Property_Wrenchable;
				} else if (prop == "MultiFacing") {
					def.properties += Property_MultiFacing;
				} else if (prop == "Permanent") {
					def.properties += Property_Permanent;
				} else if (prop == "AutoPickup") {
					def.properties += Property_AutoPickup;
				} else if (prop == "WorldLock") {
					def.properties += Property_WorldLock;
				} else if (prop == "NoSelf") {
					def.properties += Property_NoSelf;
				} else if (prop == "RandomGrow") {
					def.properties += Property_RandomGrow;
				} else if (prop == "Public") {
					def.properties += Property_Public;
				} else if (prop == "Foreground") {
					def.properties += Property_Foreground;
				} else if (prop == "OnlyCombiner") {
					def.properties += Property_Chemical;
				}
			}
			if (b_type == "Foreground_Block") {
				def.blockType = BlockTypes::FOREGROUND;
			} else if (b_type == "Background_Block") {
				def.blockType = BlockTypes::BACKGROUND;
			} else if (b_type == "Seed") {
				def.blockType = BlockTypes::SEED;
			} else if (b_type == "Consummable") {
				def.blockType = BlockTypes::CONSUMABLE;
			} else if (b_type == "Pain_Block") {
				def.blockType = BlockTypes::PAIN_BLOCK;
			} else if (b_type == "Main_Door") {
				def.blockType = BlockTypes::MAIN_DOOR;
			} else if (b_type == "Mannequin") {
				def.blockType = BlockTypes::MANNEQUIN;
			} else if (b_type == "Bedrock") {
				def.blockType = BlockTypes::BEDROCK;
			} else if (b_type == "Door") {
				def.blockType = BlockTypes::DOOR;
			} else if (b_type == "Fist") {
				def.blockType = BlockTypes::FIST;
			} else if (b_type == "Sign") {
				def.blockType = BlockTypes::SIGN;
			} else if (b_type == "Back") {
				def.blockType = BlockTypes::BACKGROUND;
			} else if (b_type == "Sheet_Music") {
				def.blockType = BlockTypes::BACKGROUND;
			} else if (b_type == "Wrench") {
				def.blockType = BlockTypes::WRENCH;
			} else if (b_type == "Checkpoint") {
				def.blockType = BlockTypes::CHECKPOINT;
			} else if (b_type == "Lock") {
				def.blockType = BlockTypes::LOCK;
			} else if (b_type == "Gateway") {
				def.blockType = BlockTypes::GATEWAY;
			} else if (b_type == "Clothing") {
				def.blockType = BlockTypes::CLOTHING;
			} else if (b_type == "Platform") {
				def.blockType = BlockTypes::PLATFORM;
			} else if (b_type == "SFX_Foreground") {
				def.blockType = BlockTypes::SFX_FOREGROUND;
			} else if (b_type == "Gems") {
				def.blockType = BlockTypes::GEMS;
			} else if (b_type == "Toggleable_Foreground") {
				def.blockType = BlockTypes::TOGGLE_FOREGROUND;
			} else if (b_type == "Chemical_Combiner") {
				def.blockType = BlockTypes::CHEMICAL_COMBINER;
			} else if (b_type == "Treasure") {
				def.blockType = BlockTypes::TREASURE;
			} else if (b_type == "Deadly_Block") {
				def.blockType = BlockTypes::DEADLY;
			} else if (b_type == "Trampoline_Block") {
				def.blockType = BlockTypes::TRAMPOLINE;
			} else if (b_type == "Animated_Foreground_Block") {
				def.blockType = BlockTypes::ANIM_FOREGROUND;
			} else if (b_type == "Portal") {
				def.blockType = BlockTypes::PORTAL;
			} else if (b_type == "Random_Block") {
				def.blockType = BlockTypes::RANDOM_BLOCK;
			} else if (b_type == "Bouncy") {
				def.blockType = BlockTypes::BOUNCY;
			} else if (b_type == "Chest") {
				def.blockType = BlockTypes::CHEST;
			} else if (b_type == "Switch_Block") {
				def.blockType = BlockTypes::SWITCH_BLOCK;
			} else if (b_type == "Magic_Egg") {
				def.blockType = BlockTypes::MAGIC_EGG;
			} else if (b_type == "Crystal") {
				def.blockType = BlockTypes::CRYSTAL;
			} else if (b_type == "Mailbox") {
				def.blockType = BlockTypes::MAILBOX;
			} else if (b_type == "Bulletin_Board") {
				def.blockType = BlockTypes::BULLETIN_BOARD;
			} else if (b_type == "Faction") {
				def.blockType = BlockTypes::FACTION;
			} else if (b_type == "Component") {
				def.blockType = BlockTypes::COMPONENT;
			} else if (b_type == "Weather_Machine") {
				def.blockType = BlockTypes::WEATHER;
			} else if (b_type == "ItemSucker") {
				def.blockType = BlockTypes::SUCKER;
			} else if (b_type == "Fish_Tank_Port") {
				def.blockType = BlockTypes::FISH;
			} else if (b_type == "Steam_Block") {
				def.blockType = BlockTypes::STEAM;
			} else if (b_type == "ground_Block") {
				def.blockType = BlockTypes::GROUND_BLOCK;
			} else if (b_type == "Display") {
				def.blockType = BlockTypes::DISPLAY;
			} else if (b_type == "Untrade_A_Box" || b_type == "Safe_Vault") {
				def.blockType = BlockTypes::STORAGE;
			} else if (b_type == "Vending") {
				def.blockType = BlockTypes::VENDING;
			} else if (b_type == "Donation_Box") {
				def.blockType = BlockTypes::DONATION;
			} else if (b_type == "Phone_Booth") {
				def.blockType = BlockTypes::PHONE;
			} else if (b_type == "Sewing_Machine") {
				def.blockType = BlockTypes::SEWINGMACHINE;
			} else if (b_type == "Crime_Villain") {
				def.blockType = BlockTypes::CRIME_VILLAIN;
			} else if (b_type == "Provider") {
				def.blockType = BlockTypes::PROVIDER;
			} else {
				def.blockType = BlockTypes::UNKNOWN;
			}
			if (++c_t != def.id) {
				Server_alert("Critical error! Unordered database at item " + to_string(c_t) + "/" + to_string(def.id) + "!");
			}
			itemDefs.push_back(def);
		}
		maxItems++;
	}
	i_e.close();
	Server_alert("Built " + to_string(maxItems) + " Items Database!");
}