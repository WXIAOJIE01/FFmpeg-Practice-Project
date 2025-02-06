#pragma once
#include <QWidget>

class XDecodeTask;
class XDemuxTask;
class XVideoView;
class XScreen_get_task;
class XCameraTask;

class XCameraWidget :
    public QWidget
{
    Q_OBJECT 
public:
    XCameraWidget(QWidget* p=nullptr);

    //��ק����
    void dragEnterEvent(QDragEnterEvent* e) override;

    //��ק�ɿ�
    void dropEvent(QDropEvent* e) override;

    //˫���¼���ֹͣ����
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    //��Ⱦ
    void paintEvent(QPaintEvent* p); 

    //��������
    bool Open(const char* url);

    //����Ļ����
    bool Open();

    //������ͷ����
    bool Open(int a);

    //��Ⱦ��Ƶ
    void Draw();

private:
    bool is_hw_ = false;
    XDecodeTask* decode_ = nullptr;
    XDemuxTask* demux_ = nullptr;
    XScreen_get_task* screen_ = nullptr;
    XVideoView* view_ = nullptr;
    XCameraTask* camera_ = nullptr;
};

