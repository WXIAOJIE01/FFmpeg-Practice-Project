#pragma once

#include <QtWidgets/QWidget>
#include "ui_xviewer.h"
#include<Qmenu.h>
#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <qstring.h>
#include <qdebug.h>


class XViewer : public QWidget
{
    Q_OBJECT

public:
    XViewer(QWidget *parent = Q_NULLPTR);

//////////////////////////////////////////////////////////////// 
    //鼠标事件 用于拖动窗口
    void mouseMoveEvent(QMouseEvent* ev) override;
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;

    //窗口大小发生变化
    void resizeEvent(QResizeEvent* ev) override;

    //右键菜单
    void contextMenuEvent(QContextMenuEvent* event) override;

    //预览视频窗口
    void View(int count);

    //创建推流窗口
    void Create_push();

    //刷新左侧相机列表
    void RefreshCams();
    
    //编辑摄像机
    void SetCam(int index);

    //定时器渲染视频 回调函数
    void timerEvent(QTimerEvent* ev) override;
public slots:
    void MaxWindow();
    void NormalWindow();
    void View1(); 
    void View4();
    void View9();
    void View16 ();

    void AddCam();  //新增摄像机配置
    void SetCam();  //新增摄像机配置
    void DelCam();  //新增摄像机配置

    void StartRecord(int sec);   //开始全部摄像头录制
    void StopRecord();    //停止全部摄像头录制
    void Preview();       //预览界面
    void Playback();      //回放界面 
    void PlayPush();
    void SetDecodeType(int index);      //设置解码方式
    void SetFilterType(QString name);      //设置滤镜方式

    void SelectCamera(QModelIndex index);    //选择摄像机
    void SelectDate(QDate date);             //选择日期
    void PlayVideo(QModelIndex index);       //选择时间播放视频

    void SetSec();                          //设置录制视频的时长(s)

    void SetRtmpurl();                      //设置推流的RTMP地址，回车获取

private:
    int decodetype_ = 0;   //默认是软解的方式
    Ui::XViewerClass ui;
    QMenu left_menu_;
};



class ComboBoxDialog : public QDialog {
    Q_OBJECT

public:
    ComboBoxDialog(QWidget* parent = nullptr) : QDialog(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);

        // 创建 QComboBox
        QComboBox* comboBox = new QComboBox(this);
        comboBox->resize(200, 200);
        comboBox->addItem("RTSP");
        comboBox->addItem("RTMP");
        comboBox->addItem("Screen");
        comboBox->addItem("Camera");

        layout->addWidget(comboBox);

        // 创建一个按钮来确认选择
        QPushButton* button = new QPushButton("OK", this);
        layout->addWidget(button);

        // 连接按钮点击信号到槽函数
        connect(button, &QPushButton::clicked, [this, comboBox]() {
            select_ = comboBox->currentText();
            qDebug() << "select" << select_;
            this->accept();
            });

        setLayout(layout);
        setWindowTitle("Select");
    }

    QString get_select() { return select_; }

private:
    QString select_;



};

