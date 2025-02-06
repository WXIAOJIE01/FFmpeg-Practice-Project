#include "xdecode.h"
using namespace std;
extern "C" {
#include<libavcodec/avcodec.h>
#include<libavutil/opt.h>
}

//预指令导入库
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")


bool XDecode::Send(const AVPacket* pkt)
{
	unique_lock<mutex> mtx(mtx_); 
	if (!c_) return false;
	auto re = avcodec_send_packet(c_, pkt);
	if (re != 0) return false;
	return true;
}
bool XDecode::Recv(AVFrame* frame)
{
	unique_lock<mutex> mtx(mtx_);
	if (!c_) return false;
	auto f = frame;
	int re;
	if (c_->hw_device_ctx && is_copy_)  // 硬件加速
	{
		f = av_frame_alloc();
		re = avcodec_receive_frame(c_, f);
	}
	else
	{
		re = avcodec_receive_frame(c_, frame);
		cout << re << endl;	
	}
	if (re == 0)
	{
		if (c_->hw_device_ctx && is_copy_)  //GPU解码
		{
			static long long f_pts = 0;
			f_pts = f->pts;
			//显存转内存 
			re = av_hwframe_transfer_data(frame, f, 0);
			frame->pts = f_pts;

			if (re != 0)
			{
				PrintErr(re);
				return false;
			}
			av_frame_free(&f);
		}
		return true;
	}
	if (c_->hw_device_ctx && is_copy_)
	{
		av_frame_free(&f);
	}	
	return false;
}

std::vector<AVFrame*> XDecode::End()
{
	vector<AVFrame*> res;
	unique_lock<mutex> mtx(mtx_);
	if (!c_) return res;

	//取出缓存数据
	int ret = avcodec_send_packet(c_, NULL); 
	auto frame = av_frame_alloc();
	while (ret >= 0)
	{
		if (!Recv(frame))
		{
			return res;
		}
		res.push_back(frame);
	}
	av_frame_free(&frame);
	return res;
}




bool XDecode::InitHW(int type)
{
	AVHWDeviceType;   //支持的硬解码类型  
	unique_lock<mutex> mtx(mtx_);
	if (!c_) return false;
	AVBufferRef* ctx = nullptr;  //硬件加速上下文
	auto re = av_hwdevice_ctx_create(&ctx, (AVHWDeviceType)type, NULL, NULL, 0);
	if (re != 0)
	{
		PrintErr(re);
		return false;
	}
	hw_device_ctx = ctx;
	c_->hw_device_ctx = av_buffer_ref(ctx); ;
	cout << "硬件加速" << type << endl;
	
	return true;
}
