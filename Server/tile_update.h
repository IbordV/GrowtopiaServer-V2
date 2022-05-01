#pragma once
#include "enet/enet.h"
#include <Windows.h>
#include <vector>
#include <chrono>
#include <experimental/filesystem>
using namespace chrono;

inline void return_packet(ENetPeer* peer, int x, int y) {
	PlayerMoving d_;
	d_.netID = pInfo(peer)->netID;
	d_.packetType = 0x8, d_.plantingTree = 0, d_.netID = -1;
	d_.x = (float)x, d_.punchX = x;
	d_.y = (float)y, d_.punchY = y;
	SendPacketRaw(4, packPlayerMoving(&d_), 56, 0, peer, ENET_PACKET_FLAG_RELIABLE);
}

inline void tile_update(int x, int y, int tile, int c_b, World* world, ENetPeer* peer) {
	PlayerMoving d_;
	d_.packetType = 0x3;
	d_.characterState = 0x0;
	d_.x = (float)x, d_.punchX = x, d_.XSpeed = 0;
	d_.y = (float)y, d_.punchY = y, d_.XSpeed = 0;
	d_.netID = c_b;
	d_.plantingTree = tile;
	return_packet(peer, x, y);

	ItemDefinition d_e_f;
	try {
		d_e_f = getItemDef(world->items[x + (y * world->width)].foreground);
		if (d_e_f.blockType == BlockTypes::CLOTHING) return;
		if (getItemDef(tile).blockType == BlockTypes::CLOTHING) return;
	}
	catch (...) {
		d_e_f.breakHits = 4;
		d_e_f.blockType = BlockTypes::UNKNOWN;
	}
	if (tile == 18) {
		int breakhits_ = 4;
		int t_ = world->items[x + (y * world->width)].foreground;
		int b_ = world->items[x + (y * world->width)].background;
		if (b_ == 0 and t_ == 0) return;
		d_.packetType = 0x8;
		d_.plantingTree = 5;
		if ((duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count() - world->items[x + (y * world->width)].breakTime >= 4000 and pInfo(peer)->cloth_hand != 2952) {
			world->items[x + (y * world->width)].breakTime = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
			world->items[x + (y * world->width)].breakLevel = 5;
		} else {
			if (y < world->height and world->items[x + (y * world->width)].breakLevel + 4 >= d_e_f.breakHits * breakhits_) {
				d_.packetType = 0x3;
				d_.netID = -1;
				d_.plantingTree = tile;
				d_.punchX = x;
				d_.punchY = y;
				world->items[x + (y * world->width)].breakLevel = 0;
				if (world->items[x + (y * world->width)].foreground != 0) {
					world->items[x + (y * world->width)].foreground = 0;
				}
				else {
					world->items[x + (y * world->width)].background = 0;
				}
			}
			else {
				if (y < world->height) {
					world->items[x + (y * world->width)].breakTime = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
					world->items[x + (y * world->width)].breakLevel += 4;
				}
			}
		}
	} else {
		for (int i = 0; i < (int)((PlayerInfo*)(peer->data))->inventory.items.size(); i++) {
			if (((PlayerInfo*)(peer->data))->inventory.items.at(i).itemID == tile) {
				if ((unsigned int)((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount > 1) {
					((PlayerInfo*)(peer->data))->inventory.items.at(i).itemCount--;
				} else {
					((PlayerInfo*)(peer->data))->inventory.items.erase(((PlayerInfo*)(peer->data))->inventory.items.begin() + i);
				}
			}
		}
		if (getItemDef(tile).blockType == BlockTypes::BACKGROUND) {
			cout << getItemDef(world->items[x + (y * world->width)].background).name << endl;
		} else if (getItemDef(tile).blockType == BlockTypes::SEED) {
			cout << getItemDef(world->items[x + (y * world->width)].foreground).name << endl;
		} else {
			cout << getItemDef(world->items[x + (y * world->width)].foreground).name << endl;
		}
		world->items[x + (y * world->width)].breakLevel = 0;
	}
	for (ENetPeer* c_p = server->peers; c_p < &server->peers[server->peerCount]; ++c_p) {
		if (c_p->state != ENET_PEER_STATE_CONNECTED) continue;
		if (isHere(peer, c_p)) {
			SendPacketRaw(4, packPlayerMoving(&d_), 56, 0, c_p, ENET_PACKET_FLAG_RELIABLE);
		}
	}
}