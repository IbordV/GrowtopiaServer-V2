#pragma once
#include "enet/enet.h"
#include <Windows.h>
#include <vector>
#include <chrono>
#include <string>
#include <experimental/filesystem>
using namespace std;
using namespace httplib;

struct InventoryItem {
	size_t itemID;
	size_t itemCount;
};

struct PlayerInventory {
	vector<InventoryItem> items;
};

struct PlayerInfo {
	//gw_id
	PlayerInventory inventory;
	vector<string> friendinfo{};
	vector<string> worldsowned{};

	string currentWorld = "";
	//login_info
	short currentInventorySize = 0;
	int peer_id = 0, xp = 0, userID = 0, skin = 0, level = 0, adminLevel = 0, effect = 0, netID = -1, x = -1, y = -1, update_req = 0;
	int cloth_back = 0, cloth_hand = 0, cloth_face = 0, cloth_shirt = 0, cloth_pants = 0, cloth_neck = 0, cloth_hair = 0, cloth_feet = 0, cloth_mask = 0, cloth_ances = 0;
	bool inGame = false, check_login = false, passed_data = false, lobby_guest_acc = false, haveGrowId = false, block_login = false, joinClothesUpdated = false, isRotatedLeft = false, RotatedLeft = false, Supporter = false, isSusspend = false, isOfLegend = false, isMuted = false, isDrTitle = false, isCursed = false, canDoubleJump = false, isInv = false;
	string tankIDName = "", tankIDPass = "", requestedName = "", country = "", rawName = "", displayName = "", displayNameBackup = "", lastworld = "", guild = "", UUIDToken = "none";
	string ip_ply = "", mac_ply = "", game_Version = "", meta = "", platformID = "";

	//longlongint
	long long world_join_left = 0;
	long long playtime = 0;
	long long account_created = 0;
	long long int seconds = 0;
	long long int lastBreak = 0;
};

struct PlayerMoving {
	int packetType;
	int netID;
	int secondNetId;
	float x;
	float y;
	int characterState;
	int plantingTree;
	float XSpeed;
	float YSpeed;
	int punchX;
	int punchY;
};

BYTE* packPlayerMoving(PlayerMoving* dataStruct) {
	const auto data = new BYTE[64];
	for (auto i = 0; i < 64; i++)
	{
		data[i] = 0;
	}
	memcpy(data, &dataStruct->packetType, 4);
	memcpy(data + 4, &dataStruct->netID, 4);
	memcpy(data + 12, &dataStruct->characterState, 4);
	memcpy(data + 20, &dataStruct->plantingTree, 4);
	memcpy(data + 24, &dataStruct->x, 4);
	memcpy(data + 28, &dataStruct->y, 4);
	memcpy(data + 32, &dataStruct->XSpeed, 4);
	memcpy(data + 36, &dataStruct->YSpeed, 4);
	memcpy(data + 44, &dataStruct->punchX, 4);
	memcpy(data + 48, &dataStruct->punchY, 4);
	return data;
}

PlayerMoving* unpackPlayerMoving(BYTE* data) {
	PlayerMoving* dataStruct = new PlayerMoving;
	memcpy(&dataStruct->packetType, data, 4);
	memcpy(&dataStruct->netID, data + 4, 4);
	memcpy(&dataStruct->characterState, data + 12, 4);
	memcpy(&dataStruct->plantingTree, data + 20, 4);
	memcpy(&dataStruct->x, data + 24, 4);
	memcpy(&dataStruct->y, data + 28, 4);
	memcpy(&dataStruct->XSpeed, data + 32, 4);
	memcpy(&dataStruct->YSpeed, data + 36, 4);
	memcpy(&dataStruct->punchX, data + 44, 4);
	memcpy(&dataStruct->punchY, data + 48, 4);
	return dataStruct;
}

