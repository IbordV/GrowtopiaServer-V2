#pragma once
#include "enet/enet.h"
#include <Windows.h>
#include <vector>
#include <chrono>
#include <string>
#include <experimental/filesystem>
#include "game_packet.h"
#include "player_broadcast.h"
using json = nlohmann::json;
using namespace std;

ENetHost* server;
string SERVER_PORT = "17091", SERVER_IP = "127.0.0.1", SERVER_CDN = "0098/63368/cache/", SERVER_GAME_VERSION = "3.86";
bool server_on_freeze = false, maintenance = false;
int itemdathash = 0, itemsDatSize = 0, totaluserids = 0, cId = 1, maxItems = 0;
BYTE* itemsDat = 0;
typedef __int16 __int16_t;

inline void Server_alert(string cmd) {
	cout << "[INFO] " + cmd + "" << endl;
}
vector<string> explode(const string& delimiter, const string& str) {
	vector<string> arr;
	int strleng = str.length();
	int delleng = delimiter.length();
	if (delleng == 0) return arr;
	int i = 0;
	int k = 0;
	while (i < strleng) {
		int j = 0;
		while (i + j < strleng && j < delleng && str[i + j] == delimiter[j]) j++;
		if (j == delleng) {
			arr.push_back(str.substr(k, i - k));
			i += delleng;
			k = i;
		} else {
			i++;
		}
	}
	arr.push_back(str.substr(k, i - k));
	return arr;
}
unsigned char* getA(string fileName, int* pSizeOut, bool bAddBasePath, bool bAutoDecompress) {
	unsigned char* pData = NULL;
	FILE* fp = fopen(fileName.c_str(), "rb");
	if (not fp) {
		Server_alert("File not found");
		if (!fp) return NULL;
	}
	fseek(fp, 0, SEEK_END);
	*pSizeOut = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	pData = (unsigned char*)new unsigned char[((*pSizeOut) + 1)];
	if (not pData) {
		printf("Out of memory opening %s?", fileName.c_str());
		return 0;
	}
	pData[*pSizeOut] = 0;
	fread(pData, *pSizeOut, 1, fp);
	fclose(fp);
	return pData;
}
bool email_(const string& str) {
	if (str.size() > 64 or str.empty())
		return false;
	const string::const_iterator at = std::find(str.cbegin(), str.cend(), '@'), dot = std::find(at, str.cend(), '.');
	if ((at == str.cend()) or (dot == str.cend()))
		return false;
	if (std::distance(str.cbegin(), at) < 1 or std::distance(at, str.cend()) < 5)
		return false;
	return true;
}
ifstream::pos_type filesize(const char* filename) {
	ifstream in(filename, ifstream::ate | ifstream::binary);
	return in.tellg();
}
uint32_t HashString(unsigned char* str, int len) {
	if (not str) return 0;
	unsigned char* n = (unsigned char*)str;
	uint32_t acc = 0x55555555;
	if (len == 0) {
		while (*n) acc = (acc >> 27) + (acc << 5) + *n++;
	} else {
		for (int i = 0; i < len; i++) {
			acc = (acc >> 27) + (acc << 5) + *n++;
		}
	}
	return acc;
}
string toUpperText(string txt) {
	string ret;
	for (char c : txt) ret += toupper(c);
	return ret;
}
string toLowerText(string name) {
	string newS;
	for (char c : name) newS += (c >= 'A' && c <= 'Z') ? c - ('A' - 'a') : c;
	string ret;
	for (int i = 0; i < (int)newS.length(); i++)
	{
		if (newS[i] == '`') i++; else ret += newS[i];
	}
	string ret2;
	for (char c : ret) if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) ret2 += c;
	return ret2;
}
string replace_str(string& str, const string& from, const string& to) {
	while (str.find(from) != string::npos)
		str.replace(str.find(from), from.length(), to);
	return str;
}
long long GetCurrentTimeInternalSeconds() {
	using namespace std::chrono;
	return (duration_cast<seconds>(system_clock::now().time_since_epoch())).count();
}
string OutputBanTime(int n) {
	string x;
	int day = n / (24 * 3600);
	if (day != 0) x.append(to_string(day) + " Days, ");
	n = n % (24 * 3600);
	int hour = n / 3600;
	if (hour != 0) x.append(to_string(hour) + " Hours, ");
	n %= 3600;
	int minutes = n / 60;
	if (minutes != 0) x.append(to_string(minutes) + " Minutes, ");
	n %= 60;
	int seconds = n;
	if (seconds != 0) x.append(to_string(seconds) + " Seconds");
	return x;
}
inline int calcBanDuration(long long banDuration) {
	int duration = 0;
	duration = (int)(banDuration - GetCurrentTimeInternalSeconds());
	return duration;
}
inline void send_p(ENetPeer* peer, int num, char* data, const int len) {
	const auto packet = enet_packet_create(nullptr, len + 5, ENET_PACKET_FLAG_RELIABLE);
	memcpy(packet->data, &num, 4);
	if (data != nullptr) {
		memcpy(packet->data + 2, data, len);
	}
	char zero = 0;
	memcpy(packet->data + 2 + len, &zero, 1);
	enet_peer_send(peer, 0, packet);
}
inline void send_packet_(ENetPeer* p_, string t_, string l_ = "", string l_p = "") {
	if (l_ != "")
		t_ = "action|log\nmsg|" + t_;
	int y_ = 3;
	BYTE z_ = 0;
	BYTE* const d_ = new BYTE[5 + t_.length()];
	memcpy(d_, &y_, 4);
	memcpy(d_ + 4, t_.c_str(), t_.length());
	memcpy(d_ + 4 + t_.length(), &z_, 1);
	ENetPacket* const p = enet_packet_create(d_, 5 + t_.length(), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(p_, 0, p);
	delete[]d_;
	if (l_ != "") {
		string w_ = "https://youtube.com/";
		if (l_p != "") {
			w_ = l_p;
		}
		l_ = "action|set_url\nurl|" + w_ + "\nlabel|" + l_ + "\n";
		BYTE* const u_ = new BYTE[5 + l_.length()];
		memcpy(u_, &y_, 4);
		memcpy(u_ + 4, l_.c_str(), l_.length());
		memcpy(u_ + 4 + l_.length(), &z_, 1);
		ENetPacket* const p3 = enet_packet_create(u_, 5 + l_.length(), ENET_PACKET_FLAG_RELIABLE);
		enet_peer_send(p_, 0, p3);
		delete[]u_;
	}
}
inline int get_player_c() {
	int count = 0;
	for (ENetPeer* c_ = server->peers; c_ < &server->peers[server->peerCount]; ++c_) {
		if (c_->state != ENET_PEER_STATE_CONNECTED) continue;
		count++;
	}
	return count;
}
BYTE* struct_ptr(ENetPacket* packet) {
	unsigned int packetLenght = packet->dataLength;
	BYTE* result = NULL;
	if (packetLenght >= 0x3C) {
		BYTE* packetData = packet->data;
		result = packetData + 4;
		if (*(BYTE*)(packetData + 16) & 8) {
			if (packetLenght < *(unsigned int*)(packetData + 56) + 60) {
				Server_alert("Packet too small for extended packet to be valid");
				Server_alert("Sizeof float is 4.  TankUpdatePacket size: 56");
				result = 0;
			}
		}
		else {
			int zero = 0;
			memcpy(packetData + 56, &zero, 4);
		}
	}
	return result;
}
inline int message_ptr(ENetPacket* packet) {
	if (packet->dataLength > 3u) {
		return *(packet->data);
	}
	return 0;
}
char* text_ptr(ENetPacket* packet) {
	char zero = 0;
	memcpy(packet->data + packet->dataLength - 1, &zero, 1);
	return (char*)(packet->data + 4);
}
int ch2n(char x) {
	switch (x) {
	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'A':
		return 10;
	case 'B':
		return 11;
	case 'C':
		return 12;
	case 'D':
		return 13;
	case 'E':
		return 14;
	case 'F':
		return 15;
	default:
		break;
	}
	return -1;
}
void SendPacketRaw(int a1, void* packetData, const size_t packetDataSize, void* a4, ENetPeer* peer, const int packetFlag) {
	ENetPacket* p;
	if (peer) {
		if (a1 == 4 && *(static_cast<BYTE*>(packetData) + 12) & 8) {
			p = enet_packet_create(nullptr, packetDataSize + *(static_cast<DWORD*>(packetData) + 13) + 5, packetFlag);
			auto four = 4;
			memcpy(p->data, &four, 4);
			memcpy(reinterpret_cast<char*>(p->data) + 4, packetData, packetDataSize);
			memcpy(reinterpret_cast<char*>(p->data) + packetDataSize + 4, a4, *(static_cast<DWORD*>(packetData) + 13));
			enet_peer_send(peer, 0, p);
		} else {
			try {
				p = enet_packet_create(nullptr, packetDataSize + 5, packetFlag);
				memcpy(p->data, &a1, 4);
				memcpy(reinterpret_cast<char*>(p->data) + 4, packetData, packetDataSize);
				enet_peer_send(peer, 0, p);
			}
			catch (...) {
				cout << "Failed to send packet raw (sendpacketraw)" << endl;
			}
		}
	}
	delete (char*)(packetData);
}