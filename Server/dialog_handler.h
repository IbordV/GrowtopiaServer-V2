#pragma once
#include "enet/enet.h"
#include <Windows.h>
#include <vector>
#include <chrono>
#include <experimental/filesystem>

inline void dialog_handler(ENetPeer* peer, string cch) {
	stringstream ss(cch);
	string to = "", btn = "";

	bool register_dialog = false;
	string username = "", password = "", passwordverify = "", email = "";
	while (getline(ss, to, '\n')) {
		vector<string> infoDat = explode("|", to);
		if (infoDat.size() == 2) {
			if (infoDat.at(0) == "buttonClicked") btn = infoDat.at(1);
			if (infoDat.at(0) == "buttonClicked" && infoDat.at(1) == "creategrowid") register_dialog = true;

			if (register_dialog) {
				if (infoDat.at(0) == "username") username = infoDat.at(1);
				if (infoDat.at(0) == "password") password = infoDat.at(1);
				if (infoDat.at(0) == "passwordverify") passwordverify = infoDat.at(1);
				if (infoDat.at(0) == "email") {
					email = infoDat.at(1);
					if (pInfo(peer)->haveGrowId or not pInfo(peer)->inGame) break;
					player_reg(peer, username, password, passwordverify, email);
					break;
				}
			}
		}
	}

	if (btn == "growid") {
		Variant::OnDialogRequest(peer, "set_default_color|`o\n\nadd_label_with_icon|big|`wCreate an Account``|left|1424|\n\nadd_spacer|small|\nadd_text_input|username|`cName||30|\nadd_text_input_password|password|`cPassword||100|\nadd_text_input_password|passwordverify|`cPassword Verify||100|\nadd_textbox|Your `wemail address `owill only be used for account verification purposes and won't be spammed or shared. If you use a fake email, you'll never be able to recover or change your password.|\nadd_text_input|email|`cEmail||100|\nadd_textbox|Your `wDiscord ID `owill be used for secondary verification if you lost access to your `wemail address`o! Please enter in such format: `wdiscordname#tag`o. Your `wDiscord Tag `ocan be found in your `wDiscord account settings`o.|\nadd_text_input|discord|`cDiscord||100|\nadd_spacer|small|\nadd_button|creategrowid|`2Get an Account!|\n");
	}
}