#pragma once
#include "xtools.h"

class QAudioInput;
class SwrContext;

class ACameraTask :
    public XThread
{
public:

    bool Open();

    void Main() override;

    void Set_device(const char* name) {
        device_name_ = name;
    }

    void Set_offset(long long offset) { offset_ = offset; }


private:
    SwrContext* asc_ = nullptr;
    QAudioInput* input_;
    const char* device_name_ = nullptr;
    std::mutex mux_;

    int sampleRate_ = 44100;
    int channels_ = 2;
    int sampleByte_ = 2;

    long long offset_ = 0;


};

