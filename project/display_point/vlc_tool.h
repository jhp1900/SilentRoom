#pragma once
#include "stdafx.h"
#include "vlc/vlc.h"
#include <thread>

class VLCTool
{
public:
	VLCTool();
	~VLCTool();

	bool BeginBroadcast(string ipaddr);		// ��ʼ�㲥
	bool EndBroadcast();					// �����㲥

	bool PlayStream(HWND pa_hwnd, string url);
	void DestoryPlay();
	libvlc_state_t PlayState();		// ���ص�ǰ����״̬
	void CheckPlayState();			// ��������ڲ���״̬����ֹͣ�ҽ����߳�
	void StopCheckThread();			// ��������߳�

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

