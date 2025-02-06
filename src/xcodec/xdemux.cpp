#include "xdemux.h"
#include "xtools.h"

using namespace std;
extern "C" {
#include<libavformat/avformat.h>
#include<libavutil/opt.h>
}

//Ԥָ����
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")


#define BERR(err) if(err!=0){PrintErr(err); return 0; }
AVFormatContext* XDemux::Open(const char* url)
{
	AVFormatContext* c = nullptr;

	AVDictionary* opts = nullptr;
	//av_dict_set(&opts, "rtsp_transport", "tcp", 0);  //����ý����ΪtcpЭ�飬Ĭ��ΪUDPЭ��
	av_dict_set(&opts, "stimeout", "1000000", 0); //���ӳ�ʱһ��

	int re = avformat_open_input(&c, url, NULL, &opts);
	if (opts)
	{
		av_dict_free(&opts); 
	}
	BERR(re);
	//��ȡý����Ϣ
	re = avformat_find_stream_info(c, NULL);
	BERR(re);
	//��ӡ�����װ��Ϣ
	av_dump_format(c, 0, url, 0); 
	return c;
}


bool XDemux::Read(AVPacket* pkt)
{
	unique_lock<mutex> lock(mux_);
	if (!c_) return false;
	auto re = av_read_frame(c_, pkt);
	BERR(re);
	//��ʱ ���ڳ�ʱ�ж�
	last_time_ = NowMs();
	return true;
}

bool XDemux::Seek(long long pts, int stream_index)
{
	unique_lock<mutex> lock(mux_);
	if (!c_) return false;
	avformat_flush(c_);
	auto re = av_seek_frame(c_, stream_index, pts, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);   //���ؼ�֡ 
	BERR(re);
	return true;
}