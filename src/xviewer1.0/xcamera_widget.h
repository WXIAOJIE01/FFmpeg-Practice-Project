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

    //拖拽进入
    void dragEnterEvent(QDragEnterEvent* e) override;

    //拖拽松开
    void dropEvent(QDropEvent* e) override;

    //双击事件，停止播放
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    //渲染
    void paintEvent(QPaintEvent* p); 

    //打开流数据
    bool Open(const char* url);

    //打开屏幕数据
    bool Open();

    //打开摄像头数据
    bool Open(int a);

    //渲染视频
    void Draw();

private:
    bool is_hw_ = false;
    XDecodeTask* decode_ = nullptr;
    XDemuxTask* demux_ = nullptr;
    XScreen_get_task* screen_ = nullptr;
    XVideoView* view_ = nullptr;
    XCameraTask* camera_ = nullptr;
};

