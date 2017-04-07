#include "vlc_tool.h"
#include "application.h"
#include "main_wnd.h"

VLCTool::VLCTool()
	: broadcast_vlc_(nullptr)
	, broadcast_name_("stu")
	, play_vlc_(nullptr)
	, play_player_(nullptr)
	, play_source_(nullptr)
	, is_playing_(false)
	, the_chack_isrun_(false)
{
}

VLCTool::~VLCTool()
{
	libvlc_release(broadcast_vlc_);
}

bool VLCTool::BeginBroadcast(string ipaddr)
{
	const char * const vlc_argv[] = {
		"--screen-fps=15",
		"--screen-follow-mouse",
		"--live-caching=100",
	};
	const char* url = "Screen://";
	string first_part = "#transcode{vcodec=mp4v,acodec=none,vb=16,threads=10,scale=0.90}:duplicate{dst=rtp{sdp=rtsp://";
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
	if (!play_source_) {
		DestoryPlay();
		return false;
	}

	play_player_ = libvlc_media_player_new_from_media(play_source_);
	if (!play_player_) {
		DestoryPlay();
		return false;
	}

	libvlc_media_player_set_hwnd(play_player_, pa_hwnd);
	libvlc_media_player_play(play_player_);
	is_playing_ = true;
	CheckPlayState();		// 启动播放的时候，就开始检测，一旦播放异常，便可停止播放
	return true;
}

void VLCTool::DestoryPlay()
{
	is_playing_ = false;
	if (play_source_) {
		libvlc_media_release(play_source_);
		play_source_ = nullptr;
	}
	if (play_player_) {
		libvlc_media_player_release(play_player_);
		play_player_ = nullptr;
	}
	if (play_vlc_) {
		libvlc_release(play_vlc_);
		play_vlc_ = nullptr;
	}
}

libvlc_state_t VLCTool::PlayState()
{
	if (play_player_)
		return libvlc_media_player_get_state(play_player_);
	return libvlc_state_t::libvlc_Error;
}

void VLCTool::CheckPlayState()
{
	auto check_thread_fun = [&]() {
		OutputDebugStringA("--------------------- start Check Play State -----------------------------");
		the_chack_isrun_ = true;
		while (true) {
			libvlc_state_t state = PlayState();
			
			std::string msg = "";
			switch (state)
			{
			case libvlc_state_t::libvlc_Buffering: msg = "libvlc_Buffering"; break;
			case libvlc_state_t::libvlc_Ended: msg = "libvlc_Ended"; break;
			case libvlc_state_t::libvlc_Error: msg = "libvlc_Error"; break;
			case libvlc_state_t::libvlc_NothingSpecial: msg = "libvlc_NothingSpecial"; break;
			case libvlc_state_t::libvlc_Opening: msg = "libvlc_Opening"; break;
			case libvlc_state_t::libvlc_Paused: msg = "libvlc_Paused"; break;
			case libvlc_state_t::libvlc_Playing: msg = "libvlc_Playing"; break;
			case libvlc_state_t::libvlc_Stopped: msg = "libvlc_Stopped"; break;
			default:
				break;
			}
			OutputDebugStringA("\n-------------------------");
			OutputDebugStringA((msg + "-------------------------\n").c_str());

			Sleep(2000);
			
			if (state != libvlc_state_t::libvlc_Playing
				&& state != libvlc_state_t::libvlc_Opening) { 
				if (state == libvlc_state_t::libvlc_NothingSpecial && is_playing_) {
					// TODO ... 重齐播放器
					libvlc_media_player_play(play_player_);
				} else {
					the_chack_isrun_ = false;
					break;
				}
			}
		}
		HWND pa_hwnd = App::GetInstance()->GetMainWnd()->GetHWND();
		::PostMessage(pa_hwnd, kAM_PlayErrorToEnd, 0, 0);		// 通知主窗口，播放结束
	};
	if (!the_chack_isrun_) {		// 无需重复启动"播放器状态检测线程"
		std::thread temp(check_thread_fun);
		chack_thread_.swap(temp);
		chack_thread_.detach();
	}
}

void VLCTool::StopCheckThread()
{
}
