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

    //��ק����
    void dragEnterEvent(QDragEnterEvent* e) override;

    //��ק�ɿ�
    void dropEvent(QDropEvent* e) override;

    //˫���¼���ֹͣ����
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    //��Ⱦ
    void paintEvent(QPaintEvent* p);

    //��rtsp����ת������
    bool Open(const char* url);

    //����Ļ�����������
    bool Open();

    //������ͷ��������
    bool Open(int a);

    //��Ⱦ��Ƶ
    void Draw();

    //����������ַ
    void Set_url(std::string url);

    //�����˲���ʽ
    void Set_filter(std::string name);

private:
    std::string filter_name_ = "ԭʼ";
    std::string rtmp_url_;
    XSynPush* push_ = nullptr;
    


};

