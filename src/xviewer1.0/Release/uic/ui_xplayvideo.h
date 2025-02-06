/********************************************************************************
** Form generated from reading UI file 'xplayvideo.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XPLAYVIDEO_H
#define UI_XPLAYVIDEO_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_XPlayVideo
{
public:
    QWidget *video;
    QWidget *set;
    QPushButton *close;
    QPushButton *max;
    QPushButton *min;
    QPushButton *normal;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *rate_2;
    QSlider *volum;
    QLabel *volum_txt;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *speedtxt;
    QSlider *speed;
    QLabel *rate;
    QLabel *time;
    QPushButton *play;
    QSlider *pos;

    void setupUi(QWidget *XPlayVideo)
    {
        if (XPlayVideo->objectName().isEmpty())
            XPlayVideo->setObjectName(QString::fromUtf8("XPlayVideo"));
        XPlayVideo->resize(754, 667);
        XPlayVideo->setMaximumSize(QSize(1920, 16777215));
        XPlayVideo->setStyleSheet(QString::fromUtf8("#close\n"
"{\n"
"	background-image: url(:/XViewer/img/close.png);\n"
"}\n"
"#min\n"
"{\n"
"	background-image: url(:/XViewer/img/min.png);\n"
"}\n"
"#max\n"
"{\n"
"	background-image: url(:/XViewer/img/max.png);\n"
"}\n"
"\n"
"#normal\n"
"{\n"
"	background-image: url(:/XViewer/img/normal.png);\n"
"}\n"
""));
        video = new QWidget(XPlayVideo);
        video->setObjectName(QString::fromUtf8("video"));
        video->setGeometry(QRect(10, 60, 721, 511));
        set = new QWidget(XPlayVideo);
        set->setObjectName(QString::fromUtf8("set"));
        set->setGeometry(QRect(610, 20, 121, 31));
        close = new QPushButton(set);
        close->setObjectName(QString::fromUtf8("close"));
        close->setGeometry(QRect(90, 10, 20, 20));
        max = new QPushButton(set);
        max->setObjectName(QString::fromUtf8("max"));
        max->setGeometry(QRect(60, 10, 20, 20));
        min = new QPushButton(set);
        min->setObjectName(QString::fromUtf8("min"));
        min->setGeometry(QRect(30, 10, 21, 20));
        normal = new QPushButton(set);
        normal->setObjectName(QString::fromUtf8("normal"));
        normal->setGeometry(QRect(60, 10, 20, 20));
        horizontalLayoutWidget = new QWidget(XPlayVideo);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(300, 10, 191, 41));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        rate_2 = new QLabel(horizontalLayoutWidget);
        rate_2->setObjectName(QString::fromUtf8("rate_2"));
        rate_2->setStyleSheet(QString::fromUtf8("alternate-background-color: rgba(255, 255, 255, 0);"));

        horizontalLayout->addWidget(rate_2);

        volum = new QSlider(horizontalLayoutWidget);
        volum->setObjectName(QString::fromUtf8("volum"));
        volum->setMinimum(0);
        volum->setMaximum(128);
        volum->setSingleStep(1);
        volum->setPageStep(2);
        volum->setValue(64);
        volum->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(volum);

        volum_txt = new QLabel(horizontalLayoutWidget);
        volum_txt->setObjectName(QString::fromUtf8("volum_txt"));
        volum_txt->setStyleSheet(QString::fromUtf8("font: 10pt \"Times New Roman\";\n"
""));

        horizontalLayout->addWidget(volum_txt);

        horizontalLayoutWidget_2 = new QWidget(XPlayVideo);
        horizontalLayoutWidget_2->setObjectName(QString::fromUtf8("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(30, 10, 191, 41));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        speedtxt = new QLabel(horizontalLayoutWidget_2);
        speedtxt->setObjectName(QString::fromUtf8("speedtxt"));
        speedtxt->setStyleSheet(QString::fromUtf8("font: 10pt \"Times New Roman\";\n"
""));

        horizontalLayout_2->addWidget(speedtxt);

        speed = new QSlider(horizontalLayoutWidget_2);
        speed->setObjectName(QString::fromUtf8("speed"));
        speed->setMinimum(1);
        speed->setMaximum(20);
        speed->setSingleStep(1);
        speed->setPageStep(2);
        speed->setValue(10);
        speed->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(speed);

        rate = new QLabel(horizontalLayoutWidget_2);
        rate->setObjectName(QString::fromUtf8("rate"));
        rate->setStyleSheet(QString::fromUtf8("alternate-background-color: rgba(255, 255, 255, 0);"));

        horizontalLayout_2->addWidget(rate);

        time = new QLabel(XPlayVideo);
        time->setObjectName(QString::fromUtf8("time"));
        time->setGeometry(QRect(650, 600, 151, 21));
        play = new QPushButton(XPlayVideo);
        play->setObjectName(QString::fromUtf8("play"));
        play->setGeometry(QRect(10, 580, 71, 61));
        play->setStyleSheet(QString::fromUtf8("background-image: url(:/XViewer/img/play.png);"));
        pos = new QSlider(XPlayVideo);
        pos->setObjectName(QString::fromUtf8("pos"));
        pos->setGeometry(QRect(100, 600, 521, 22));
        pos->setOrientation(Qt::Horizontal);

        retranslateUi(XPlayVideo);
        QObject::connect(speed, SIGNAL(sliderReleased()), XPlayVideo, SLOT(SetSpeed()));
        QObject::connect(pos, SIGNAL(sliderReleased()), XPlayVideo, SLOT(PlayPos()));
        QObject::connect(play, SIGNAL(clicked()), XPlayVideo, SLOT(Pause()));
        QObject::connect(pos, SIGNAL(sliderMoved(int)), XPlayVideo, SLOT(Move()));
        QObject::connect(volum, SIGNAL(sliderReleased()), XPlayVideo, SLOT(SetVolum()));
        QObject::connect(min, SIGNAL(clicked()), XPlayVideo, SLOT(showMinimized()));
        QObject::connect(close, SIGNAL(clicked()), XPlayVideo, SLOT(close()));
        QObject::connect(max, SIGNAL(clicked()), XPlayVideo, SLOT(ShowMax()));
        QObject::connect(normal, SIGNAL(clicked()), XPlayVideo, SLOT(ShowNormal()));

        QMetaObject::connectSlotsByName(XPlayVideo);
    } // setupUi

    void retranslateUi(QWidget *XPlayVideo)
    {
        XPlayVideo->setWindowTitle(QCoreApplication::translate("XPlayVideo", "XPlayVideo", nullptr));
        close->setText(QString());
        max->setText(QString());
        min->setText(QString());
        normal->setText(QString());
        rate_2->setText(QCoreApplication::translate("XPlayVideo", "\351\237\263\351\207\217", nullptr));
        volum_txt->setText(QCoreApplication::translate("XPlayVideo", "\345\275\223\345\211\215\351\237\263\351\207\217:50%", nullptr));
        speedtxt->setText(QCoreApplication::translate("XPlayVideo", "\345\275\223\345\211\215\351\200\237\345\272\246:1", nullptr));
        rate->setText(QCoreApplication::translate("XPlayVideo", "\346\222\255\346\224\276\351\200\237\345\272\246:", nullptr));
        time->setText(QCoreApplication::translate("XPlayVideo", "-/-", nullptr));
        play->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class XPlayVideo: public Ui_XPlayVideo {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XPLAYVIDEO_H
