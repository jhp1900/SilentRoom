#pragma once
#include "stdafx.h"
#include "vlc/vlc.h"

class VLCTool
{
public:
	VLCTool();
	~VLCTool();

	bool BeginBroadcast(string ipaddr);		// ¿ªÊ¼¹ã²¥

private:
	libvlc_instance_t *broadcast_vlc_;
	string media_name_;
};