inline void player_updateData(ENetPeer* peer) {
	if (pInfo(peer)->haveGrowId and pInfo(peer)->rawName != "" and peer->data != NULL and !pInfo(peer)->passed_data) {
		try {
			ifstream r_p("players/_" + pInfo(peer)->rawName + ".json");
			if (!r_p.is_open()) {
				enet_peer_disconnect_later(peer, 0);
				return;
			}
			json j;
			r_p >> j;
			r_p.close();
			string username = j["username"], friends = j["friends"], worldsowned = j["worldsowned"];
			pInfo(peer)->rawName = toLowerText(username);
			pInfo(peer)->displayName = j["displayName"].get<string>();
			pInfo(peer)->displayNameBackup = j["displayNameBackup"].get<string>();
			pInfo(peer)->cloth_back = j["Back"].get<int>(), pInfo(peer)->cloth_hand = j["Hand"].get<int>(), pInfo(peer)->cloth_face = j["Face"].get<int>(), pInfo(peer)->cloth_shirt = j["Shirt"].get<int>(), pInfo(peer)->cloth_pants = j["Pants"].get<int>(), pInfo(peer)->cloth_neck = j["Neck"].get<int>(), pInfo(peer)->cloth_hair = j["Hair"].get<int>(), pInfo(peer)->cloth_feet = j["Feet"].get<int>(), pInfo(peer)->cloth_ances = j["Ances"].get<int>();
			pInfo(peer)->canDoubleJump = j["canDoubleJump"].get<bool>();
			pInfo(peer)->isSusspend = j["isSusspend"].get<bool>();
			pInfo(peer)->isCursed = j["isCursed"].get<bool>();
			pInfo(peer)->isMuted = j["isMuted"].get<bool>();
			pInfo(peer)->isDrTitle = j["isDrTitle"].get<bool>();
			pInfo(peer)->isOfLegend = j["isOfLegend"].get<bool>();
			pInfo(peer)->Supporter = j["Supporter"].get<bool>();
			pInfo(peer)->effect = j["effect"].get<int>();
			pInfo(peer)->adminLevel = j["adminLevel"].get<int>();
			pInfo(peer)->guild = j["guild"].get<string>();
			pInfo(peer)->account_created = j["date"].get<long long int>();
			if (pInfo(peer)->account_created == 0) pInfo(peer)->account_created = time(NULL) / 86400;
			pInfo(peer)->playtime = time(NULL), pInfo(peer)->seconds = j["playtime"].get<long long int>();
			pInfo(peer)->level = j["level"].get<int>();
			pInfo(peer)->xp = j["xp"].get<int>();
			pInfo(peer)->skin = j["skin"].get<int>();
			pInfo(peer)->currentInventorySize = j["inventorySize"].get<int>();
			pInfo(peer)->userID = j["userID"].get<int>();
			pInfo(peer)->lastworld = j["lastworld"].get<string>();
			json inventory = j["inventory"];
			stringstream s_f(friends);
			while (s_f.good()) {
				string str;
				getline(s_f, str, ',');
				if (str.size() == 0) continue;
				pInfo(peer)->friendinfo.push_back(str);
			}
			stringstream s_w(worldsowned);
			while (s_w.good()) {
				string str;
				getline(s_w, str, ',');
				if (str.size() == 0) continue;
				pInfo(peer)->worldsowned.push_back(str);
			}
			PlayerInventory inventory_data; {
				InventoryItem item;
				for (int i = 0; i < pInfo(peer)->currentInventorySize; i++) {
					int itemid = inventory["items"].at(i)["i_"];
					int quantity = inventory["items"].at(i)["q_"];
					if (itemid != 0 && quantity != 0) {
						item.itemCount = quantity;
						item.itemID = itemid;
						inventory_data.items.push_back(item);
					}
				}
			}
			pInfo(peer)->inventory = inventory_data;
			pInfo(peer)->passed_data = true;
			j.clear(); inventory.clear();
		}
		catch (exception& e) {
			cout << e.what() << endl;
			enet_peer_disconnect_later(peer, 0);
		}
	}
}
inline void player_reg(ENetPeer* peer, string user_, string pass, string pass_v, string email) {
	string err_ = "";
	bool err_b = false;
	if (user_.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != string::npos || pass.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@#") != string::npos || pass_v.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@#") != string::npos || email.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@.") != string::npos) {
		err_ += "`4Oops! `wYou can only use letters and numbers in your GrowID Data.", err_b = true;
	} else if (experimental::filesystem::exists("players/_" + toLowerText(user_) + ".json")) {
		err_ += "`4Oops!`` The name `w" + user_ + "`` is so cool someone else has already taken it.  Please choose a different name.", err_b = true;
	} else if (user_.size() < 3 or user_.size() > 18) {
		err_ += "`4Oops! `wYour `wGrowID`` must be between `$3`` and `$18`` characters long.", err_b = true;
	} else if (pass.size() < 8 or pass.size() > 18) {
		err_ += "`4Oops! `wYour password must be between `$8`` and `$18`` characters long.", err_b = true;
	} else if (toLowerText(pass) != toLowerText(pass_v) or pass_v.size() < 8 or pass_v.size() > 18) {
		err_ += "`4Oops! `wPasswords don't match. Try again.", err_b = true;
	} else if (!email_(email)) {
		err_ += "`4Oops! `wLook, if you'd like to be able try retrieve your password if you lose it, you'd better enter a real email.  We promise to keep your data 100% private and never spam you.", err_b = true;
	} if (err_b) {
		Variant::OnDialogRequest(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wCreate an Account``|left|1424|\n\nadd_spacer|small|\nadd_smalltext|"+ err_ +"|\nadd_text_input|username|`cName||30|\nadd_text_input_password|password|`cPassword||100|\nadd_text_input_password|passwordverify|`cPassword Verify||100|\nadd_textbox|Your `wemail address `owill only be used for account verification purposes and won't be spammed or shared. If you use a fake email, you'll never be able to recover or change your password.|\nadd_text_input|email|`cEmail||100|\nadd_textbox|Your `wDiscord ID `owill be used for secondary verification if you lost access to your `wemail address`o! Please enter in such format: `wdiscordname#tag`o. Your `wDiscord Tag `ocan be found in your `wDiscord account settings`o.|\nadd_text_input|discord|`cDiscord||100|\nadd_spacer|small|\nadd_button|creategrowid|`2Get an Account!|\n");
		return;
	} else {
		if (experimental::filesystem::exists("players/ipreg/_" + pInfo(peer)->ip_ply + ".txt")) {
			auto r_ipc = 0;
			ifstream g_p("players/ipreg/_" + pInfo(peer)->ip_ply + ".txt");
			g_p >> r_ipc; r_ipc += 1; g_p.close();
			if (r_ipc >= 3) {
				err_ += "`4Oops! `wYou have too much accounts created.", err_b = true;
				Variant::OnDialogRequest(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wCreate an Account``|left|1424|\n\nadd_spacer|small|\nadd_smalltext|" + err_ + "|\nadd_text_input|username|`cName||30|\nadd_text_input_password|password|`cPassword||100|\nadd_text_input_password|passwordverify|`cPassword Verify||100|\nadd_textbox|Your `wemail address `owill only be used for account verification purposes and won't be spammed or shared. If you use a fake email, you'll never be able to recover or change your password.|\nadd_text_input|email|`cEmail||100|\nadd_textbox|Your `wDiscord ID `owill be used for secondary verification if you lost access to your `wemail address`o! Please enter in such format: `wdiscordname#tag`o. Your `wDiscord Tag `ocan be found in your `wDiscord account settings`o.|\nadd_text_input|discord|`cDiscord||100|\nadd_spacer|small|\nadd_button|creategrowid|`2Get an Account!|\n");
				return;
			}
			ofstream w_p("players/ipreg/_" + pInfo(peer)->ip_ply + ".txt");
			w_p << r_ipc; w_p.close();
		} else {
			ofstream w_p("players/ipreg/_" + pInfo(peer)->ip_ply + ".txt");
			w_p << 1; w_p.close();
		}
		if (!experimental::filesystem::exists("gemdb/_" + toLowerText(user_) + ".txt")) {
			ofstream gem("gemdb/_" + toLowerText(user_) + ".txt");
			gem << 10000;
			gem.close();
		}
		totaluserids++;
		if (totaluserids == 1) totaluserids++;
		try {
			ifstream p_data("players/_" + toLowerText(user_) + ".json");
			if (p_data.is_open()) {
				p_data.close();
				enet_peer_disconnect_later(peer, 0);
				return;
			}
			json items, j_, j_l = json::array();
			j_["a_"] = 1, j_["i_"] = 18, j_["q_"] = 1;
			j_l.push_back(j_);
			j_["a_"] = 2, j_["i_"] = 32, j_["q_"] = 1;
			j_l.push_back(j_);
			j_["a_"] = 3, j_["i_"] = 98, j_["q_"] = 1;
			j_l.push_back(j_);
			j_["a_"] = 4, j_["i_"] = 6336, j_["q_"] = 1;
			j_l.push_back(j_);
			j_["a_"] = 5, j_["i_"] = 4584, j_["q_"] = 200;
			j_l.push_back(j_);
			for (int i = 5; i < 200; i++) {
				j_["a_"] = i + 1, j_["i_"] = 0, j_["q_"] = 0;
				j_l.push_back(j_);
			}
			items["items"] = j_l;
			ofstream w_ply("players/_" + toLowerText(user_) + ".json");
			if (!w_ply.is_open()) {
				enet_peer_disconnect_later(peer, 0);
				return;
			}
			time_t a_;
			a_ = time(NULL);
			int days_ = int(a_) / (60 * 60 * 24);
			json j;
			j["username"] = toLowerText(user_);
			j["password"] = toLowerText(pass);
			j["displayName"] = user_;
			j["displayNameBackup"] = user_;
			j["email"] = email;
			j["Back"] = 0;
			j["Hand"] = 0;
			j["Face"] = 0;
			j["Shirt"] = 0;
			j["Pants"] = 0;
			j["Neck"] = 0;
			j["Hair"] = 0;
			j["Feet"] = 0;
			j["Mask"] = 0;
			j["Ances"] = 0;
			j["canDoubleJump"] = false;
			j["isSusspend"] = false;
			j["isCursed"] = false;
			j["isMuted"] = false;
			j["isDrTitle"] = false;
			j["Supporter"] = false;
			j["isOfLegend"] = false;
			j["effect"] = 0x808000;
			j["adminLevel"] = 0;
			j["guild"] = "";
			j["date"] = days_;
			j["playtime"] = 0;
			j["level"] = 1;
			j["xp"] = 0;
			j["skin"] = 0x8295C3FF;
			j["inventorySize"] = 16;
			j["friends"] = "";
			j["worldsowned"] = "";
			j["mac"] = pInfo(peer)->mac_ply;
			j["ip"] = pInfo(peer)->ip_ply;
			j["userID"] = totaluserids;
			j["lastworld"] = "";
			j["ipID"] = to_string(peer->address.host);
			j["inventory"] = items;
			w_ply << j << std::endl;
			w_ply.close(); 
			j.clear(); items.clear(); j_.clear(); j_l.clear();
		}
		catch (exception& e) {
			cout << e.what() << std::endl;
			enet_peer_disconnect_later(peer, 0);
		}
		ofstream f_uid;
		f_uid.open("uids.txt");
		f_uid << to_string(totaluserids);
		f_uid.close();

		pInfo(peer)->lobby_guest_acc = false;
		gamepacket_t p;
		p.Insert("SetHasGrowID");
		p.Insert(1);
		p.Insert(user_);
		p.Insert(pass);
		p.CreatePacket(peer);
		Variant::OnConsoleMessage(peer, "`cYou will be disconnected for a while after creating an account, come back again!");
		send_packet_(peer, "action|play_sfx\nfile|audio/piano_nice.wav\ndelayMS|0\n");
		enet_peer_disconnect_later(peer, 0);
		return;
	}
}
inline void player_login(ENetPeer* peer, string cch) {
	vector<string> a_pkt = explode("|", replace_str(cch, "\n", "|"));
	for (int i_ = 0; i_ < (int)a_pkt.size(); i_++) {
		if (a_pkt.at(i_) == "tankIDName") {
			pInfo(peer)->tankIDName = a_pkt.at(i_ + 1);
			ifstream ifs("players/_" + toLowerText(pInfo(peer)->tankIDName) + ".json");
			if (ifs.is_open()) {
				pInfo(peer)->haveGrowId = true;
				pInfo(peer)->rawName = toLowerText(pInfo(peer)->tankIDName);
			} else {
				send_packet_(peer, "`4Unable to log on: `oThat `wGrowID`` doesn't seem valid, or the password is wrong. If you don't have one, press `wCancel``, un-check `w'I have a GrowID'``, then click `wConnect``.", "Retrieve lost password");
				send_packet_(peer, "action|logon_fail");
				pInfo(peer)->block_login = true;
			}
			ifs.close();
		} else if (a_pkt.at(i_) == "tankIDPass") {
			pInfo(peer)->tankIDPass = a_pkt.at(i_ + 1);
		} else if (a_pkt.at(i_) == "requestedName") {
			pInfo(peer)->requestedName = a_pkt.at(i_ + 1);
		} else if (a_pkt.at(i_) == "country") {
			pInfo(peer)->country = a_pkt.at(i_ + 1);
			if (pInfo(peer)->country.length() > 4) {
				send_packet_(peer, "action|log\nmsg|`4Unable to log on: `oSomething wrong about your account!");
				send_packet_(peer, "action|logon_fail");
			}
		} else if (a_pkt.at(i_) == "mac") {
			pInfo(peer)->mac_ply = a_pkt.at(i_ + 1);
		} else if (a_pkt.at(i_) == "game_version") {
			pInfo(peer)->game_Version = a_pkt.at(i_ + 1);
		} else if (a_pkt.at(i_) == "meta") {
			pInfo(peer)->meta = a_pkt.at(i_ + 1);
		} else if (a_pkt.at(i_) == "platformID") {
			pInfo(peer)->platformID = a_pkt.at(i_ + 1);
			if (a_pkt.at(i_ + 1) == "2") {
				send_packet_(peer, "action|log\nmsg|`4Linux Device is Unsupported This Server!");
				send_packet_(peer, "action|logon_fail");
			}
		}
	}
	if (pInfo(peer)->haveGrowId) {
		try {
			ifstream ply("players/_" + toLowerText(pInfo(peer)->tankIDName) + ".json");
			json j;
			ply >> j;
			ply.close();
			string p_json = j["password"], p_req = toLowerText(pInfo(peer)->tankIDPass);
			bool is_spnd = j["isSusspend"], exist = experimental::filesystem::exists("playerbanned/ip/" + to_string(peer->address.host) + ".txt"), exist_2 = experimental::filesystem::exists("playerbanned/account/" + pInfo(peer)->rawName + ".txt");
			if (p_json != p_req) {
				send_packet_(peer, "`4Unable to log on: `oThat `wGrowID`` doesn't seem valid, or the password is wrong. If you don't have one, press `wCancel``, un-check `w'I have a GrowID'``, then click `wConnect``.", "Retrieve lost password");
				send_packet_(peer, "action|logon_fail");
				pInfo(peer)->block_login = true;
			} else if (exist) {
				string c_t = "0";
				if (experimental::filesystem::exists("playerbanned/ip/" + to_string(peer->address.host) + ".txt")) {
					ifstream i_f("playerbanned/ip/" + to_string(peer->address.host) + ".txt");
					string c_f((istreambuf_iterator<char>(i_f)), (istreambuf_iterator<char>()));
					c_t = c_f;
					i_f.close();
				}
				long long ban_d = atoi(c_t.c_str());
				int b_tleft = calcBanDuration(ban_d);
				if (b_tleft > 0) {
					send_packet_(peer, "action|log\nmsg|`4Sorry, this account, device or location has been temporarily suspended.");
					send_packet_(peer, "action|log\nmsg|`oif you didn't do anything wrong, it could be because you're playing from the same place or the same device as someone who did.");
					send_packet_(peer, "action|log\nmsg|`oThis is a temporary ban caused by `w" + pInfo(peer)->tankIDName + " `oand will be removed in `w" + OutputBanTime(calcBanDuration(ban_d)) + "`o. if that's not your name, try playing from another location or device to fix it!");
					send_packet_(peer, "action|logon_fail");
					pInfo(peer)->block_login = true;
				}
			} else if (exist_2) {
				string c_t = "0";
				if (experimental::filesystem::exists("playerbanned/account/" + pInfo(peer)->rawName + ".txt")) {
					ifstream i_f("playerbanned/account/" + pInfo(peer)->rawName + ".txt");
					string c_f((istreambuf_iterator<char>(i_f)), (istreambuf_iterator<char>()));
					c_t = c_f;
					i_f.close();
				}
				long long ban_d = atoi(c_t.c_str());
				int b_tleft = calcBanDuration(ban_d);
				if (b_tleft > 0) {
					send_packet_(peer, "action|log\nmsg|`4Sorry, this account is currently banned. You will have to wait `w" + OutputBanTime(calcBanDuration(ban_d)) + " `4for this ban to expire so you can use this account again.");
					send_packet_(peer, "action|logon_fail");
					pInfo(peer)->block_login = true;
				}
			} else if (is_spnd) {
				send_packet_(peer, "`4Sorry, this account [`5" + pInfo(peer)->tankIDName + "`4] has been suspended. Contact `#Panji#2249 `4in Discord if you have any questions.", "Contact Server");
				send_packet_(peer, "action|logon_fail");
				pInfo(peer)->block_login = true;
			} else if (pInfo(peer)->game_Version != SERVER_GAME_VERSION) {
				send_packet_(peer, "`4UPDATE REQUIRED`o : The `$V" + SERVER_GAME_VERSION + " `oupdate is now avallable for your device. Go get it! You'll need that before you can login into private server.", "Download Latest Version", "https://www.growtopiagame.com/");
				send_packet_(peer, "action|logon_fail");
				pInfo(peer)->block_login = true;
			} else {
				pInfo(peer)->inGame = true;
				for (ENetPeer* net_peer = server->peers; net_peer < &server->peers[server->peerCount]; ++net_peer) {
					if (net_peer->state != ENET_PEER_STATE_CONNECTED || net_peer == peer) continue;
					string upper_peer_name = pInfo(net_peer)->rawName;
					if (upper_peer_name == toLowerText(pInfo(peer)->tankIDPass) && pInfo(net_peer) != pInfo(peer)) {
						Variant::OnConsoleMessage(peer, "`4ALREADY ON?! `o: This account was already online, kicking it off so you can log on. (if you were just playing before, this is nothing to worry about)");
						enet_peer_disconnect_later(net_peer, 0);
						break;
					}
				}
				player_updateData(peer);
			}
			j.clear();
		}
		catch (exception exx) {
			cout << exx.what() << endl;
			enet_peer_disconnect_later(peer, 0);
		}
	} else {
		pInfo(peer)->inGame = true;
	}
	Variant::OnSuperMainStartAcceptLogon(peer, itemdathash, "ubistatic-a.akamaihd.net", SERVER_CDN, "cc.cz.madkite.freedom org.aqua.gg idv.aqua.bulldog com.cih.gamecih2 com.cih.gamecih com.cih.game_cih cn.maocai.gamekiller com.gmd.speedtime org.dax.attack com.x0.strai.frep com.x0.strai.free org.cheatengine.cegui org.sbtools.gamehack com.skgames.traffikrider org.sbtoods.gamehaca com.skype.ralder org.cheatengine.cegui.xx.multi1458919170111 com.prohiro.macro me.autotouch.autotouch com.cygery.repetitouch.free com.cygery.repetitouch.pro com.proziro.zacro com.slash.gamebuster", "proto=160|choosemusic=audio/mp3/about_theme.mp3|active_holiday=0|wing_week_day=0|ubi_week_day=0|server_tick=33190254|clash_active=0|drop_lavacheck_faster=1|isPayingUser=0|usingStoreNavigation=1|enableInventoryTab=1|bigBackpack=1|", 2357275721);
}
inline void SendInventory(ENetPeer* peer, PlayerInventory inventory) {
	if (pInfo(peer)->currentWorld == "EXIT" or pInfo(peer)->currentWorld.empty()) return;
	try {
		const int inventoryLen = inventory.items.size();
		const auto packetLen = 66 + (inventoryLen * 4) + 4;
		auto* data2 = new BYTE[packetLen];
		auto MessageType = 0x4;
		auto PacketType = 0x9;
		auto NetID = -1;
		auto CharState = 0x8;
		memset(data2, 0, packetLen);
		memcpy(data2, &MessageType, 4);
		memcpy(data2 + 4, &PacketType, 4);
		memcpy(data2 + 8, &NetID, 4);
		memcpy(data2 + 16, &CharState, 4);
		int endianInvVal = _byteswap_ulong(inventoryLen);
		memcpy(data2 + 66 - 4, &endianInvVal, 4);
		if (pInfo(peer)->haveGrowId) endianInvVal = _byteswap_ulong(pInfo(peer)->currentInventorySize - 1);
		else endianInvVal = _byteswap_ulong(2);
		memcpy(data2 + 66 - 8, &endianInvVal, 4);
		int val = 0;
		for (int i = 0; i < inventoryLen; i++) {
			val = 0;
			val |= inventory.items[i].itemID;
			val |= inventory.items[i].itemCount << 16;
			val &= 0x00FFFFFF;
			val |= 0x00 << 24;
			memcpy(data2 + (i * 4) + 66, &val, 4);
		}
		const auto packet3 = enet_packet_create(data2, packetLen, ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(peer, 0, packet3);
		delete[] data2;
	}
	catch (const out_of_range& e) {
		cout << e.what() << endl;
	}
}
inline bool isHere(ENetPeer* peer, ENetPeer* peer2) {
	return pInfo(peer)->currentWorld == pInfo(peer2)->currentWorld;
}
inline bool isDev(ENetPeer* peer) {
	if (pInfo(peer)->adminLevel == 10) return true;
	else return false;
}
inline bool isMod(ENetPeer* peer) {
	if (pInfo(peer)->adminLevel >= 8) return true;
	else return false;
}
inline bool isVip(ENetPeer* peer) {
	if (pInfo(peer)->adminLevel == 2) return true;
	else return false;
}
inline int c_playerWorld(const string name) {
	int c_ = 0;
	for (ENetPeer* c_p = server->peers; c_p < &server->peers[server->peerCount]; ++c_p) {
		if (c_p->state != ENET_PEER_STATE_CONNECTED) continue;
		if (pInfo(c_p)->currentWorld == name) {
			c_++;
		}
	}
	return c_;
}
inline void sendPData(ENetPeer* peer, PlayerMoving* data) {
	for (ENetPeer* c_p = server->peers; c_p < &server->peers[server->peerCount]; ++c_p) {
		if (c_p->state != ENET_PEER_STATE_CONNECTED) continue;
		if (peer != c_p) {
			if (isHere(peer, c_p)) {
				data->netID = pInfo(peer)->netID;
				SendPacketRaw(4, packPlayerMoving(data), 56, 0, c_p, ENET_PACKET_FLAG_RELIABLE);
			}
		}
	}
}
inline void update_all_clothes(ENetPeer* peer) {
	for (ENetPeer* c_p = server->peers; c_p < &server->peers[server->peerCount]; ++c_p) {
		if (c_p->state != ENET_PEER_STATE_CONNECTED) continue;
		if (isHere(peer, c_p)) {
			{
				gamepacket_t p(0, pInfo(peer)->netID);
				p.Insert("OnSetClothing");
				p.Insert((float)pInfo(peer)->cloth_hair, (float)pInfo(peer)->cloth_shirt, (float)pInfo(peer)->cloth_pants);
				p.Insert((float)pInfo(peer)->cloth_feet, (float)pInfo(peer)->cloth_face, (float)pInfo(peer)->cloth_hand);
				p.Insert((float)pInfo(peer)->cloth_back, (float)pInfo(peer)->cloth_mask, (float)pInfo(peer)->cloth_neck);
				p.Insert(pInfo(peer)->skin);
				p.CreatePacket(c_p);
			}
			{
				gamepacket_t p(0, pInfo(c_p)->netID);
				p.Insert("OnSetClothing");
				p.Insert((float)pInfo(c_p)->cloth_hair, (float)pInfo(c_p)->cloth_shirt, (float)pInfo(c_p)->cloth_pants);
				p.Insert((float)pInfo(c_p)->cloth_feet, (float)pInfo(c_p)->cloth_face, (float)pInfo(c_p)->cloth_hand);
				p.Insert((float)pInfo(c_p)->cloth_back, (float)pInfo(c_p)->cloth_mask, (float)pInfo(c_p)->cloth_neck);
				p.Insert(pInfo(c_p)->skin);
				p.CreatePacket(peer);
			}
		}
	}
}