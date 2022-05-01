#pragma once
#include "enet/enet.h"
#include <Windows.h>
#include <vector>
#include <chrono>
#include <experimental/filesystem>

class Variant {
	public:
		static void OnConsoleMessage(ENetPeer* peer, string text);
		static void OnSuperMainStartAcceptLogon(ENetPeer* peer, int item_dash, string url_path, string cdn_path, string web_path, string proto_info, unsigned int tribute);
		static void OnDialogRequest(ENetPeer* peer, string text);
		static void OnSetHasGrowID(ENetPeer* peer, int c_t, string g_id, string p_id);
		static void OnEmoticonDataChanged(ENetPeer* peer, int c_t, string text);
		static void OnSetBux(ENetPeer* peer, int c_t, int acc_state);
		static void OnAddNotification(ENetPeer* peer, string txt, string audio_, string _image);
		static void OnTalkBubble(ENetPeer* peer, int netID, string txt, int chat_Color, bool overlay);
		static void OnSpawn(ENetPeer* peer, string txt);
		static void OnSetPos(ENetPeer* peer, int netID, float x, float y);
		static void OnRemove(ENetPeer* peer, int netID);
		static void PlayAudio(ENetPeer* peer, string file_, int delay);
};

inline void Variant::OnConsoleMessage(ENetPeer* peer, string text) {
	gamepacket_t p;
	p.Insert("OnConsoleMessage");
	p.Insert(text);
	p.CreatePacket(peer);
}
inline void Variant::OnSuperMainStartAcceptLogon(ENetPeer* peer, int item_dash, string url_path, string cdn_path, string web_path, string proto_info, unsigned int tribute) {
	gamepacket_t p;
	p.Insert("OnSuperMainStartAcceptLogonHrdxs47254722215a");
	p.Insert(item_dash);
	p.Insert(url_path);
	p.Insert(cdn_path);
	p.Insert(web_path);
	p.Insert(proto_info);
	p.Insert(tribute);
	p.CreatePacket(peer);
}
inline void Variant::OnDialogRequest(ENetPeer* peer, string text) {
	gamepacket_t p(400);
	p.Insert("OnDialogRequest");
	p.Insert(text);
	p.CreatePacket(peer);
}
inline void Variant::OnSetHasGrowID(ENetPeer* peer, int c_t, string g_id, string p_id) {
	gamepacket_t p;
	p.Insert("SetHasGrowID");
	p.Insert(c_t);
	p.Insert(g_id);
	p.Insert(p_id);
	p.CreatePacket(peer);
}
inline void Variant::OnEmoticonDataChanged(ENetPeer* peer, int c_t, string text) {
	gamepacket_t p;
	p.Insert("OnEmoticonDataChanged");
	p.Insert(c_t);
	p.Insert(text);
	p.CreatePacket(peer);
}
inline void Variant::OnSetBux(ENetPeer* peer, int c_t, int acc_state) {
	gamepacket_t p;
	p.Insert("OnSetBux");
	p.Insert(c_t);
	p.Insert(acc_state);
	p.CreatePacket(peer);
}
inline void Variant::OnAddNotification(ENetPeer* peer, string txt, string audio_, string _image) {
	gamepacket_t p;
	p.Insert("OnAddNotification");
	p.Insert(_image);
	p.Insert(txt);
	p.Insert(audio_);
	p.CreatePacket(peer);
}
inline void Variant::OnTalkBubble(ENetPeer* peer, int netID, string txt, int chat_Color, bool overlay) {
	if (overlay) {
		gamepacket_t p(0, netID);
		p.Insert("OnTalkBubble");
		p.Insert(txt);
		p.Insert(chat_Color);
		p.Insert(1);
		p.CreatePacket(peer);
	} else {
		gamepacket_t p(0, netID);
		p.Insert("OnTalkBubble");
		p.Insert(txt);
		p.Insert(chat_Color);
		p.CreatePacket(peer);
	}
}
inline void Variant::OnSpawn(ENetPeer* peer, string txt) {
	gamepacket_t p;
	p.Insert("OnSpawn");
	p.Insert(txt);
	p.CreatePacket(peer);
}
inline void Variant::OnSetPos(ENetPeer* peer, int netID, float x, float y) {
	gamepacket_t p(0, netID);
	p.Insert("OnSetPos");
	p.Insert(x);
	p.Insert(y);
	p.CreatePacket(peer);
}
inline void Variant::OnRemove(ENetPeer* peer, int netID) {
	gamepacket_t p;
	p.Insert("OnRemove");
	p.Insert("netID|" + to_string(netID) + "");
	p.CreatePacket(peer);
}
inline void Variant::PlayAudio(ENetPeer* peer, string file_, int delay) {
	string w_ = "action|play_sfx\nfile|"+ file_ +"\ndelayMS|"+to_string(delay)+"\n";
	int y_ = 3;
	BYTE z_ = 0;
	BYTE* const d_ = new BYTE[5 + w_.length()];
	memcpy(d_, &y_, 4);
	memcpy(d_ + 4, w_.c_str(), w_.length());
	memcpy(d_ + 4 + w_.length(), &z_, 1);
	ENetPacket* const p = enet_packet_create(d_, 5 + w_.length(), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, p);
	delete[]d_;
}