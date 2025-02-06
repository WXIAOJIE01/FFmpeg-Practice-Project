#include "xcamera_record.h"
#include "xdemux_task.h"
#include "xmux_task.h"
#include "xencode_task.h"
#include "xscreen_get_task.h"
#include "xtools.h"
#include <chrono>
#include<iomanip>
#include<sstream>
using namespace std;
using namespace chrono;
/// <summary>
/// 生成存储的视频文件名
/// </summary>
/// <returns></returns>
static std::string GetfileName(string path)
{
	//获取当前时间
	stringstream ss;
	auto t = system_clock::to_time_t(system_clock::now());
	auto time_str = put_time(localtime(&t), "%Y_%m_%d_%H_%M_%S");
	ss << path << "/" << "cam_" << time_str << ".mp4";
	return ss.str() ;
}
void XCameraRecord::Main()
{
	if(index_!=2)
		record_.Open(GetfileName(save_path_), index_);
	else
		record_.Open(GetfileName(save_path_), index_, rtsp_url_);
	record_.Start();
	auto cur = NowMs();
	while (!is_exit_ && NowMs() - cur < file_sec_ * 1000)
	{
	//定时创建新的文件
		this_thread::sleep_for(1ms);
	}
	record_.Stop();

	//循环录制
	//while (!is_exit_)
	//{
	//	//定时创建新的文件
	//	if (NowMs() - cur > file_sec_ * 1000)
	//	{
	//		cur = NowMs();
	//		record_.Stop();   //停止存储，写入索引

	//		if (!record_.Open(GetfileName(save_path_), index_))								  //音频参数
	//		{
	//			LOGERROR("again record failed!");
	//			record_.Stop();
	//			return;
	//		}
	//		record_.Start();
	//	}
	//	this_thread::sleep_for(1ms);
	//}
	//record_.Stop();
	
}