#pragma once
#include "xtools.h"

class XFilter;

class XCODEC_API XFilterTask :
    public XThread
{
public:
    virtual void Main() override;

    void Open(FilterType type);

    void Do(PData* f);

    void set_stream_index(int index) { stream_index_ = index; }

    void Stop() override;

protected:
    int stream_index_ = -1;
    XFilter* filter_ = nullptr;
    PDataList frames_;
};

