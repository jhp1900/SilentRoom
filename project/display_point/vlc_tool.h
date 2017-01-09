#pragma once
#include "stdafx.h"
#include "vlc/vlc.h"

class VLCTool
{
public:
	VLCTool();
	~VLCTool();

	bool BeginBroadcast(string ipaddr);		// ¿ªÊ¼¹ã²¥

	bool PlayStream(HWND pa_hwnd, string url);
	void DestoryPlay();

private:
	libvlc_instance_t *broadcast_vlc_;
	string broadcast_name_;

	libvlc_instance_t* play_vlc_;
	libvlc_media_player_t* play_player_;
	libvlc_media_t* play_source_;
};

