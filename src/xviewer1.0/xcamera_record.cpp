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
/// ���ɴ洢����Ƶ�ļ���
/// </summary>
/// <returns></returns>
static std::string GetfileName(string path)
{
	//��ȡ��ǰʱ��
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
	//��ʱ�����µ��ļ�
		this_thread::sleep_for(1ms);
	}
	record_.Stop();

	//ѭ��¼��
	//while (!is_exit_)
	//{
	//	//��ʱ�����µ��ļ�
	//	if (NowMs() - cur > file_sec_ * 1000)
	//	{
	//		cur = NowMs();
	//		record_.Stop();   //ֹͣ�洢��д������

	//		if (!record_.Open(GetfileName(save_path_), index_))								  //��Ƶ����
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