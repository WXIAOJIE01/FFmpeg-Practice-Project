#include "xplayvideo.h"
#include<QDebug>
#include<QMouseEvent>

#define C(s) QString::fromLocal8Bit(s)


static bool mouse_press = false;
static QPoint mouse_point;

void XPlayVideo::timerEvent(QTimerEvent* ev)
{

	if (player_.is_pause())
	{
		ui.play->setStyleSheet("background-image: url(:/XViewer/img/play.png);");
	}
	else
	{
		ui.play->setStyleSheet("background-image: url(:/XViewer/img/pause.png);");
	}

	if (player_.is_pause()) return;
	player_.Update();
	auto pos = player_.pos_ms();
	auto total = player_.total_ms();

	ui.pos->setMaximum(total);
	ui.pos->setValue(pos);

	//计算时间时-分-秒
	//总
	int hours = total / 1000 / 60 / 60;
	int minutes = (total / 1000 / 60) % 60;
	int seconds = (total / 1000) % 60;

	QString total_time = QString("%1:%2:%3")
		.arg(hours, 2, 10, QChar('0'))
		.arg(minutes, 2, 10, QChar('0'))
		.arg(seconds, 2, 10, QChar('0'));

	hours = pos / 1000 / 60 / 60;
	minutes = (pos / 1000 / 60) % 60;
	seconds = (pos / 1000) % 60;

	QString cur_time = QString("%1:%2:%3")
		.arg(hours, 2, 10, QChar('0'))
		.arg(minutes, 2, 10, QChar('0'))
		.arg(seconds, 2, 10, QChar('0'));

	QString show_time = QString("%1/%2")
		.arg(cur_time).arg(total_time);

	ui.time->setText(show_time);
	


}


XPlayVideo::XPlayVideo(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	//去除原窗口边框
	setWindowFlags(Qt::FramelessWindowHint);
	ui.normal->setVisible(false);
	ui.max->setVisible(true);

}

void XPlayVideo::mouseMoveEvent(QMouseEvent* ev)
{
	if (!mouse_press)
	{
		QWidget::mouseMoveEvent(ev);
		return;
	}
	this->move(ev->globalPos() - mouse_point);
}
void XPlayVideo::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		mouse_press = true;
		mouse_point = ev->pos();
	}
}
void XPlayVideo::mouseReleaseEvent(QMouseEvent* ev)
{
	mouse_press = false;
}


bool XPlayVideo::Open(const char* url)
{
	if(!player_.Open(url,(void*)ui.video->winId())) return false;
	player_.Start();
	player_.Pause(false);  //播放状态
	 
	//启动定时器
	setRefresh(10);
	startTimer(refresh_time);

	return true;
}

void XPlayVideo::Close()
{

	player_.Stop();
}
void XPlayVideo::closeEvent(QCloseEvent* ev)
{
	Close();
}

void XPlayVideo::Move()
{
	player_.Pause(true);
}

void XPlayVideo::SetVolum()
{
	int v = ui.volum->value();
	QString qs = C("当前音量:");
	qs.append(QString::number((int)((double) v / 128 * 100)));
	qs.append(C("%"));
	ui.volum_txt->setText(qs);
	player_.SetVolum(v);
}

void XPlayVideo::Pause()
{
	player_.Pause(!(player_.is_pause()));

}
 
void XPlayVideo::PlayPos()
{
	player_.Seek(ui.pos->value());
	player_.Pause(false);
}



void XPlayVideo::SetSpeed()
{
	float speed = 1;
	int s = ui.speed->value();
	if (s <= 10)
	{
		speed = (float)s / 10;
	}
	else
	{
		speed = speed + (float)s / 20;
	}
	QString qs = C("当前速度:");
	qs.append(QString::number(speed));
	ui.speedtxt->setText(qs);
	player_.SetSpeed(speed);
}


void XPlayVideo::ShowMax()
{
	ui.max->setVisible(false);
	ui.normal->setVisible(true);
	showMaximized();
}
void XPlayVideo::ShowNormal()
{
	ui.max->setVisible(true);
	ui.normal->setVisible(false);
	showNormal();
}

void XPlayVideo::resizeEvent(QResizeEvent* ev)
{
	//按钮移动
	int set_x = width() - ui.set->width();
	int set_y = ui.set->y();
	ui.set->move(set_x, set_y);


	int play_x = ui.play->x();
	int temp_y = ui.play->height();
	int play_y = height() - temp_y;
	ui.play->move(play_x, play_y-20);

	int pos_x = ui.pos->x();
	ui.pos->move(pos_x, play_y);

	int time_x = ui.time->x();
	ui.time->move(time_x, play_y);

	ui.video->resize(size().width() - 30, size().height() - 150);




	//ui.playline->resize(size().width(), 80);
	//auto psize = ui.playback_wid->size();
	//auto tsize = ui.time_list->size();
	//auto csize = ui.cam_list->size();
	//ui.time_list->resize(tsize.width(), psize.height());
	//ui.cal->resize(psize.width() - tsize.width(), psize.height());
	//ui.cam_list->resize(csize.width(), psize.height());
}

XPlayVideo::~XPlayVideo()
{
	Close();
}
