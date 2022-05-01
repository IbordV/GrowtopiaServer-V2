#pragma once
#include "enet/enet.h"
#include <Windows.h>
#include <vector>
#include <chrono>
#include <experimental/filesystem>
#include "player_base.h"
using namespace chrono;

#define Property_Zero 0
#define Property_NoSeed 1
#define Property_Dropless 2
#define Property_Beta 4
#define Property_Mod 8
#define Property_Chemical 12
#define Property_Untradable 16
#define Property_Wrenchable 32
#define Property_MultiFacing 64
#define Property_Permanent 128
#define Property_AutoPickup 256
#define Property_WorldLock 512
#define Property_NoSelf 1024
#define Property_RandomGrow 2048
#define Property_Public 4096
#define Property_Foreground 8192
#define Property_NoBlock 16384

enum ClothTypes {
	HAIR,SHIRT,
	PANTS, FEET, FACE,
	HAND, BACK, MASK,
	NECKLACE, ANCES, NONE
};

enum BlockTypes {
	FOREGROUND, BACKGROUND, CONSUMABLE, SEED, PAIN_BLOCK, BEDROCK,
	MAIN_DOOR, SIGN, DOOR, CLOTHING, FIST, WRENCH, CHECKPOINT, MANNEQUIN, LOCK, GATEWAY,
	TREASURE, WEATHER, TRAMPOLINE, TOGGLE_FOREGROUND, CHEMICAL_COMBINER, SWITCH_BLOCK, SFX_FOREGROUND,
	RANDOM_BLOCK, PORTAL, PLATFORM, MAILBOX, MAGIC_EGG, CRYSTAL, GEMS, DEADLY, CHEST, FACTION,
	BULLETIN_BOARD, BOUNCY,
	ANIM_FOREGROUND, COMPONENT, SUCKER, FISH, STEAM, GROUND_BLOCK, DISPLAY, STORAGE,
	VENDING, DONATION, PHONE, SEWINGMACHINE, CRIME_VILLAIN, PROVIDER, UNKNOWN
};

struct ItemDefinition {
	int id;
	int rarity;
	int growTime;
	int breakHits;
	int properties;
	string name = "";
	string description = "";
	ClothTypes clothType;
	BlockTypes blockType;
};
vector<ItemDefinition> itemDefs;

struct DroppedItem {
	int id, uid, count;
	float x, y;
};

struct WorldItem {
	__int16 foreground = 0; 
	__int16 background = 0;
	int int_data = 0, growTime = 0, fruit_count = 0, breakLevel = 0;
	string string_data = "";
	bool water_state = false, fire_state = false, flipped = false, activated = false, opened = false, silenced = false;
	long long int breakTime = 0;
};

struct World {
	string name, owner_name, display_owner;
	int width = 100, height = 60;
	int droppedCount = 0, weather = 0, ownerID = 0, w_level = 0;
	bool allow_drp = false, isNuked = false, isPublic = false;
	WorldItem* items;
	vector<DroppedItem> droppedItems{};
	vector<string> accessed{};
};

struct AWorld {
	World* ptr;
	World info;
	int id = -1;
};

class WorldDB
{
public:
	World get(string name);
	World get_pointer(string name);
	AWorld get2(string name);
};
vector<World> worlds;
WorldDB worldDB;

