#pragma once
#include "xtools.h"
#include "xcamera.h"
class XCODEC_API XCameraTask : public XThread
{
public:

    void Open();

    void Main() override;

    void Set_device(const char* name) {
        device_name_ = name;
    }

    int get_width() { return xc_.get_width(); }
    int get_height() { return xc_.get_height(); }

    //返回一份复制
    AVFrame* GetFrame();

    void Set_offset(long long offset) { offset_ = offset; }


private:
    int width_ = -1;
    int height_ = -1;

    AVFrame* frame_ = nullptr;

    XCamera xc_;
    const char* device_name_ = nullptr;
    std::mutex mux_;

    long long offset_ = 0;

};

