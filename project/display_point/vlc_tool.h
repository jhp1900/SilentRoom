#pragma once
#include "stdafx.h"
#include "vlc/vlc.h"
#include <thread>

class VLCTool
{
public:
	VLCTool();
	~VLCTool();

	bool BeginBroadcast(string ipaddr);		// 开始广播
	bool EndBroadcast();					// 结束广播

	bool PlayStream(HWND pa_hwnd, string url);
	void DestoryPlay();
	libvlc_state_t PlayState();		// 返回当前播放状态
	void CheckPlayState();			// 如果不出于播放状态，则停止且结束线程
	void StopCheckThread();			// 结束检测线程

private:
	libvlc_instance_t *broadcast_vlc_;
	string broadcast_name_;

	libvlc_instance_t* play_vlc_;
	libvlc_media_player_t* play_player_;
	libvlc_media_t* play_source_;
	std::thread chack_thread_;
	bool is_playing_;
	bool the_chack_isrun_;
};

