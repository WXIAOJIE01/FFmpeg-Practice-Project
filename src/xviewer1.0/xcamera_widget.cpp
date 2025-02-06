#include "xcamera_widget.h"
#include <QStyleOption>
#include <QPainter>
#include <QDragEnterEvent>
#include <QDropEvent>
#include<QDebug>
#include <QListWidget>
#include "xdemux_task.h"
#include "xdecode_task.h"
#include "xvideo_view.h"
#include "xcamera_config.h"
#include "xscreen_get_task.h"
#include "xcamera_task.h"

//中文字符 
#define C(s) QString::fromLocal8Bit(s)
//渲染
void XCameraWidget::paintEvent(QPaintEvent* p)
{
	//渲染样式表
	QStyleOption opt;
	opt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

}

//调用父节点的构造函数
XCameraWidget::XCameraWidget(QWidget* p) : QWidget(p)
{
	//接收拖拽
	this->setAcceptDrops(true);
}

//拖拽进入
void XCameraWidget::dragEnterEvent(QDragEnterEvent* e)
{
	//接收拖拽进入
	e->acceptProposedAction(); 
}

//拖拽松开
void XCameraWidget::dropEvent(QDropEvent* e)
{
	//拿到url
	qDebug()<<e->source()->objectName();
	auto wid = (QListWidget*)e->source();
	qDebug() << wid->currentRow();
	auto cam = XCamearConfig::Instance()->GetCam(wid->currentRow());
	is_hw_ = XCamearConfig::Instance()->is_hw();
	if (cam.name == C("Screen")&& cam.sub_url == C("Screen") && cam.url == C("Screen"))
	{
		Open();
	}
	else if(cam.name == C("Camera") && cam.sub_url == C("Camera") && cam.url == C("Camera"))
	{
		Open(1);
	}
	else
	{
		Open(cam.sub_url);
	}

}


//结束播放
void XCameraWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (decode_)
	{
		decode_->Clear();
		decode_->Stop();
		decode_ = nullptr;
	}
	if (demux_)
	{
		demux_->Stop();
		demux_ = nullptr;
	}
	if (screen_)
	{
		screen_->Stop();
		screen_ = nullptr;
	}
	if (view_)
	{
		view_->Close();
		view_ = nullptr;
	}
	if (camera_)
	{
		camera_->Stop();
		camera_ = nullptr;
	}
	return;
}

bool XCameraWidget::Open(const char* url)
{
	//打开解封装
	demux_ = new XDemuxTask();
	if (!demux_->Open(url))
	{
		return false;
	}

	//打开视频解码器
	decode_ = new XDecodeTask();
	if (is_hw_)
	{
		decode_->Set_hw(true);
	}
	auto para = demux_->CopyVideoPara();
	if (!decode_->Open(para->para))
	{
		return false;
	}
	
	//设定解码线程接受解码数据
	demux_->set_next(decode_);

	//初始化渲染参数
	view_ = XVideoView::Create();
	view_->set_win_id((void*)winId());
	view_->Init(para->para);

	//启动解封装和解码线程
	demux_->Start();
	decode_->Start(); 
	return true; 

}


bool XCameraWidget::Open()
{
	qDebug() << "open screen";
	screen_ = new XScreen_get_task();
	screen_->Open();
	screen_->Start();

	//初始化渲染参数
	view_ = XVideoView::Create();
	view_->set_win_id((void*)winId());
	if (view_->Init(screen_->get_width(), screen_->get_height(), XVideoView::YUV420P))
	{
		return false;
	}
	return true;
}


bool XCameraWidget::Open(int a)
{
	qDebug() << "open camera";
	camera_ = new XCameraTask();
	camera_->Set_device("video=HIK 1080P Camera");

	camera_->Open();
	camera_->Start();

	//初始化渲染参数
	view_ = XVideoView::Create();
	view_->set_win_id((void*)winId());
	if (view_->Init(camera_->get_width(), camera_->get_height(), XVideoView::YUV420P))
	{
		return false;
	}
	return true;

}

//渲染视频
void XCameraWidget::Draw()
{
	if (!view_) return;
	if(demux_ && decode_)
	{
		auto f = decode_->GetFrame();
		if (!f) return;
		view_->DrawFrame(f);
		XFreeFrame(&f);
	}
	else if (screen_)
	{
		auto f = screen_->GetFrame();
		if (!f) return;
		view_->DrawFrame(f);
		XFreeFrame(&f);
	}
	else if (camera_)
	{
		auto f = camera_->GetFrame();
		if (!f) return;
		view_->DrawFrame(f);
		XFreeFrame(&f);
	}

}