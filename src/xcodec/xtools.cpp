#include "xtools.h"
#include <sstream>
using namespace std;
extern"C" {
#include<libavcodec/avcodec.h>
#include<libavformat/avformat.h>
}
//启动线程
void XThread::Start()
{
	unique_lock<mutex> lock(mux_);

	static int i = 0;
	i++;
	index_  = i;

	is_exit_ = false;
	//启动线程
	th_ = thread(&XThread::Main, this);
	stringstream ss;
	ss << "XThread::Start()" << index_;
	LOGINFO(ss.str());
}

void XThread::Exit()
{
	stringstream ss;
	ss << "XThread::Stop() begin()" << index_;
	LOGINFO(ss.str());
	is_exit_ = true;
}

void XThread::Wait()
{
	stringstream ss;
	if (th_.joinable())  //判断子线程是否可以等待
	{
		th_.join();		//等待子线程退出 
	}
	ss.str("");
	ss << "XThread::Stop() end()" << index_;
	LOGINFO(ss.str());
}

//停止线程(设置退出标志，等待线程退出）
void XThread::Stop()
{
	Exit();
	Wait(); 
}

void MSleep(unsigned int ms)
{
	auto beg = clock();
	for (int i = 0; i < ms; i++)
	{
		this_thread::sleep_for(1ms);
		if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= ms)
			break;
	}
}

long long NowMs()
{
	return clock() / (CLOCKS_PER_SEC / 1000); 
}

void PrintErr(int err)
{
	char buf[1024] = { 0 };
	av_strerror(err, buf, sizeof(buf) - 1);
	cerr << buf << endl;
}

long long XRescale(long long pts,
	AVRational* src_time_base,
	AVRational* des_time_base)
{
	return av_rescale_q(pts, *src_time_base, *des_time_base);

}


XPara* XPara::Create()
{
	return new XPara();
}
XPara::~XPara()
{
	if (para)
	{
		avcodec_parameters_free(&para);
	}
	if (time_base)
	{
		delete time_base;
		time_base = nullptr; 
	}
}
XPara::XPara()
{
	para = avcodec_parameters_alloc();
	time_base = new AVRational();
}


AVPacket* XAVPacketList::Pop()
{
	unique_lock<mutex> lock(mux_);
	if (pkts_.empty()) return nullptr;
	auto pkt = pkts_.front();
	pkts_.pop_front();
	return pkt;
}

int XAVPacketList::Size()
{
	unique_lock<mutex> lock(mux_);
	return pkts_.size();
}

void XAVPacketList ::Push(AVPacket* pkt)
{
	unique_lock<mutex> lock(mux_);
	//生成的新的AVPacket 对象 引用计数+1；
	auto p = av_packet_alloc();
	av_packet_ref(p, pkt);   //引用计数 减少数据复制，线程安全

	pkts_.push_back(p);

	//超出最大空间，清理数据，到关键帧位置
	if (pkts_.size() > max_packets_)
	{
		//处理第一帧
		if (pkts_.front()->flags & AV_PKT_FLAG_KEY)  //关键帧
		{
			av_packet_free(&pkts_.front());  //清理
			pkts_.pop_front();  //出队
			//return;
		}
		//清理所有非关键帧之前的数据
		while (!pkts_.empty())
		{
			if (pkts_.front()->flags & AV_PKT_FLAG_KEY) //关键帧
			{
				return;
			}
			av_packet_free(&pkts_.front()); //清理
			pkts_.pop_front();    //出队
		}
	}
}

void XAVPacketList::Clear()
{
	unique_lock<mutex> lock(mux_);
	while (!pkts_.empty())
	{
		av_packet_free(&(pkts_.front()));
		pkts_.pop_front();
	}
	return;
}

AVFrame* XAVFrameList::Pop()
{
	unique_lock<mutex> lock(mux_);
	if (frames_.empty()) return nullptr;
	auto frame = frames_.front();
	frames_.pop_front();
	return frame;
}
void XAVFrameList::Push(AVFrame* frame)
{
	unique_lock<mutex> lock(mux_);
	//生成的新的AVPacket 对象 引用计数+1；
	auto p = av_frame_alloc();
	av_frame_ref(p, frame);   //引用计数 减少数据复制，线程安全

	frames_.push_back(p);
	//超出最大空间，清理数据，到关键帧位置
	if (frames_.size() > max_frames_)
	{
		//处理第一帧
		if (frames_.front()->flags & AV_PKT_FLAG_KEY)  //关键帧
		{
			av_frame_free(&frames_.front());  //清理
			frames_.pop_front();  //出队
			return;
		}
		//清理所有非关键帧之前的数据
		while (!frames_.empty())
		{
			if (frames_.front()->flags & AV_PKT_FLAG_KEY) //关键帧
			{
				return;
			}
			av_frame_free(&frames_.front()); //清理
			frames_.pop_front();    //出队
		}
	}
}

void XAVFrameList::Clear()
{
	unique_lock<mutex> lock(mux_);
	while (!frames_.empty())
	{
		av_frame_free(&(frames_.front()));
		frames_.pop_front();
	}
	return;
}

int XAVFrameList::Size()
{
	unique_lock<mutex> lock(mux_);
	return frames_.size();
}

PData* PDataList::Pop()
{
	unique_lock<mutex> lock(mux_);
	if (datas_.empty()) return nullptr;
	PData* data = datas_.front();
	datas_.pop_front();
	return data;
}

PData::PData(AVFrame* data, long long pts, long long offset)
{
	data_ = av_frame_alloc();
	av_frame_ref(data_, data);
	pts_ = pts;
	offset_ = offset;
}


void PDataList::Clear()
{
	unique_lock<mutex> lock(mux_);
	while (!datas_.empty())
	{
		if(!datas_.front()->data_|| !datas_.front()->data_->data)
			av_frame_free(&(datas_.front()->data_));
		datas_.pop_front();
	}
	return;
}

int PDataList::Size()
{
	unique_lock<mutex> lock(mux_);
	return datas_.size();
}


//PData::~PData()
//{
//	if (data_)
//	{
//		av_frame_free(&data_);
//		pts_ = 0;
//		offset_ = 0;
//	}
//}

void PDataList::Push(PData* p)
{
	unique_lock<mutex> lock(mux_);
	//生成的新的AVPacket 对象 引用计数+1；

	datas_.push_back(p);
	//超出最大空间，清理数据，到关键帧位置
	if (datas_.size() > max_data_)
	{
		//处理第一帧
		if (datas_.front()->data_->pict_type == AV_PICTURE_TYPE_I)  //关键帧
		{
			av_frame_free(&(datas_.front()->data_));  //清理
			datas_.pop_front();  //出队
			return;
		}
		//清理所有非关键帧之前的数据
		while (!datas_.empty())
		{
			if (datas_.front()->data_->pict_type == AV_PICTURE_TYPE_I) //关键帧
			{
				return;
			}
			av_frame_free(&(datas_.front()->data_));  //清理
			datas_.pop_front();    //出队
		}
	}
}


void XFreeFrame(AVFrame** frame)
{
	if (!frame || !(*frame)) return;
	av_frame_free(frame);
}



	
