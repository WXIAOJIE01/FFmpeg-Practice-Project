#pragma once
#include "xfilter.h"
class XCODEC_API XEdge :
    public XFilter
{
public:
    bool Init() override;
    // Í¨¹ý XFilter ¼Ì³Ð
    virtual PData* process(PData* frame) override;

private:

};

