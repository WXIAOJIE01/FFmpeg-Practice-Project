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

//�����ַ� 
#define C(s) QString::fromLocal8Bit(s)
//��Ⱦ
void XCameraWidget::paintEvent(QPaintEvent* p)
{
	//��Ⱦ��ʽ��
	QStyleOption opt;
	opt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

}

//���ø��ڵ�Ĺ��캯��
XCameraWidget::XCameraWidget(QWidget* p) : QWidget(p)
{
	//������ק
	this->setAcceptDrops(true);
}

//��ק����
void XCameraWidget::dragEnterEvent(QDragEnterEvent* e)
{
	//������ק����
	e->acceptProposedAction(); 
}

//��ק�ɿ�
void XCameraWidget::dropEvent(QDropEvent* e)
{
	//�õ�url
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


//��������
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
	//�򿪽��װ
	demux_ = new XDemuxTask();
	if (!demux_->Open(url))
	{
		return false;
	}

	//����Ƶ������
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
	
	//�趨�����߳̽��ܽ�������
	demux_->set_next(decode_);

	//��ʼ����Ⱦ����
	view_ = XVideoView::Create();
	view_->set_win_id((void*)winId());
	view_->Init(para->para);

	//�������װ�ͽ����߳�
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

	//��ʼ����Ⱦ����
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

	//��ʼ����Ⱦ����
	view_ = XVideoView::Create();
	view_->set_win_id((void*)winId());
	if (view_->Init(camera_->get_width(), camera_->get_height(), XVideoView::YUV420P))
	{
		return false;
	}
	return true;

}

//��Ⱦ��Ƶ
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