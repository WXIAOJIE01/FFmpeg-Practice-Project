#include "xfilter_task.h"
#include "xfilter.h"
#include <thread>
using namespace std;

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")

void XFilterTask::Main()
{
	while (!is_exit_)
	{
		auto frame = frames_.Pop();
		if (!frame)
		{
			this_thread::sleep_for(1ms);
			continue;
		}
		if (!filter_) {
			Next(frame);
			continue;
		}
		auto out_frame = filter_->process(frame);
		Next(out_frame);
	}
}

void XFilterTask::Open(FilterType type)
{
	filter_ = XFilter::create(type);
	if (filter_)
		filter_->set_stream_index(stream_index_);
	return;
}

void XFilterTask::Do(PData* f)
{
	if (!f->data_) return;
	frames_.Push(f);

}

void XFilterTask::Stop()
{
	XThread::Stop();
	//frames_.Clear();

}
