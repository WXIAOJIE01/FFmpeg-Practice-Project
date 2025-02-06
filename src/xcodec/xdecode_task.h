#pragma once
#include "xtools.h"
#include "xdecode.h"
class XCODEC_API XDecodeTask  :
    public XThread
{
public:
    /// <summary>
    /// �򿪽�����
    /// </summary>
    /// <param name="para"></param>
    /// <returns></returns>
    bool Open(AVCodecParameters* para);

    //������������
    void Do(AVPacket* pkt) override;

    //�߳�������
    void Main() override;

    //���ص�ǰ��Ҫ��Ⱦ��AVFrame�����û�з���null
    //need_view_������Ⱦ
    //���ؽ����Ҫ��XFree Frame�ͷ�
    AVFrame* GetFrame();

    //Ӳ��Ⱦ
    void DrawFrame(AVFrame* frame, AVCodecContext* c, void* hwnd1);

    //���ô����pkt��stream_index
    void set_stream_index(int i) { stream_index_ = i; }

    void set_frame_cache(bool set) { frame_cache_ = set; }

    bool is_open() { return is_open_; }

    //����ͬ��ʱ��
    void set_syn_pts(long long p) { syn_pts_ = p; }

    void set_block_size(int s) { block_size_ = s; }

    //������
    void Clear();

    //������Դ
    void Stop();
     
    //��ǰ����λ�õĺ���
    long long cur_ms() { return cur_ms_; }

    void set_time_base(AVRational* time_base);

    void set_seek_pos(long long pts) { seek_pts_ = pts; }

    //����Ӳ������
    void Set_hw(bool is_hw, bool is_copy=true) { is_hw_ = is_hw, is_copy_ = is_copy; }

    bool Get_hw_nocopy() { return is_hw_ && !is_copy_; }

    AVCodecContext* CopyCodecContext() { return decode_.CopyCodecContext(); }

    bool SendPacket(AVPacket* pkt);

    AVFrame* Get_seekFrame();
private:
    long long cur_pts_ = -1;  //��ǰ���뵽��pts(�Խ�������Ϊ׼ )
    AVRational* time_base_ = nullptr;

    long long cur_ms_ = 0;
    int block_size_ = 0;            //������С
    long long syn_pts_ = -1;          //ͬ��ʱ�� -1��ͬ��
    bool is_open_ = false;
    int stream_index_ = 0;
    std::mutex mux_;
    XDecode decode_;
    XAVPacketList pkt_list_; 
    AVFrame* frame_ = nullptr;      //�����洢
    
    bool need_view_ = false;        //�Ƿ���Ҫ��Ⱦ��ÿֻ֡��Ⱦһ�Σ�ͨ��GetFrame����
    bool need_round = false;        

    std::list<AVFrame*> frames_;    //�洢��Ƶ����
    bool frame_cache_ = false;      //�Ƿ񻺳�frame����

    long long seek_pts_ = 0;

    bool is_hw_ = false;
    bool is_copy_ = true;
};

