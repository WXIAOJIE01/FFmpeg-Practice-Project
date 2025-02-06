#pragma once
#include "xtools.h"
#include "xdecode.h"
class XCODEC_API XDecodeTask  :
    public XThread
{
public:
    /// <summary>
    /// 打开解码器
    /// </summary>
    /// <param name="para"></param>
    /// <returns></returns>
    bool Open(AVCodecParameters* para);

    //责任链处理函数
    void Do(AVPacket* pkt) override;

    //线程主函数
    void Main() override;

    //返回当前需要渲染的AVFrame，如果没有返回null
    //need_view_控制渲染
    //返回结果需要用XFree Frame释放
    AVFrame* GetFrame();

    //硬渲染
    void DrawFrame(AVFrame* frame, AVCodecContext* c, void* hwnd1);

    //设置传入的pkt的stream_index
    void set_stream_index(int i) { stream_index_ = i; }

    void set_frame_cache(bool set) { frame_cache_ = set; }

    bool is_open() { return is_open_; }

    //设置同步时间
    void set_syn_pts(long long p) { syn_pts_ = p; }

    void set_block_size(int s) { block_size_ = s; }

    //清理缓存
    void Clear();

    //清理资源
    void Stop();
     
    //当前播放位置的毫秒
    long long cur_ms() { return cur_ms_; }

    void set_time_base(AVRational* time_base);

    void set_seek_pos(long long pts) { seek_pts_ = pts; }

    //设置硬件解码
    void Set_hw(bool is_hw, bool is_copy=true) { is_hw_ = is_hw, is_copy_ = is_copy; }

    bool Get_hw_nocopy() { return is_hw_ && !is_copy_; }

    AVCodecContext* CopyCodecContext() { return decode_.CopyCodecContext(); }

    bool SendPacket(AVPacket* pkt);

    AVFrame* Get_seekFrame();
private:
    long long cur_pts_ = -1;  //当前解码到的pts(以解码数据为准 )
    AVRational* time_base_ = nullptr;

    long long cur_ms_ = 0;
    int block_size_ = 0;            //阻塞大小
    long long syn_pts_ = -1;          //同步时间 -1不同步
    bool is_open_ = false;
    int stream_index_ = 0;
    std::mutex mux_;
    XDecode decode_;
    XAVPacketList pkt_list_; 
    AVFrame* frame_ = nullptr;      //解码后存储
    
    bool need_view_ = false;        //是否需要渲染，每帧只渲染一次，通过GetFrame控制
    bool need_round = false;        

    std::list<AVFrame*> frames_;    //存储音频缓冲
    bool frame_cache_ = false;      //是否缓冲frame队列

    long long seek_pts_ = 0;

    bool is_hw_ = false;
    bool is_copy_ = true;
};

