 #pragma once
#include <thread>
#include <iostream>
#include <mutex>
#include <list>
#include<mutex>
#include<string>
#include<vector>

struct AVPacket;
struct AVCodecParameters;
struct AVRational;
struct AVFrame;
struct AVBufferRef;
struct AVCodecContext;
struct AVFormatContext;
struct SwsContext;

struct IDirect3D9;
struct IDirect3DDevice9;
struct IDirect3DSurface9;

struct PData;

	//��־���� DEBUG INFO ERROR FATAL

enum FilterType
{
	raw = 0,
	bilateralFilter = 1,
	whitening = 2,
	edge = 3
};

enum XLogLevel
{
	XLOG_TYPE_DEBUG,
	XLOG_TYPE_INFOR,
	XLOG_TYPE_ERROR,
	XLOG_TYPE_FATAL
};
#define LOG_MIN_LEVEL XLOG_TYPE_DEBUG
#define XLOG(s, level) \
	if(level >= LOG_MIN_LEVEL) \
	std::cout << level <<":" << __FILE__ << ":" << __LINE__ << ":"\
	<< s << std::endl; 

#define LOGDEBUG(s) XLOG(s, XLOG_TYPE_DEBUG)
#define LOGINFO(s) XLOG(s, XLOG_TYPE_INFOR)
#define LOGERROR(s) XLOG(s, XLOG_TYPE_ERROR)
#define LOGFATAL(s) XLOG(s, XLOG_TYPE_FATAL)

#ifdef _WIN32
#ifdef XCODEC_EXPORTS
#define XCODEC_API __declspec(dllexport)
#else
#define XCODEC_API __declspec(dllimport)
#endif
#else
#define XXX_API
#endif



class XCODEC_API XThread
{
public:
	//�����߳�
	virtual void Start();

	//�����߳��˳���־
	virtual void Exit();

	//ֹͣ�߳�(�����˳���־���ȴ��߳��˳�)
	virtual void Stop(); 

	//�ȴ��߳��˳�
	virtual void Wait(); 

	//�߳�������
	int index_ = 0;

	//ִ������ ��Ҫ����
	virtual void Do(AVPacket* pkt){}

	//ִ������ ��Ҫ����
	virtual void Do(AVFrame* f) {}

	//ִ������ ��Ҫ����
	virtual void Do(unsigned char* buf) {}

	//ִ������ ��Ҫ����
	virtual void Do(PData* d) {}

	//���ݵ���һ������������
	virtual void Next(AVPacket* pkt)
	{
		std::unique_lock<std::mutex> lock(mux_);
		if (next_)
			next_->Do(pkt);
	}

	virtual void Next(AVFrame* f)
	{
		std::unique_lock<std::mutex> lock(mux_);
		if (next_)
			next_->Do(f);
	}

	//���ݵ���һ������������
	virtual void Next(unsigned char* buf)
	{
		std::unique_lock<std::mutex> lock(mux_);
		if (next_)
			next_->Do(buf);
	}

	virtual void Next(PData* d)
	{
		std::unique_lock<std::mutex> lock(mux_);
		if (next_)
			next_->Do(d);
	}

	//������������һ���ڵ�(�̰߳�ȫ)
	void set_next(XThread* xt)
	{
		std::unique_lock<std::mutex> lock(mux_);
		next_ = xt; 
	}

	//��ͣ���߲���
	virtual void Pause(bool is_pause) { is_pause_ = is_pause; }
	//��ȡ״̬
	bool is_pause() { return is_pause_; }

	 
protected:

	bool is_pause_ = false;

	//�߳���ں���
	virtual void Main() = 0;

	//��־�߳��˳�
	bool is_exit_ = false;
	std::thread  th_;

	//��
	std::mutex mux_;

	XThread *next_ = nullptr;  //��������һ���ڵ� 

};


XCODEC_API long long NowMs();

XCODEC_API void MSleep(unsigned int ms);

XCODEC_API void PrintErr(int err);

//����ʱ���������
XCODEC_API long long XRescale(long long pts,
	AVRational* src_time_base,
	AVRational* des_time_base);


class XCODEC_API XTools
{
};


//����Ƶ����
class XPara
{
public:
	AVCodecParameters* para = nullptr;       //����Ƶ����
	AVRational* time_base = nullptr;         //ʱ�����
	long long total_ms = 0;					 //��ʱ�� ms
	// ��������
	static XPara* Create();
	~XPara();
private:
	//˽���ǽ�ֹ����ջ�ж���
	XPara();
};

/// <summary>
///  �̰߳�ȫavpacket list
/// </summary>
class XAVPacketList
{
public:
	AVPacket* Pop();
	void Push(AVPacket* pkt);
	int Size();
	void Clear();
private:
	std::list<AVPacket*> pkts_;
	int max_packets_ = 1000;  //����б���������������
	std::mutex mux_;
	 
};

/// <summary>
///  �̰߳�ȫavframe list
/// </summary>
class XAVFrameList
{
public:
	AVFrame* Pop();
	void Push(AVFrame* pkt);
	int Size();
	void Clear();
private:
	std::list<AVFrame*> frames_;
	int max_frames_ = 1000;  //����б���������������
	std::mutex mux_;

};

/// <summary>
///  �̰߳�ȫpdata list
/// </summary>
class XCODEC_API PDataList
{
public:
	PData* Pop();
	void Push(PData* pkt);
	int Size();
	void Clear();
private:
	std::list<PData*> datas_;
	int max_data_ = 1000;  //����б���������������
	std::mutex mux_;

};


//struct XDate
//{
//	std::vector<unsigned char>data;
//	int offset = 0;			//ƫ��λ��
//	long long pts = 0;		//��¼ʱ�䣬����ͬ��
//};


struct XCODEC_API PData
{
	AVFrame* data_ = nullptr;
	int offset_ = 0;			//ƫ��λ��
	long long pts_ = 0;		//��¼ʱ�䣬����ͬ��

	PData(AVFrame* data, long long pts, long long offset);
	//PData();
	//~PData();

};


void XCODEC_API XFreeFrame(AVFrame** frame);

