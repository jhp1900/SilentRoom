#include "vlc_tool.h"

VLCTool::VLCTool()
	: broadcast_vlc_(nullptr)
	, broadcast_name_("stu")
	, play_vlc_(nullptr)
	, play_player_(nullptr)
	, play_source_(nullptr)
{
}

VLCTool::~VLCTool()
{
}

bool VLCTool::BeginBroadcast(string ipaddr)
{
	const char * const vlc_argv[] = {
		"--screen-fps=25",
		"--screen-follow-mouse",
		"--live-caching=300",
	};
	const char* url = "Screen://";
	string first_part = "#transcode{vcodec=mp4v,acodec=none,vb=16,threads=2,scale=0.80}:duplicate{dst=rtp{sdp=rtsp://";
	string second_part = ":554/live}}";

	first_part += ipaddr + second_part;

	broadcast_vlc_ = libvlc_new(sizeof(vlc_argv) / sizeof(vlc_argv[0]), vlc_argv);
	if (!broadcast_vlc_)
		return false;
	int ret = libvlc_vlm_add_broadcast(broadcast_vlc_, broadcast_name_.c_str(), url, (first_part).c_str(), 0, NULL, true, false);
	if (ret == -1)
		return false;
	ret = libvlc_vlm_play_media(broadcast_vlc_, broadcast_name_.c_str());
	if (ret == -1)
		return false;

	return true;
}

bool VLCTool::PlayStream(HWND pa_hwnd, string url)
{
	if (!pa_hwnd || empty(url))
		return false;

	play_vlc_ = libvlc_new(0, NULL);
	if (!play_vlc_)
		return false;
	
	play_source_ = libvlc_media_new_location(play_vlc_, url.c_str());
	if (!play_source_)
		return false;

	play_player_ = libvlc_media_player_new_from_media(play_source_);
	if (!play_player_)
		return false;

	libvlc_media_player_set_hwnd(play_player_, pa_hwnd);
	libvlc_media_player_play(play_player_);
	return true;
}

void VLCTool::DestoryPlay()
{
	libvlc_media_release(play_source_);
	libvlc_media_player_release(play_player_);
	libvlc_release(play_vlc_);
}
