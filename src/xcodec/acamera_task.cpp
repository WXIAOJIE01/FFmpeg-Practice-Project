#include "acamera_task.h"
#include <QtCore/QCoreApplication>
#include <QAudioInput>
#include<iostream>
using namespace std;

extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")

//enum AVSampleFormat {
//    AV_SAMPLE_FMT_NONE = -1,
//    AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
//    AV_SAMPLE_FMT_S16,         ///< signed 16 bits
//    AV_SAMPLE_FMT_S32,         ///< signed 32 bits
//    AV_SAMPLE_FMT_FLT,         ///< float
//    AV_SAMPLE_FMT_DBL,         ///< double
//
//    AV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
//    AV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
//    AV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
//    AV_SAMPLE_FMT_FLTP,        ///< float, planar
//    AV_SAMPLE_FMT_DBLP,        ///< double, planar
//    AV_SAMPLE_FMT_S64,         ///< signed 64 bits
//    AV_SAMPLE_FMT_S64P,        ///< signed 64 bits, planar
//
//    AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
//};

AVSampleFormat inSampleFmt_ = AV_SAMPLE_FMT_S16;
AVSampleFormat outSampleFmt_ = AV_SAMPLE_FMT_FLTP;

bool ACameraTask::Open()
{

    sampleRate_ = 44100;
    channels_ = 2;
    sampleByte_ = 2;

    inSampleFmt_ = AV_SAMPLE_FMT_S16;
    outSampleFmt_ = AV_SAMPLE_FMT_FLTP;

	//1 qt音频开始录制
    QAudioFormat fmt;
    fmt.setSampleRate(sampleRate_);
    fmt.setChannelCount(channels_);
    fmt.setSampleSize(sampleByte_ * 8);

    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(fmt))
    {
        cout << "Audio format not support!" << endl;
        fmt = info.nearestFormat(fmt);
    }

    input_ = new QAudioInput(fmt);

    //2.音频重采样 上下文初始化

    asc_ = swr_alloc_set_opts(asc_,
        av_get_default_channel_layout(channels_), outSampleFmt_, sampleRate_, //输出格式
        av_get_default_channel_layout(channels_), inSampleFmt_, sampleRate_, //输入格式
        0, 0
    );
    if (!asc_)
    {
        cout << "swr_alloc_set_opts failed!";
        getchar();
        return false;
    }
    int ret = swr_init(asc_);
    if (ret != 0)
    {
        cerr << "swr init error" << endl;
        return false;

    }
    cout << "音频重采样上下文成功" << endl;
}

void ACameraTask::Main()
{
    AVFrame* pcm = av_frame_alloc();
    pcm->format = outSampleFmt_;
    pcm->channels = channels_;
    pcm->channel_layout = av_get_default_channel_layout(2);
    pcm->nb_samples = 1024;   //一帧音频一通道的采样数量
    av_frame_get_buffer(pcm, 0);   //给pcm分配存储空间

    int readSize = pcm->nb_samples * channels_ * sampleByte_;
    char* buf = new char[readSize];


    //开始录制音频
    QIODevice* io = input_->start();
    int apts = 0;
    long long lasta = -1;
    while (!is_exit_)
    {
        if (input_->bytesReady() < readSize)
        {
            this_thread::sleep_for(chrono::milliseconds(1));
            continue;
        }
        int size = 0;
        while (size != readSize)
        {
            int len = io->read(buf + size, readSize - size);
            if (len < 0) break;
            size += len;
        }
        if (size != readSize) continue;
        //cout << size << " " << flush;

        //记录时间用于同步
        long long pts = NowMs()+500;
        if (lasta == pts)
        {
            pts += 1000;
        }

        //重采样源数据
        const uint8_t* indata[AV_NUM_DATA_POINTERS] = { 0 };
        indata[0] = (uint8_t*)buf;
        //输出样本数量 number of sample per channel
        int len = swr_convert(asc_, pcm->data, pcm->nb_samples,    //输出参数，输出存储地址和样本数量 
            indata, pcm->nb_samples);


        Next(&PData(pcm, pts, offset_));
        cout << "a" << flush;
    }
    //清理资源
    input_->stop();
    io->close();
    input_ = nullptr;
    delete[] buf;

}