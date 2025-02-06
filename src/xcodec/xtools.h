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

	//日志级别 DEBUG INFO ERROR FATAL

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
	//启动线程
	virtual void Start();

	//设置线程退出标志
	virtual void Exit();

	//停止线程(设置退出标志，等待线程退出)
	virtual void Stop(); 

	//等待线程退出
	virtual void Wait(); 

	//线程索引号
	int index_ = 0;

	//执行责任 需要重载
	virtual void Do(AVPacket* pkt){}

	//执行任务 需要重载
	virtual void Do(AVFrame* f) {}

	//执行任务 需要重载
	virtual void Do(unsigned char* buf) {}

	//执行任务 需要重载
	virtual void Do(PData* d) {}

	//传递到下一个责任链函数
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

	//传递到下一个责任链函数
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

	//设置责任链下一个节点(线程安全)
	void set_next(XThread* xt)
	{
		std::unique_lock<std::mutex> lock(mux_);
		next_ = xt; 
	}

	//暂停或者播放
	virtual void Pause(bool is_pause) { is_pause_ = is_pause; }
	//获取状态
	bool is_pause() { return is_pause_; }

	 
protected:

	bool is_pause_ = false;

	//线程入口函数
	virtual void Main() = 0;

	//标志线程退出
	bool is_exit_ = false;
	std::thread  th_;

	//锁
	std::mutex mux_;

	XThread *next_ = nullptr;  //责任链下一个节点 

};


XCODEC_API long long NowMs();

XCODEC_API void MSleep(unsigned int ms);

XCODEC_API void PrintErr(int err);

//根据时间基数计算
XCODEC_API long long XRescale(long long pts,
	AVRational* src_time_base,
	AVRational* des_time_base);


class XCODEC_API XTools
{
};


//音视频参数
class XPara
{
public:
	AVCodecParameters* para = nullptr;       //音视频参数
	AVRational* time_base = nullptr;         //时间基数
	long long total_ms = 0;					 //总时长 ms
	// 创建对象
	static XPara* Create();
	~XPara();
private:
	//私有是禁止创建栈中对象
	XPara();
};

/// <summary>
///  线程安全avpacket list
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
	int max_packets_ = 1000;  //最大列表数量，超出清理
	std::mutex mux_;
	 
};

/// <summary>
///  线程安全avframe list
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
	int max_frames_ = 1000;  //最大列表数量，超出清理
	std::mutex mux_;

};

/// <summary>
///  线程安全pdata list
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
	int max_data_ = 1000;  //最大列表数量，超出清理
	std::mutex mux_;

};


//struct XDate
//{
//	std::vector<unsigned char>data;
//	int offset = 0;			//偏移位置
//	long long pts = 0;		//记录时间，方便同步
//};


struct XCODEC_API PData
{
	AVFrame* data_ = nullptr;
	int offset_ = 0;			//偏移位置
	long long pts_ = 0;		//记录时间，方便同步

	PData(AVFrame* data, long long pts, long long offset);
	//PData();
	//~PData();

};


void XCODEC_API XFreeFrame(AVFrame** frame);

