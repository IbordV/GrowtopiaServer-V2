#define _CRT_SECURE_NO_DEPRECATE
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATIBON_WARNING
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include <iostream>
#include "include/httplib.h"
#include "enet/enet.h"
#include "json.hpp"
#include <string>
#include <vector>

#define pInfo(peer) ((PlayerInfo*)(peer->data))
using namespace std;
#include "back_end.h"
#include "world_base.h"
#include "server_base.h"
#include "player_base.h"
#include "dialog_handler.h"
#include "tile_update.h"

int main() { //(c) Panji#2249 | GrowtopiaNoobs
	system("Color D");
	Server_alert("Server made by: Panji#2249");
	server_base_load();
	build_item_base();

	enet_initialize();
	ENetAddress address;
	enet_address_set_host(&address, "0.0.0.0");
	address.port = atoi(SERVER_PORT.c_str());
	server = enet_host_create(&address, 1024, 2, 0, 0);
	if (server == NULL) {
		Server_alert("An error occurred while trying to create an ENet server host.\n");
		while (1);
		exit(EXIT_FAILURE);
	}
	ifstream t("uids.txt");
	string str_uid((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
	totaluserids = atoi(str_uid.c_str());
	server->checksum = enet_crc32;
	enet_host_compress_with_range_coder(server);
	srand(unsigned int(time(NULL)));

	ENetEvent event;
	while (true) {
		while (enet_host_service(server, &event, 1000) > 0) {
			try {
				ENetPeer* peer = event.peer;
				switch (event.type) {
					case ENET_EVENT_TYPE_CONNECT:
					{
						if (server_on_freeze) break;
						send_p(peer, 1, nullptr, 0);
						peer->data = new PlayerInfo;
						char clientConnection[16];
						enet_address_get_host_ip(&peer->address, clientConnection, 16);
						pInfo(peer)->ip_ply = clientConnection;
						pInfo(peer)->peer_id = peer->connectID;
						break;
					}
					case ENET_EVENT_TYPE_RECEIVE:
					{
						if (server_on_freeze) break;
						World* world = getPlyersWorld(peer);
						int m_type = message_ptr(event.packet);
						string cch = text_ptr(event.packet);
						switch (m_type) {
							case 2:
							{
								if (itemdathash == 0) {
									enet_peer_disconnect_later(peer, 0);
									break;
								}
								if (not pInfo(peer)->inGame) {
									if (server_on_freeze) break;
									player_login(peer, cch);
									break;
								}
								if (cch.substr(0, 17) == "action|enter_game" and pInfo(peer)->inGame) {
									if (server_on_freeze) break;
									if (pInfo(peer)->block_login) {
										enet_peer_disconnect_later(peer, 0);
										break;
									}
									if (pInfo(peer)->haveGrowId) {
										int o_ct = 0, totalcount = pInfo(peer)->friendinfo.size(), gem = 0;
										string t_ = "No friends are online.", name = pInfo(peer)->displayName;
										if (not pInfo(peer)->passed_data) {
											enet_peer_disconnect_later(peer, 0);
											break;
										}
										gamepacket_t p;
										p.Insert("SetHasAccountSecured");
										p.Insert(1);
										p.CreatePacket(peer);
										Variant::OnSetHasGrowID(peer, 1, pInfo(peer)->tankIDName, pInfo(peer)->tankIDPass);
										Variant::OnEmoticonDataChanged(peer, 151224576, u8"(wl)|─ü|0&(yes)|─é|0&(no)|─â|0&(love)|─ä|0&(oops)|─à|0&(shy)|─å|0&(wink)|─ç|0&(tongue)|─ê|1&(agree)|─ë|0&(sleep)|─è|0&(punch)|─ï|0&(music)|─î|0&(build)|─ì|0&(megaphone)|─Ä|0&(sigh)|─Å|1&(mad)|─É|1&(wow)|─æ|1&(dance)|─Æ|0&(see-no-evil)|─ô|0&(bheart)|─ö|0&(heart)|─ò|0&(grow)|─û|0&(gems)|─ù|0&(kiss)|─ÿ|1&(gtoken)|─Ö|0&(lol)|─Ü|0&(smile)|─Ç|1&(cool)|─£|0&(cry)|─¥|0&(vend)|─₧|0&(bunny)|─¢|0&(cactus)|─ƒ|0&(pine)|─ñ|0&(peace)|─ú|0&(terror)|─í|0&(troll)|─á|0&(evil)|─ó|0&(fireworks)|─ª|0&(football)|─Ñ|0&(alien)|─º|0&(party)|─¿|0&(pizza)|─⌐|0&(clap)|─¬|0&(song)|─½|0&(ghost)|─¼|0&(nuke)|─¡|0&(halo)|─«|0&(turkey)|─»|0&(gift)|─░|0&(cake)|─▒|0&(heartarrow)|─▓|0&(lucky)|─│|0&(shamrock)|─┤|0&(grin)|─╡|0&(ill)|─╢|0&(eyes)|─╖|0&(weary)|─╕|0&");
										for (ENetPeer* c_cpeer = server->peers; c_cpeer < &server->peers[server->peerCount]; ++c_cpeer) {
											if (c_cpeer->state != ENET_PEER_STATE_CONNECTED || c_cpeer->data == NULL) continue;
											if (find(pInfo(peer)->friendinfo.begin(), pInfo(peer)->friendinfo.end(), pInfo(c_cpeer)->rawName) != pInfo(peer)->friendinfo.end()) {
												o_ct++;
											}
										}
										if (pInfo(peer)->lastworld == "EXIT" or pInfo(peer)->lastworld.empty()) {
											send_wo(peer);
										} else {
											join_world(peer, pInfo(peer)->lastworld, true);
										}
										if (o_ct > 0) t_ = "`w" + to_string(o_ct) + "`o friend is online.";
										Variant::OnConsoleMessage(peer, "Welcome back, `w" + name + "`o. " + t_ + "");
										Variant::OnConsoleMessage(peer, "`oWhere would you like to go? (`w" + to_string(get_player_c()) + " `oonline)");
										ifstream g_;
										g_.open("gemdb/_" + pInfo(peer)->rawName + ".txt");
										g_ >> gem;
										g_.close();
										Variant::OnSetBux(peer, gem, 0);
									} else {
										send_wo(peer);
										gamepacket_t p(500);
										p.Insert("OnDialogRequest");
										p.Insert("set_default_color|`o\n\nadd_label_with_icon|big|`wWelcome to `9GrowSenkai|left|5834|\nadd_spacer|small|\nadd_smalltext|`oGreetings, new `2PLAYERS!`o Welcome to `9GrowSenkai Private Server`o. Before you start, you need to create an account to Start Playing!.|\nadd_spacer|small|\nadd_label_with_icon|small|`oAfter you create an `2Account`o, you need to Disconnect first and Login back to play. You can visit our `5Discord `oto see Tips for Playing!|left|6102|\nadd_spacer|small|\nadd_url_button|comment|`5Discord``|noflags|https://discord.gg/9HqQh7NTsd|`wWanna check our `5Discord `wServer?|0|0|\nadd_button|growid|`oClick this to `2Get a GrowID|0|0|noflags|\n");
										p.CreatePacket(peer);
										pInfo(peer)->lobby_guest_acc = true;
									}
									break;
								}
								else if (cch == "action|refresh_item_data\n") {
									if (server_on_freeze) break;
									pInfo(peer)->update_req++;
									if (pInfo(peer)->update_req >= 2) {
										enet_peer_reset(peer);
										break;
									}
									if (itemsDat != NULL) {
										ENetPacket* packet = enet_packet_create(itemsDat, itemsDatSize + 60, ENET_PACKET_FLAG_RELIABLE);
										enet_peer_send(peer, 0, packet);
									}
									break;
								}
								else if (cch.find("action|dialog_return") == 0) {
									if (server_on_freeze) break;
									if (pInfo(peer)->lobby_guest_acc and cch.find("buttonClicked|growid") == string::npos and cch.find("buttonClicked|creategrowid") == string::npos) break;
									if (not pInfo(peer)->passed_data and cch.find("buttonClicked|growid") == string::npos and cch.find("buttonClicked|creategrowid") == string::npos) break;
									dialog_handler(peer, cch);
									break;
								}
								break;
							}
							case 3:
							{
								stringstream ss(text_ptr(event.packet));
								string cch = text_ptr(event.packet), to = "", name_ = "", id = "", a_ = "", c_jr = "0";
								while (getline(ss, to, '\n')) {
									if (to.find('|') == -1) continue;
									id = to.substr(0, to.find("|")), a_ = to.substr(to.find("|") + 1, to.length() - to.find("|") - 1);
									if (id == "name" and c_jr == "1") {
										name_ = a_;
										join_w_setting(peer, name_);
									}
									if (id == "action") {
										if (a_ == "join_request") {
											c_jr = "1";
										}
										if (a_ == "quit_to_exit") {
											left_world(peer);
											send_wo(peer);
											Variant::OnConsoleMessage(peer, "Where would you like to go? (`w" + to_string(get_player_c()) + " `oonline)");
										}
										if (a_ == "quit") {
											enet_peer_disconnect_now(peer, 0);
											delete peer->data;
											peer->data = NULL;
										}
									}
								}
								break;
							}
							case 4:
							{
								if (server_on_freeze or pInfo(peer)->currentWorld.empty()) break;
								BYTE* tank_up = struct_ptr(event.packet);
								if (tank_up) {
									PlayerMoving* pMov = unpackPlayerMoving(tank_up);
									if (pMov->x < 0 or pMov->y < 0) {
										int xblock = 3040, yblock = 736;
										for (auto i = 0; i < world->width * world->height; i++) {
											if (world->items[i].foreground == 6) {
												xblock = (i % world->width) * 32, yblock = (i / world->width) * 32;
											}
										}
										Variant::OnSetPos(peer, pInfo(peer)->netID, (float)xblock, (float)yblock);
										break;
									}
									switch (pMov->packetType) {
										case 0:
										{
											pInfo(peer)->x = (int)pMov->x;
											pInfo(peer)->y = (int)pMov->y;
											pInfo(peer)->isRotatedLeft = pMov->characterState & 0x10;
											sendPData(peer, pMov);
											if (not pInfo(peer)->joinClothesUpdated) {
												pInfo(peer)->joinClothesUpdated = true;
												update_all_clothes(peer);
											}
											break;
										}
										case 18:
										{
											sendPData(peer, pMov);
										}
										case 3:
										{
											if (pMov->punchX != -1 and pMov->punchY != -1) {
												if (pMov->packetType == 3) {
													if (pInfo(peer)->haveGrowId and pInfo(peer)->inGame and pInfo(peer)->passed_data and pInfo(peer)->currentWorld != "EXIT" and world != NULL) {
														if (pInfo(peer)->isRotatedLeft = pMov->characterState & 0x10) pInfo(peer)->RotatedLeft = true;
														else pInfo(peer)->RotatedLeft = false;
														if (pInfo(peer)->lastBreak + 100 > (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count() and pMov->plantingTree != 18 and pMov->plantingTree != 32) {
															break;
														}
														pInfo(peer)->lastBreak = (duration_cast<milliseconds>(system_clock::now().time_since_epoch())).count();
														if (pMov->plantingTree == 18) {
															tile_update(pMov->punchX, pMov->punchY, pMov->plantingTree, pInfo(peer)->netID, world, peer);
														} else {
															tile_update(pMov->punchX, pMov->punchY, pMov->plantingTree, pInfo(peer)->netID, world, peer);
														}
													}
												}
											}
										}
										delete[] pMov;
										default:
											break;
									}
								}
								break;
							}
							default:
								break;
						}
						enet_packet_destroy(event.packet);
						break;
					}
					case ENET_EVENT_TYPE_DISCONNECT:
					{
						delete (ENetPeer*)(event.peer->data);
						event.peer->data = NULL;
						break;
					}
				}
			}
			catch (...) {
				Server_alert("ERROR: Server has out/bad_of_range");
			}
		}
	}
	Server_alert("Program ended??? Huh?");
	while (1);
		return 0;
}