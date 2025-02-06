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
    //����¼� �����϶�����
    void mouseMoveEvent(QMouseEvent* ev) override;
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;

    //���ڴ�С�����仯
    void resizeEvent(QResizeEvent* ev) override;

    //�Ҽ��˵�
    void contextMenuEvent(QContextMenuEvent* event) override;

    //Ԥ����Ƶ����
    void View(int count);

    //������������
    void Create_push();

    //ˢ���������б�
    void RefreshCams();
    
    //�༭�����
    void SetCam(int index);

    //��ʱ����Ⱦ��Ƶ �ص�����
    void timerEvent(QTimerEvent* ev) override;
public slots:
    void MaxWindow();
    void NormalWindow();
    void View1(); 
    void View4();
    void View9();
    void View16 ();

    void AddCam();  //�������������
    void SetCam();  //�������������
    void DelCam();  //�������������

    void StartRecord(int sec);   //��ʼȫ������ͷ¼��
    void StopRecord();    //ֹͣȫ������ͷ¼��
    void Preview();       //Ԥ������
    void Playback();      //�طŽ��� 
    void PlayPush();
    void SetDecodeType(int index);      //���ý��뷽ʽ
    void SetFilterType(QString name);      //�����˾���ʽ

    void SelectCamera(QModelIndex index);    //ѡ�������
    void SelectDate(QDate date);             //ѡ������
    void PlayVideo(QModelIndex index);       //ѡ��ʱ�䲥����Ƶ

    void SetSec();                          //����¼����Ƶ��ʱ��(s)

    void SetRtmpurl();                      //����������RTMP��ַ���س���ȡ

private:
    int decodetype_ = 0;   //Ĭ�������ķ�ʽ
    Ui::XViewerClass ui;
    QMenu left_menu_;
};



class ComboBoxDialog : public QDialog {
    Q_OBJECT

public:
    ComboBoxDialog(QWidget* parent = nullptr) : QDialog(parent) {
        QVBoxLayout* layout = new QVBoxLayout(this);

        // ���� QComboBox
        QComboBox* comboBox = new QComboBox(this);
        comboBox->resize(200, 200);
        comboBox->addItem("RTSP");
        comboBox->addItem("RTMP");
        comboBox->addItem("Screen");
        comboBox->addItem("Camera");

        layout->addWidget(comboBox);

        // ����һ����ť��ȷ��ѡ��
        QPushButton* button = new QPushButton("OK", this);
        layout->addWidget(button);

        // ���Ӱ�ť����źŵ��ۺ���
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

