 #pragma once

#include <QDialog>
#include "ui_xplayvideo.h"
#include "xplayer.h"

class XPlayVideo : public QDialog
{
	Q_OBJECT

public:
	XPlayVideo(QWidget *parent = Q_NULLPTR);
	~XPlayVideo();
	bool Open(const char* url);
	
	void timerEvent(QTimerEvent* ev) override;
	//重载窗口关闭的函数
	void Close();
	void closeEvent(QCloseEvent* ev) override;
	
	void setRefresh(int time) { refresh_time = time; }

	//鼠标事件 用于拖动窗口
	void mouseMoveEvent(QMouseEvent* ev) override;
	void mousePressEvent(QMouseEvent* ev) override;
	void mouseReleaseEvent(QMouseEvent* ev) override;

	//窗口大小发生变化
	void resizeEvent(QResizeEvent* ev) override;

	void Sethw(bool is_hw = false, bool is_copy = true) { player_.Sethw(is_hw, is_copy); }
public slots:
	void ShowNormal();
	void ShowMax();
	void SetSpeed();         //控制播放速度
	void PlayPos();          //控制播放进度
	void Pause();			 //播放和暂停
	void Move();			 //进度条移动
	void SetVolum();		 //设置视频播放音量
private:
	Ui::XPlayVideo ui;
	XPlayer player_;
	int refresh_time = 10;    //定时器刷新时间(ms)

};
