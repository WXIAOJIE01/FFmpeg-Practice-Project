#pragma once
#include <qwidget.h>
#include <QWidget>

class XSynPush;
#include<string>

class XPushStreamWidget :
    public QWidget
{
    Q_OBJECT
public:
    XPushStreamWidget(QWidget* p = nullptr);

    //拖拽进入
    void dragEnterEvent(QDragEnterEvent* e) override;

    //拖拽松开
    void dropEvent(QDropEvent* e) override;

    //双击事件，停止推流
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    //渲染
    void paintEvent(QPaintEvent* p);

    //对rtsp进行转码推流
    bool Open(const char* url);

    //对屏幕共享进行推流
    bool Open();

    //对摄像头进行推流
    bool Open(int a);

    //渲染视频
    void Draw();

    //设置推流地址
    void Set_url(std::string url);

    //设置滤波方式
    void Set_filter(std::string name);

private:
    std::string filter_name_ = "原始";
    std::string rtmp_url_;
    XSynPush* push_ = nullptr;
    


};

