#include "vlc_tool.h"

VLCTool::VLCTool()
	: broadcast_vlc_(nullptr)
	, media_name_("stu")
{
}

VLCTool::~VLCTool()
{
}

bool VLCTool::BeginBroadcast(string ipaddr)
{
	const char * const vlc_argv[] = {
		"--screen-fps=15",
		"--screen-follow-mouse",
		"--live-caching=100",
		"--screen-fragment-size=16"
	};
	const char* url = "Screen://";
	string first_part = "#transcode{vcodec=mpeg4,fps=25,acodec=none,vb=256,threads=2,scale=1.00}:duplicate{dst=rtp{sdp=rtsp://";
	string second_part = ":554/live}}";

	first_part += ipaddr + second_part;

	broadcast_vlc_ = libvlc_new(sizeof(vlc_argv) / sizeof(vlc_argv[0]), vlc_argv);
	if (!broadcast_vlc_)
		return false;
	int ret = libvlc_vlm_add_broadcast(broadcast_vlc_, media_name_.c_str(), url, (first_part).c_str(), 0, NULL, true, false);
	if (ret == -1)
		return false;
	ret = libvlc_vlm_play_media(broadcast_vlc_, media_name_.c_str());
	if (ret == -1)
		return false;

	return true;
}
