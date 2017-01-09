#pragma once
#include "stdafx.h"
#include "vlc/vlc.h"

class VLCTool
{
public:
	VLCTool();
	~VLCTool();

	bool BeginBroadcast(string ipaddr);		// ��ʼ�㲥

private:
	libvlc_instance_t *broadcast_vlc_;
	string media_name_;
};