ItemDefinition getItemDef(const int id) {
	try {
		if (id < (int)itemDefs.size() && id > 0 && id <= maxItems) {
			return itemDefs.at(id);
		}
	}
	catch (...) {}
	return itemDefs.at(0);
}
World createWorld(string name, int width, int height) {
	World world;
	world.name = name;
	world.width = width;
	world.height = height;
	world.items = new WorldItem[world.width * world.height];
	int randMB = (rand() % 100);
	for (int i = 0; i < world.width * world.height; i++) {
		if (i >= 3700) world.items[i].background = 14;
		if (i >= 3700) world.items[i].foreground = 2;
		if (i == 3600 + randMB) world.items[i].foreground = 6;
		else if (i >= 3600 && i < 3700) world.items[i].foreground = 0;
		if (i == 3700 + randMB) world.items[i].foreground = 8;
		if (i >= 3800 && i < 5400 && !(rand() % 48)) { world.items[i].foreground = 10; }
		if (i >= 5000 && i < 5400 && !(rand() % 6)) { world.items[i].foreground = 4; }
		else if (i >= 5400) { world.items[i].foreground = 8; }
	}
	return world;
}
AWorld WorldDB::get2(string name) {
	AWorld ret;
	try {
		name = toUpperText(name);
		if (name.length() < 1) throw 1;
		for (char c : name) {
			if ((c < 'A' || c>'Z') && (c < '0' || c>'9')) throw 2;
		}
		if (name == "EXIT") throw 3;
		const auto p = std::find_if(worlds.begin(), worlds.end(), [name](const World& a) { return a.name == name; });
		if (p != worlds.end()) {
			ret.id = p - worlds.begin();
			ret.info = worlds.at(p - worlds.begin());
			ret.ptr = &worlds.at(p - worlds.begin());
			return ret;
		}
		try {
			ifstream r_w("worlds/_" + name + ".json");
			if (not r_w.is_open()) {
				World info = createWorld(name, 100, 60);
				worlds.push_back(info);
				ret.id = worlds.size() - 1;
				ret.info = info;
				ret.ptr = &worlds.at(worlds.size() - 1);
				return ret;
			}
			json j;
			r_w >> j;
			r_w.close();
			World info;
			info.name = j["name"].get<string>();
			info.owner_name = j["owner"].get<string>();
			info.display_owner = j["displayOwner"].get<string>();
			info.ownerID = j["ownerID"].get<int>();
			info.width = j["width"].get<int>();
			info.height = j["height"].get<int>();
			info.droppedCount = j["dc"].get<int>();
			info.weather = j["weather"].get<int>();
			info.w_level = j["worldlevel"].get<int>();
			info.allow_drp = j["allowDrop"].get<bool>();
			info.isNuked = j["isNuked"].get<bool>();
			info.isPublic = j["isPublic"].get<bool>();
			string admin_at = j["admins"];
			if (admin_at != "") {
				vector<string> infoDats = explode("|", admin_at);
				for (auto i = 0; i < (int)infoDats.size(); i++) {
					if (infoDats.at(i) == "") continue;
					info.accessed.push_back(infoDats.at(i));
				}
			}
			json dropped_ = j["dropped"];
			for (auto i = 0; i < info.droppedCount; i++) {
				DroppedItem d_;
				d_.count = dropped_[i]["c"].get<BYTE>();
				d_.id = dropped_[i]["id"].get<short>();
				d_.x = (float)dropped_[i]["x"].get<int>();
				d_.y = (float)dropped_[i]["y"].get<int>();
				d_.uid = dropped_[i]["uid"].get<int>();
				info.droppedItems.push_back(d_);
			}
			json tiles = j["tiles"];
			info.items = new WorldItem[6000];
			for (auto i = 0; i < 6000; i++) {
				info.items[i].foreground = tiles[i]["f"].get<int>();
				info.items[i].background = tiles[i]["b"].get<int>();
				info.items[i].int_data = tiles[i]["id"].get<int>();
				info.items[i].growTime = tiles[i]["gt"].get<int>();
				info.items[i].fruit_count = tiles[i]["fc"].get<int>();
				info.items[i].string_data = tiles[i]["sd"].get<string>();
				if (not info.items[i].string_data.empty()) {
					if (info.items[i].string_data.find(",")) {
						vector<string> s_ = explode(",", info.items[i].string_data);
						if (s_.at(0) == "1") info.items[i].water_state = true;
						if (s_.at(1) == "1") info.items[i].fire_state = true;
						if (s_.at(2) == "1") info.items[i].flipped = true;
						if (s_.at(3) == "1") info.items[i].activated = true;
						if (s_.at(4) == "1") info.items[i].opened = true;
						if (s_.at(5) == "1") info.items[i].silenced = true;
						s_.clear();
					}
				}
			}
			worlds.push_back(info);
			j.clear(); tiles.clear(); dropped_.clear();
			ret.id = worlds.size() - 1;
			ret.info = info;
			ret.ptr = &worlds.at(worlds.size() - 1);
			return ret;
		}
		catch (exception& e) {
			cout << e.what() << endl;
			World info = createWorld(name, 100, 60);
			worlds.push_back(info);
			ret.id = worlds.size() - 1;
			ret.info = info;
			ret.ptr = &worlds.at(worlds.size() - 1);
			return ret;
		}
	}
	catch (const out_of_range& e) {
		cout << e.what() << endl;
		AWorld ret;
		ret.id = -1;
		return ret;
	}
	throw 1;
}
World WorldDB::get(string name) {
	return this->get2(name).info;
}
World* getPlyersWorld(ENetPeer* peer) {
	try {
		return worldDB.get2(pInfo(peer)->currentWorld).ptr;
	}
	catch (...) {
		return NULL;
	}
}

