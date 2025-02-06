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

//�����ַ� 
#define C(s) QString::fromLocal8Bit(s)
//��Ⱦ
void XPushStreamWidget::paintEvent(QPaintEvent* p)
{
	//��Ⱦ��ʽ��
	QStyleOption opt;
	opt.init(this);
	QPainter painter(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

}

//���ø��ڵ�Ĺ��캯��
XPushStreamWidget::XPushStreamWidget(QWidget* p) : QWidget(p)
{
	// ������ק
	this->setAcceptDrops(true);
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor(255, 192, 203));
	this->setPalette(palette);
	this->setAutoFillBackground(true);

}

//��ק����
void XPushStreamWidget::dragEnterEvent(QDragEnterEvent* e)
{
	//������ק����
	e->acceptProposedAction();

	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor(135, 206, 235, 128));
	this->setPalette(palette);
	this->setAutoFillBackground(true);
}

//��ק�ɿ�
void XPushStreamWidget::dropEvent(QDropEvent* e)
{
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, QColor(135, 206, 235));
	this->setPalette(palette);
	this->setAutoFillBackground(true);
	//�õ�url
	qDebug() << e->source()->objectName();
	auto wid = (QListWidget*)e->source();
	qDebug() << wid->currentRow();
	auto cam = XCamearConfig::Instance()->GetCam(wid->currentRow());
	//��Ļ����
	if (cam.name == C("Screen") && cam.sub_url == C("Screen") && cam.url == C("Screen"))
	{
		//��ȡ��ĻAVFrame->����AVPacket->RTMP����
		Open();
	}
	//���������
	else if (cam.name == C("Camera") && cam.sub_url == C("Camera") && cam.url == C("Camera"))
	{

		Open(1);
	}
	//rtsp�ļ�����
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
	//��ȡ����ͷ����AVFrame ->����->push
	push_ = new XSynPush;

	if (filter_name_ == "ԭʼ")
		push_->SetFilter(FilterType::raw);
	else if (filter_name_ == "ĥƤ")
		push_->SetFilter(FilterType::bilateralFilter);
	else if (filter_name_ == "��Ե")
		push_->SetFilter(FilterType::edge);
	
	push_->Open(rtmp_url_, 0);
	push_->Start();

	return true;

}

//��Ⱦ��Ƶ
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