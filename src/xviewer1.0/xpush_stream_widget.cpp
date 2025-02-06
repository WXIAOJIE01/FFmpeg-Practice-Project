#include "xpush_stream_widget.h"
#include <QStyleOption>
#include <QPainter>
#include <QDragEnterEvent>
#include <QDropEvent>
#include<QDebug>
#include <qlabel.h>
#include <QListWidget>
#include <QVBoxLayout>
#include "xcamera_config.h"
#include "xscreen_get_task.h"
#include "xpush_rtmp_task.h"
#include "xencode_task.h"
#include "xcamera_task.h"
#include "xdemux_task.h"
#include "xsyn_push.h"
#include <iostream>
using namespace std;

//中文字符 
#define C(s) QString::fromLocal8Bit(s)
//渲染
void XPushStreamWidget::paintEvent(QPaintEvent* p)
{
	//渲染样式表
	QStyleOption opt;
	opt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

}

//调用父节点的构造函数
XPushStreamWidget::XPushStreamWidget(QWidget* p) : QWidget(p)
{
	// 接收拖拽
	this->setAcceptDrops(true);
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor(255, 192, 203));
	this->setPalette(palette);
	this->setAutoFillBackground(true);

}

//拖拽进入
void XPushStreamWidget::dragEnterEvent(QDragEnterEvent* e)
{
	//接收拖拽进入
	e->acceptProposedAction();

	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor(135, 206, 235, 128));
	this->setPalette(palette);
	this->setAutoFillBackground(true);
}

//拖拽松开
void XPushStreamWidget::dropEvent(QDropEvent* e)
{
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor(135, 206, 235));
	this->setPalette(palette);
	this->setAutoFillBackground(true);
	//拿到url
	qDebug() << e->source()->objectName();
	auto wid = (QListWidget*)e->source();
	qDebug() << wid->currentRow();
	auto cam = XCamearConfig::Instance()->GetCam(wid->currentRow());
	//屏幕推流
	if (cam.name == C("Screen") && cam.sub_url == C("Screen") && cam.url == C("Screen"))
	{
		//获取屏幕AVFrame->编码AVPacket->RTMP推流
		Open();
	}
	//摄像机推流
	else if (cam.name == C("Camera") && cam.sub_url == C("Camera") && cam.url == C("Camera"))
	{

		Open(1);
	}
	//rtsp文件推流
	else
	{
		Open(cam.sub_url);
	}

}


void XPushStreamWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
	qDebug() << "stop push" << endl;

	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor(255, 192, 203));
	this->setPalette(palette);
	this->setAutoFillBackground(true);

	push_->Stop();
}

bool XPushStreamWidget::Open(const char* url)
{
	qDebug() << "push rtsp";
	push_ = new XSynPush;

	push_->Open(rtmp_url_, 2, url);
	push_->Start();

	return true;
}


bool XPushStreamWidget::Open()
{
	qDebug() << "push screen";
	push_ = new XSynPush;

	push_->Open(rtmp_url_, 1);
	push_->Start();

	return true;
}


bool XPushStreamWidget::Open(int a)
{
	qDebug() << "push camera";
	//获取摄像头数据AVFrame ->编码->push
	push_ = new XSynPush;

	if (filter_name_ == "原始")
		push_->SetFilter(FilterType::raw);
	else if (filter_name_ == "磨皮")
		push_->SetFilter(FilterType::bilateralFilter);
	else if (filter_name_ == "边缘")
		push_->SetFilter(FilterType::edge);
	
	push_->Open(rtmp_url_, 0);
	push_->Start();

	return true;

}

//渲染视频
void XPushStreamWidget::Draw()
{
	qDebug() << "open camera";
}

void XPushStreamWidget::Set_url(string url)
{
	rtmp_url_.clear();
	rtmp_url_ = url;
	return;
}

void XPushStreamWidget::Set_filter(string name)
{
	filter_name_.clear();
	filter_name_ = name;
	return;
}