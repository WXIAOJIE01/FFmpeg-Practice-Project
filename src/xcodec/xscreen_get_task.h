#pragma once
#include "xtools.h"
#include "xscreen.h"
class XCODEC_API XScreen_get_task :
    public XThread
{
public:
    void Open();

    void Main() override;

    AVFrame* GetFrame();

    int get_width() { return w_; }
    int get_height() { return h_; }

    void Set_offset(long long offset) { offset_ = offset; }
private:
    std::mutex mux_;
    Xscreen screen_;
    unsigned char* buf_ = nullptr;
    int size_ = 0;

    AVFrame* frame_ = nullptr;

    //转换上下文
    SwsContext* rgb2yuv_ = nullptr;

    int w_ = -1;
    int h_ = -1;

    long long offset_ = 0;
};

