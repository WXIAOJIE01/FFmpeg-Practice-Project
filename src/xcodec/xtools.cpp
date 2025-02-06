#include "xtools.h"
#include <sstream>
using namespace std;
extern"C" {
#include<libavcodec/avcodec.h>
#include<libavformat/avformat.h>
}
//�����߳�
void XThread::Start()
{
	unique_lock<mutex> lock(mux_);

	static int i = 0;
	i++;
	index_  = i;

	is_exit_ = false;
	//�����߳�
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
	if (th_.joinable())  //�ж����߳��Ƿ���Եȴ�
	{
		th_.join();		//�ȴ����߳��˳� 
	}
	ss.str("");
	ss << "XThread::Stop() end()" << index_;
	LOGINFO(ss.str());
}

//ֹͣ�߳�(�����˳���־���ȴ��߳��˳���
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
	//���ɵ��µ�AVPacket ���� ���ü���+1��
	auto p = av_packet_alloc();
	av_packet_ref(p, pkt);   //���ü��� �������ݸ��ƣ��̰߳�ȫ

	pkts_.push_back(p);

	//�������ռ䣬�������ݣ����ؼ�֡λ��
	if (pkts_.size() > max_packets_)
	{
		//�����һ֡
		if (pkts_.front()->flags & AV_PKT_FLAG_KEY)  //�ؼ�֡
		{
			av_packet_free(&pkts_.front());  //����
			pkts_.pop_front();  //����
			//return;
		}
		//�������зǹؼ�֮֡ǰ������
		while (!pkts_.empty())
		{
			if (pkts_.front()->flags & AV_PKT_FLAG_KEY) //�ؼ�֡
			{
				return;
			}
			av_packet_free(&pkts_.front()); //����
			pkts_.pop_front();    //����
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
	//���ɵ��µ�AVPacket ���� ���ü���+1��
	auto p = av_frame_alloc();
	av_frame_ref(p, frame);   //���ü��� �������ݸ��ƣ��̰߳�ȫ

	frames_.push_back(p);
	//�������ռ䣬�������ݣ����ؼ�֡λ��
	if (frames_.size() > max_frames_)
	{
		//�����һ֡
		if (frames_.front()->flags & AV_PKT_FLAG_KEY)  //�ؼ�֡
		{
			av_frame_free(&frames_.front());  //����
			frames_.pop_front();  //����
			return;
		}
		//�������зǹؼ�֮֡ǰ������
		while (!frames_.empty())
		{
			if (frames_.front()->flags & AV_PKT_FLAG_KEY) //�ؼ�֡
			{
				return;
			}
			av_frame_free(&frames_.front()); //����
			frames_.pop_front();    //����
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
	//���ɵ��µ�AVPacket ���� ���ü���+1��

	datas_.push_back(p);
	//�������ռ䣬�������ݣ����ؼ�֡λ��
	if (datas_.size() > max_data_)
	{
		//�����һ֡
		if (datas_.front()->data_->pict_type == AV_PICTURE_TYPE_I)  //�ؼ�֡
		{
			av_frame_free(&(datas_.front()->data_));  //����
			datas_.pop_front();  //����
			return;
		}
		//�������зǹؼ�֮֡ǰ������
		while (!datas_.empty())
		{
			if (datas_.front()->data_->pict_type == AV_PICTURE_TYPE_I) //�ؼ�֡
			{
				return;
			}
			av_frame_free(&(datas_.front()->data_));  //����
			datas_.pop_front();    //����
		}
	}
}


void XFreeFrame(AVFrame** frame)
{
	if (!frame || !(*frame)) return;
	av_frame_free(frame);
}



	
