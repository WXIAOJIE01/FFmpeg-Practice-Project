#pragma once
#include "xtools.h"
#include "xmux.h"
#include "xencode.h"
#include<mutex>
class XCODEC_API XRtmppushTask : public XThread
{
public:
	void Main() override;

	void Do(AVFrame* f);

	bool Open(const char* url_, int w, int h);

	bool Open(const char* url_, int rate);

private:
	const char* out_url_;

	std::mutex mtx_;
	XEncode enc_;
	XMux mux_;
	AVFrame* f_ = nullptr;  //½âÂëºó´æ´¢Ö¡

	AVCodecParameters* enc_par_ = nullptr;
	
	int width_ = -1;
	int height_ = -1;

	int sampleRate_ = -1;
	int channels_ = -1;

	XAVFrameList frames_;

	bool is_video = true;


};

