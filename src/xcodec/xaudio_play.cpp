#include "xaudio_play.h"
#include <sdl\SDL.h>
#include <iostream>

extern "C" {
#include<libavcodec/avcodec.h>
}
using namespace std;

#pragma comment(lib, "SDL2.lib")

class CXAudioPlay : public XAudioPlay
{
public:
 
	//暂停
	void Pause(bool is_pause)
	{
		if (is_pause)
		{
			SDL_PauseAudio(1);
			pause_begin = NowMs(); 
		}
		else
		{
			//去掉暂停的时间 
			if(pause_begin > 0)
				last_ms_ += (NowMs() - pause_begin); 
			SDL_PauseAudio(0);
		}

	} 

	bool Open(XAudioSpec& spec)
	{
		this->spec_ = spec;
		//退出上一次音频
		SDL_QuitSubSystem(SDL_INIT_AUDIO);

		SDL_AudioSpec  sdl_spec;
		sdl_spec.freq = spec.freq;
		sdl_spec.format = spec.format;
		sdl_spec.channels = spec.channels;
		sdl_spec.samples = spec.samples;
		sdl_spec.silence = 0;
		sdl_spec.userdata = this;
		sdl_spec.callback = AudioCallback; 
		if (SDL_OpenAudio(&sdl_spec, nullptr) < 0)
		{
			cerr << SDL_GetError() << endl;
			return false;
		}
		//开始播放
		SDL_PauseAudio(0);
		return true;
	}

	void Close()
	{
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		unique_lock<mutex> lock(mux_);
		audio_datas_.clear();
		cur_pts_ = 0;		//当前播放位置
		last_ms_ = 0;		//上次的时间戳
		pause_begin = 0;	//暂停开始时间
		
	}

	virtual void Callback(unsigned char* stream, int len)
	{
		SDL_memset(stream, 0, len);
		unique_lock<mutex> lock(mux_);
		if (audio_datas_.empty()) return;
		auto buf = audio_datas_.front();
		//1 buf大于stream缓冲 offset记录位置
		//2 buf小于stream缓冲 拼接 
		int mixde_size = 0;  //已经处理的字节数
		int need_size = len;  //已经处理的字节数
		cur_pts_ = buf.pts;
		last_ms_ = NowMs();
		while (mixde_size < len)
		{
			if (audio_datas_.empty()) break;
			buf = audio_datas_.front();
			int size = buf.data.size() - buf.offset;  //剩余未处理的数据
			if (size > need_size)
			{
				size = need_size;
			}
			SDL_MixAudio(stream + mixde_size, buf.data.data() + buf.offset, size, volume_ );
			need_size -= size;
			mixde_size += size;
			buf.offset += size;
			if (buf.offset >= buf.data.size())
			{
				audio_datas_.pop_front();
			}
		}
	}


	void SetSpeed(float s)
	{
		speed_ = s;
		auto spec = spec_;
		auto old_fre = spec.freq; 
		spec.freq *= s;
		Open(spec);
		spec_.freq = old_fre;
	}

	long long cur_pts() override
	{
		double ms = 0;
		if (last_ms_ > 0)
		{
			ms = NowMs() - last_ms_;   //距离上次写入缓冲的播放时间
		}
		//毫秒换算成pts的时间基数
		if(time_base_>0)
		ms = ms / (double)1000 / (double)time_base_;
		return cur_pts_ + speed_ * ms;
		//return cur_pts_;
	}

private:
	long long cur_pts_ = 0;		//当前播放位置
	long long last_ms_ = 0;		//上次的时间戳
	long long pause_begin = 0;	//暂停开始时间
};

bool XAudioPlay::Open(AVCodecParameters* para)
{
	XAudioSpec spec;
	spec.channels = para->channels;
	spec.freq = para->sample_rate;
	
	AVSampleFormat;
	switch (para->format)
	{
	case AV_SAMPLE_FMT_S16:           //<< signed 16bits
	case AV_SAMPLE_FMT_S16P:          //<< signed 16bits planar
		spec.format = AUDIO_S16;
		break;
	case AV_SAMPLE_FMT_S32:           //<< signed 32bits
	case AV_SAMPLE_FMT_S32P:          //<< signed 32bits planar
		spec.format = AUDIO_S32;
	case AV_SAMPLE_FMT_FLT:           //float
	case AV_SAMPLE_FMT_FLTP:          //float planar
		spec.format = AUDIO_F32; 
		

	default:
		break;
	}
	return Open(spec);
}


bool XAudioPlay::Open(XPara * para)
{
	if (para->time_base->num > 0)
		time_base_ = (double)para->time_base->den / (double)para->time_base->num;
	return Open(para->para);
}

XAudioPlay* XAudioPlay::Instance()
{
	static CXAudioPlay cx;
	return &cx;
} 

void XAudioPlay::Push(AVFrame* frame)
{
	if (!frame || !frame->data[0]) return;
	vector<unsigned char> buf;
	//暂时支持双通道
	int sample_size = av_get_bytes_per_sample((AVSampleFormat)frame->format);  
	int channels = frame->channels;
	unsigned char* L = frame->data[0];
	unsigned char* R = frame->data[1];
	unsigned char* data = nullptr;
	if (channels == 1)
	{
		Push(frame->data[0], frame->nb_samples*sample_size, frame->pts);
		return;
	}
	switch (frame->format )
	{
	//case AV_SAMPLE_FMT_S16P:          //<< signed 16bits planar
	case AV_SAMPLE_FMT_S32P:          //<< signed 32bits planar
	case AV_SAMPLE_FMT_FLTP:          //float planar
		buf.resize(frame->linesize[0]);
		data = buf.data();
		//LLLLL RRRRR -> LRLRLRLRLR

		for (int i = 0; i < frame->nb_samples ; i++)
		{
			
			memcpy(data + i * sample_size * channels, L + i * sample_size, sample_size);
			memcpy(data + i * sample_size * channels+sample_size, R + i * sample_size, sample_size);
		}
		Push(data, frame->linesize[0], frame->pts);
		return;
		break;

	default:
		break;
	}
	Push(frame->data[0], frame->linesize[0], frame->pts);
}

XAudioPlay::XAudioPlay()
{
	SDL_Init(SDL_INIT_AUDIO); 
}