#pragma once
#include "xfilter.h"
class XCODEC_API XEdge :
    public XFilter
{
public:
    bool Init() override;
    // ͨ�� XFilter �̳�
    virtual PData* process(PData* frame) override;

private:

};

