#pragma once
#include "xtools.h"
#include "xencode.h"

class XEncode;
class SwsContext;

class XCODEC_API XEncodeTask :
    public XThread
{
public:
    //����ĳ���ͱ���������
    bool Open(int w, int h, int codec_id = -1);

    //����Ƶ
    bool Open(int codec_id = -1);

    bool is_open() { return is_open_; }

    //������������
    void Do(AVFrame* f) override;

    //������������
    void Do(PData* d) override;

    AVCodecParameters* GetvideoPara() { return enc_.CopyVideoPara(); }

    void Set_index(bool is_video) { is_video_ = is_video; }

    void Stop();

    void set_stream_index(int index) { stream_index_ = index; }

    AVRational* get_time_base() { return enc_time_base_; }

protected:
    XEncode enc_;
    void Main() override;
    bool is_open_ = false;
    bool is_video_ = true;
    std::mutex mux_;
    int width_ = 0;
    int height_ = 0;
    int stream_index_ = 0;
    AVRational* enc_time_base_;
    XAVFrameList frames_;
    PDataList datas;
};