inline void peer_connect(ENetPeer* peer) {
	for (ENetPeer* c_p = server->peers; c_p < &server->peers[server->peerCount]; ++c_p) {
		if (c_p->state != ENET_PEER_STATE_CONNECTED) continue;
		if (peer != c_p) {
			if (isHere(peer, c_p)) {
				string invis_cp = "0", invis_ = "0", netid_ = to_string(pInfo(peer)->netID), netid_cp = to_string(pInfo(c_p)->netID), userid_cp = to_string(pInfo(c_p)->userID);
				if (pInfo(c_p)->isInv) invis_cp = "1";
				if (pInfo(peer)->isInv) invis_ = "1";
				Variant::OnSpawn(peer, "spawn|avatar\nnetID|" + netid_cp + "\nuserID|" + userid_cp + "\ncolrect|0|0|20|30\nposXY|" + to_string(pInfo(c_p)->x) + "|" + to_string(pInfo(c_p)->y) + "\nname|``" + pInfo(c_p)->displayName + "``\ncountry|" + pInfo(c_p)->country + "\ninvis|" + invis_cp + "\nmstate|0\nsmstate|0\n");
				Variant::OnSpawn(c_p, "spawn|avatar\nnetID|" + netid_ + "\nuserID|" + userid_cp + "\ncolrect|0|0|20|30\nposXY|" + to_string(pInfo(peer)->x) + "|" + to_string(pInfo(peer)->y) + "\nname|``" + pInfo(peer)->displayName + "``\ncountry|" + pInfo(peer)->country + "\ninvis|" + invis_ + "\nmstate|0\nsmstate|0\n");
			}
		}
	}
}
inline bool isWorldAdmin(ENetPeer* peer, World* world) {
	try {
		for (int i = 0; i < (int)world->accessed.size(); i++) {
			if (world->accessed.at(i) == "") continue;
			if (world->accessed.at(i) == pInfo(peer)->rawName) return true;
		}
	}
	catch (const std::out_of_range& e) {
		std::cout << e.what() << std::endl;
	}
	return false;
}
inline void send_wo(ENetPeer* peer) {
	gamepacket_t p;
	p.Insert("OnRequestWorldSelectMenu");
	if (not pInfo(peer)->haveGrowId) {
		p.Insert("");
	} else {
		p.Insert("add_filter|\nadd_heading|Random Worlds<ROW2>|\nadd_floater|START|1|0.5|3529161471\n");
	}
	p.CreatePacket(peer);
}
inline int matching_state(ENetPeer* peer, World* worldInfo, int loc_) {
	int type = 0x00000000;
	if (worldInfo->items[loc_].activated) type |= 0x00400000;
	if (worldInfo->items[loc_].flipped) type |= 0x00200000;
	if (worldInfo->items[loc_].silenced) type |= 0x02000000;
	if (worldInfo->items[loc_].water_state) type |= 0x04000000;
	if (worldInfo->items[loc_].fire_state) type |= 0x10000000;
	return type;
}
inline void sendWorld(ENetPeer* peer, World* worldInfo) {
	try {
		if (pInfo(peer)->lobby_guest_acc) return;
		pInfo(peer)->joinClothesUpdated = false;
		int zero = 0;
		string asdf = "0400000004A7379237BB2509E8E0EC04F8720B050000000000000000FBBB0000010000007D920100FDFDFDFD04000000040000000000000000000000070000000000";
		string worldName = worldInfo->name;
		int xSize = worldInfo->width;
		int ySize = worldInfo->height;
		int square = xSize * ySize;
		short nameLen = (__int16_t)(worldName.length());
		int payloadLen = asdf.length() / 2;
		int dataLen = payloadLen + 2 + nameLen + 12 + (square * 8) + 4 + 100;
		int offsetData = dataLen - 100;
		int allocMem = payloadLen + 2 + nameLen + 12 + (square * 8) + 4 + 16000 + 100 + (worldInfo->droppedItems.size() * 20);
		BYTE* data = new BYTE[allocMem];
		memset(data, 0, allocMem);
		for (auto i = 0; i < (int)asdf.length(); i += 2)
		{
			char x = ch2n(asdf[i]);
			x = x << 4;
			x += ch2n(asdf[i + 1]);
			memcpy(data + (i / 2), &x, 1);
		}
		__int16_t item = 0;
		int smth = 0;
		for (int i = 0; i < square * 8; i += 4) memcpy(data + payloadLen + i + 14 + nameLen, &zero, 4);
		for (int i = 0; i < square * 8; i += 8) memcpy(data + payloadLen + i + 14 + nameLen, &item, 2);
		memcpy(data + payloadLen, &nameLen, 2);
		memcpy(data + payloadLen + 2, worldName.c_str(), nameLen);
		memcpy(data + payloadLen + 2 + nameLen, &xSize, 4);
		memcpy(data + payloadLen + 6 + nameLen, &ySize, 4);
		memcpy(data + payloadLen + 10 + nameLen, &square, 4);
		BYTE* blockPtr = data + payloadLen + 14 + nameLen;
		int sizeofblockstruct = 8;
		for (auto i = 0; i < square; i++)
		{
			int tile = worldInfo->items[i].foreground;
			sizeofblockstruct = 8;
			if (tile == 6) {
				int type = 0x00000000;
				type = matching_state(peer, worldInfo, i);
				memcpy(blockPtr, &tile, 2);
				memcpy(blockPtr + 4, &type, 4);
				BYTE btype = 1;
				memcpy(blockPtr + 8, &btype, 1);
				string doorText = "EXIT";
				const char* doorTextChars = doorText.c_str();
				short length = (short)(doorText.size());
				memcpy(blockPtr + 9, &length, 2);
				memcpy(blockPtr + 11, doorTextChars, length);
				sizeofblockstruct += 4 + length;
				dataLen += 4 + length;
			}
			else if ((worldInfo->items[i].foreground == 0) or (worldInfo->items[i].foreground == 2) or (worldInfo->items[i].foreground == 8) or (worldInfo->items[i].foreground == 6) or (worldInfo->items[i].foreground == 4) or (worldInfo->items[i].foreground == 10) or getItemDef(worldInfo->items[i].foreground).blockType == BlockTypes::FOREGROUND or getItemDef(worldInfo->items[i].background).blockType == BlockTypes::BACKGROUND or getItemDef(worldInfo->items[i].foreground).blockType == BlockTypes::TOGGLE_FOREGROUND or getItemDef(worldInfo->items[i].foreground).blockType == BlockTypes::SWITCH_BLOCK or getItemDef(worldInfo->items[i].foreground).blockType == BlockTypes::CHEST or getItemDef(worldInfo->items[i].foreground).blockType == BlockTypes::CHEMICAL_COMBINER or getItemDef(worldInfo->items[i].foreground).blockType == BlockTypes::GATEWAY) {
				memcpy(blockPtr, &worldInfo->items[i].foreground, 2);
				int type = 0;
				type = matching_state(peer, worldInfo, i);
				memcpy(blockPtr + 4, &type, 4);
			} else {
				memcpy(blockPtr, &zero, 2);
			}
			memcpy(blockPtr + 2, &worldInfo->items[i].background, 2);
			blockPtr += sizeofblockstruct;
		}
		dataLen += 8;
		int itemcount = worldInfo->droppedItems.size();
		int itemuid = worldInfo->droppedCount;
		memcpy(blockPtr, &itemcount, 4);
		memcpy(blockPtr + 4, &itemuid, 4);
		blockPtr += 8;
		int iteminfosize = 16;
		for (auto i = 0; i < itemcount; i++)
		{
			int item1 = worldInfo->droppedItems[i].id;
			int count = worldInfo->droppedItems[i].count;
			int uid = worldInfo->droppedItems[i].uid;
			float x = worldInfo->droppedItems[i].x;
			float y = worldInfo->droppedItems[i].y;
			memcpy(blockPtr, &item1, 2);
			memcpy(blockPtr + 2, &x, 4);
			memcpy(blockPtr + 6, &y, 4);
			memcpy(blockPtr + 10, &count, 2);
			memcpy(blockPtr + 12, &uid, 4);
			blockPtr += iteminfosize;
			dataLen += iteminfosize;
		}
		dataLen += 100;
		blockPtr += 4;
		memcpy(blockPtr, &worldInfo->weather, 4);
		blockPtr += 4;
		offsetData = dataLen - 100;
		BYTE* data2 = new BYTE[101];
		memset(data2, 0, 101);
		memcpy(data2 + 0, &zero, 4);
		int weather = worldInfo->weather;
		memcpy(data2 + 4, &weather, 4);
		memcpy(data + dataLen - 4, &smth, 4);
		ENetPacket* packet2 = enet_packet_create(data, dataLen, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet2);
		pInfo(peer)->currentWorld = worldInfo->name;
		pInfo(peer)->lastworld = worldInfo->name;
		for (auto i = 0; i < square; i++) {
			if ((worldInfo->items[i].foreground == 0) or (worldInfo->items[i].foreground == 2) or (worldInfo->items[i].foreground == 8) or (worldInfo->items[i].foreground == 6) or (worldInfo->items[i].foreground == 4) or (worldInfo->items[i].foreground == 10) or getItemDef(worldInfo->items[i].foreground).blockType == BlockTypes::FOREGROUND or getItemDef(worldInfo->items[i].background).blockType == BlockTypes::BACKGROUND) {

			} else {
				int x = i % xSize, y = i / xSize;
				PlayerMoving m_v;
				m_v.packetType = 0x3;
				m_v.characterState = 0x0;
				m_v.x = (float)(i % worldInfo->width);
				m_v.y = (float)(i / worldInfo->height);
				m_v.punchX = (i % worldInfo->width);
				m_v.punchY = (i / worldInfo->height);
				m_v.XSpeed = 0;
				m_v.YSpeed = 0;
				m_v.netID = -1;
				m_v.plantingTree = worldInfo->items[i].foreground;
				SendPacketRaw(4, packPlayerMoving(&m_v), 56, nullptr, peer, ENET_PACKET_FLAG_RELIABLE);
			}
		}
		delete[] data;
		delete[] data2;
	}
	catch (const out_of_range& e) {
		cout << e.what() << endl;
	}
}
inline void join_world(ENetPeer* peer, string act_, bool fixed_gateway = false, bool door_gateway = false, int x_spwn = -1, int y_spwn = -1) {
	try {
		if (pInfo(peer)->haveGrowId and pInfo(peer)->inGame and pInfo(peer)->passed_data and !pInfo(peer)->tankIDName.empty()) {
			auto w_ = worldDB.get(act_);
			int p_world = c_playerWorld(act_), isvalid=1;
			string d_f_s = " `0[", status = "", is_invis = "";
			if (p_world >= 60) {
				if (fixed_gateway) {
					send_wo(peer);
					return;
				}
				send_packet_(peer, "action|log\nmsg|`oWorld is at max capacity. Try again later...");
				isvalid = 0;
			} else if (w_.isNuked and not isDev(peer) and not isMod(peer)) {
				if (fixed_gateway) {
					send_wo(peer);
					return;
				}
				send_packet_(peer, "action|log\nmsg|`oThat world is inaccessible.");
				isvalid = 0;
			} else if (w_.w_level > pInfo(peer)->level and w_.owner_name != pInfo(peer)->rawName and not isDev(peer) and not isMod(peer)) {
				if (fixed_gateway) {
					send_wo(peer);
					return;
				}
				send_packet_(peer, "action|log\nmsg|`oPlayers lower than level " + to_string(w_.w_level) + " can't enter " + w_.name + ".");
				isvalid = 0;
			} else if (w_.height * w_.width > 6000) {
				send_packet_(peer, "action|log\nmsg|`4To reduce confusion, that is not a valid world.``  Try another?");
				isvalid = 0;
			}  if (isvalid <= 0) {
				gamepacket_t p;
				p.Insert("OnFailedToEnterWorld");
				p.CreatePacket(peer);
				return;
			}
			sendWorld(peer, &w_);
			SendInventory(peer, pInfo(peer)->inventory);
			int x = 3040, y = 736;
			for (auto j = 0; j < w_.width * w_.height; j++) {
				if (w_.items[j].foreground == 6) {
					x = (j % w_.width) * 32, y = (j / w_.width) * 32;
				}
			}
			pInfo(peer)->x = x, pInfo(peer)->y = y;
			if (door_gateway and x_spwn != -1 and y_spwn != -1) {
				x = x_spwn * 32, y = y_spwn * 32;
				pInfo(peer)->x = x_spwn * 32, pInfo(peer)->y = y_spwn * 32;
			}
			d_f_s += "`0]";
			if (d_f_s == " `0[`0]") d_f_s = "";
			Variant::OnConsoleMessage(peer, "World `w" + w_.name + "``" + d_f_s + " `oentered. There are `w" + to_string(c_playerWorld(w_.name) - 1) + "`` other people here, `w" + to_string(get_player_c()) + "`` online.");
			if (not w_.owner_name.empty()) {
				try {
					ifstream r_p("players/_" + w_.owner_name + ".json");
					if (!r_p.is_open()) return;
					json j; r_p >> j; r_p.close();
					if (w_.owner_name == pInfo(peer)->rawName or isWorldAdmin(peer, &w_)) status = "``(`2ACCESS GRANTED``)";
					else if (w_.isPublic) status = "``(`9PUBLIC WORLD``)";
					Variant::OnConsoleMessage(peer, "`#[`w" + w_.name + "`$ World Locked`o by " + j["displayNameBackup"].get<string>() + "" + status + "`#]");
					j.clear();
				}
				catch (exception& e) {
					cout << e.what() << endl;
					return;
				}
			}
			if (w_.owner_name == pInfo(peer)->rawName) {
				if (not isDev(peer) or not isMod(peer) or pInfo(peer)->displayName.find("`4") == string::npos or not isVip(peer)) {
					if (not pInfo(peer)->isOfLegend) {
						pInfo(peer)->displayName = "`2" + pInfo(peer)->displayName;
					}
				}
			}
			if (isWorldAdmin(peer, &w_)) {
				if (not isDev(peer) or not isMod(peer) or pInfo(peer)->displayName.find("`4") == string::npos or not isVip(peer)) {
					if (not pInfo(peer)->isOfLegend) {
						pInfo(peer)->displayName = "`^" + pInfo(peer)->displayName;
					}
				}
			}
			if (pInfo(peer)->isInv) is_invis = "1";
			gamepacket_t p;
			p.Insert("OnSpawn");
			p.Insert("spawn|avatar\nnetID|" + to_string(cId) + "\nuserID|" + to_string(pInfo(peer)->userID) + "\ncolrect|0|0|20|30\nposXY|" + to_string(x) + "|" + to_string(y) + "\nname|``" + pInfo(peer)->displayName + "``\ncountry|" + pInfo(peer)->country + "\ninvis|" + is_invis + "\nmstate|0\nsmstate|0\nonlineID|\ntype|local\n");
			p.CreatePacket(peer);
			for (ENetPeer* c_p = server->peers; c_p < &server->peers[server->peerCount]; ++c_p) {
				if (c_p->state != ENET_PEER_STATE_CONNECTED) continue;
				if (peer != c_p) {
					if (isHere(peer, c_p)) {
						if (not pInfo(peer)->isInv) {
							Variant::OnTalkBubble(c_p, pInfo(peer)->netID, "`5<`w" + pInfo(peer)->displayName + "`` `5entered, `w" + to_string(c_playerWorld(pInfo(peer)->currentWorld) - 1) + "`` `5others here>```w", 0, true);
							Variant::OnConsoleMessage(c_p, "`5<`w" + pInfo(peer)->displayName + "`` `5entered, `w" + to_string(c_playerWorld(pInfo(peer)->currentWorld) - 1) + "`` `5others here>```w");
							Variant::PlayAudio(c_p, "audio/door_open.wav", 0);
						}
					}
				}
			}
			Variant::PlayAudio(peer, "audio/door_open.wav", 0);
			pInfo(peer)->netID = cId;
			peer_connect(peer);
			for (ENetPeer* c_p = server->peers; c_p < &server->peers[server->peerCount]; ++c_p) {
				if (c_p->state != ENET_PEER_STATE_CONNECTED) continue;
				if (peer != c_p) {
					if (isHere(peer, c_p)) {
						if (not pInfo(c_p)->isInv) {
							Variant::OnTalkBubble(peer, pInfo(c_p)->netID, pInfo(c_p)->displayName, 1, false);
						}
					}
				}
			}
			cId++;
		}
	}
	catch (const out_of_range& e) {
		cout << e.what() << endl;
	}
}
inline void join_w_setting(ENetPeer* peer, string act_, bool door_gateway = false, int x_to = -1, int y_to = -1) {
	try {
		bool is_valid = true;
		act_ = toUpperText(act_);
		vector<string> bad_name = { "MEMEK", "KONTOL", "FUCK", "SHIT", "DICK", "BOBO", "TANGA", "ANJING", "FVCK", "PUSSY", "AUTISM", "ANAL", "ANUS", "ERROR", "ASS", "BALLS", "BASTARD", "BITCH", "SYSTEM", "ADMIN", "VAGINA", "TITS", "SPUNKS", "SEX" };
		if (pInfo(peer)->haveGrowId and pInfo(peer)->inGame and pInfo(peer)->passed_data and not pInfo(peer)->tankIDName.empty()) {
			if (not door_gateway) {
				if (pInfo(peer)->world_join_left + 1500 > (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count()) {
					gamepacket_t p;
					p.Insert("OnFailedToEnterWorld");
					p.CreatePacket(peer);
					return;
				}
				pInfo(peer)->world_join_left = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
			}
			for (int i = 0; i < (int)bad_name.size(); i++) {
				if (act_.find(bad_name[i]) != string::npos) act_ = "DISNEYLAND";
			} 
			if (act_.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != string::npos) {
				send_packet_(peer, "action|log\nmsg|`oSorry, spaces and special characters are not allowed in world or door names.  Try again.");
				is_valid = false;
			} else if (act_ == "EXIT") {
				send_packet_(peer, "action|log\nmsg|`oExit from what? Press back if you're done playing.");
				is_valid = false;
			} else if (act_.size() < 1 or act_.size() >= 24) {
				send_packet_(peer, "action|log\nmsg|`4To reduce confusion, that is not a valid world name.``  Try another?");
				is_valid = false;
			} else if (act_ == "") {
				act_ = "START";
			} if (not is_valid) {
				gamepacket_t p;
				p.Insert("OnFailedToEnterWorld");
				p.CreatePacket(peer);
				return;
			} 
			if (door_gateway) {
				join_world(peer, act_, false, true, x_to, x_to);
				return;
			}
			join_world(peer, act_);
		} else {
			gamepacket_t p;
			p.Insert("OnFailedToEnterWorld");
			p.CreatePacket(peer);
			return;
		}
	}
	catch (const out_of_range& e) {
		cout << e.what() << endl;
	}
}
inline void left_world(ENetPeer* peer, bool door_gateway = false) {
	try {
		if (pInfo(peer)->haveGrowId and pInfo(peer)->inGame and pInfo(peer)->passed_data and not pInfo(peer)->tankIDName.empty()) {
			if (pInfo(peer)->lobby_guest_acc) enet_peer_disconnect_later(peer, 0);
			if (not door_gateway) {
				if (pInfo(peer)->world_join_left + 1500 > (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count()) {
					gamepacket_t p;
					p.Insert("OnFailedToEnterWorld");
					p.CreatePacket(peer);
					return;
				}
				pInfo(peer)->world_join_left = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
			}
			if (not isDev(peer) and not isMod(peer) and pInfo(peer)->displayName.find("`4") == string::npos and not isVip(peer)) {
				if (not pInfo(peer)->isOfLegend) {
					if (pInfo(peer)->displayName.find("`2")) {
						pInfo(peer)->displayName = pInfo(peer)->displayNameBackup;
					}
					if (pInfo(peer)->displayName.find("`^")) {
						pInfo(peer)->displayName = pInfo(peer)->displayNameBackup;
					}
				}
			}
			for (ENetPeer* c_p = server->peers; c_p < &server->peers[server->peerCount]; ++c_p) {
				if (c_p->state != ENET_PEER_STATE_CONNECTED || c_p->data == NULL) continue;
				if (peer != c_p) {
					if (isHere(peer, c_p)) {
						if (pInfo(peer)->isInv) {
							Variant::OnRemove(c_p, pInfo(peer)->netID);
							Variant::OnConsoleMessage(c_p, "`5<`w" + pInfo(peer)->displayName + "`` `5left, `w" + to_string(c_playerWorld(pInfo(peer)->currentWorld) - 1) + "`` `5others here>```w");
							Variant::OnTalkBubble(c_p, pInfo(peer)->netID, "`5<`w" + pInfo(peer)->displayName + "`` `5left, `w" + to_string(c_playerWorld(pInfo(peer)->currentWorld) - 1) + "`` `5others here>```w", 0, true);
							Variant::PlayAudio(c_p, "audio/door_shut.wav", 0);
						}
					}
				}
			}
			Variant::PlayAudio(peer, "audio/door_shut.wav", 0);
			pInfo(peer)->currentWorld = "EXIT";
		}
	}
	catch (const out_of_range& e) {
		cout << e.what() << endl;
	}	
}