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
	//���ش��ڹرյĺ���
	void Close();
	void closeEvent(QCloseEvent* ev) override;
	
	void setRefresh(int time) { refresh_time = time; }

	//����¼� �����϶�����
	void mouseMoveEvent(QMouseEvent* ev) override;
	void mousePressEvent(QMouseEvent* ev) override;
	void mouseReleaseEvent(QMouseEvent* ev) override;

	//���ڴ�С�����仯
	void resizeEvent(QResizeEvent* ev) override;

	void Sethw(bool is_hw = false, bool is_copy = true) { player_.Sethw(is_hw, is_copy); }
public slots:
	void ShowNormal();
	void ShowMax();
	void SetSpeed();         //���Ʋ����ٶ�
	void PlayPos();          //���Ʋ��Ž���
	void Pause();			 //���ź���ͣ
	void Move();			 //�������ƶ�
	void SetVolum();		 //������Ƶ��������
private:
	Ui::XPlayVideo ui;
	XPlayer player_;
	int refresh_time = 10;    //��ʱ��ˢ��ʱ��(ms)

};
