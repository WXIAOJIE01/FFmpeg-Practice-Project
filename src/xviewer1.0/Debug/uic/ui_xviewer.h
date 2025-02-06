/********************************************************************************
** Form generated from reading UI file 'xviewer.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XVIEWER_H
#define UI_XVIEWER_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "xcalendar.h"
#include "xpush_stream_widget.h"

QT_BEGIN_NAMESPACE

class Ui_XViewerClass
{
public:
    QWidget *head;
    QWidget *head_button;
    QPushButton *min;
    QPushButton *close;
    QPushButton *max;
    QPushButton *normal;
    QLabel *status;
    QPushButton *preview;
    QPushButton *playback;
    QPushButton *push_stream;
    QComboBox *comboBox;
    QWidget *body;
    QWidget *cams;
    QWidget *left;
    QListWidget *cam_list;
    QPushButton *add_cam;
    QPushButton *add_cam_2;
    QPushButton *add_cam_3;
    QWidget *playback_wid;
    QListWidget *time_list;
    XCalendar *cal;
    XPushStreamWidget *push_wid;
    QLabel *push_status;
    QLineEdit *input_url;
    QLabel *push_lable;
    QComboBox *filter_type;

    void setupUi(QWidget *XViewerClass)
    {
        if (XViewerClass->objectName().isEmpty())
            XViewerClass->setObjectName(QString::fromUtf8("XViewerClass"));
        XViewerClass->resize(1067, 762);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/XViewer/img/xv.ico"), QSize(), QIcon::Normal, QIcon::Off);
        XViewerClass->setWindowIcon(icon);
        XViewerClass->setStyleSheet(QString::fromUtf8("/* \346\214\211\351\222\256\346\240\267\345\274\217*/\n"
"QPushButton:hover\n"
"{\n"
"background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(0, 102, 184, 255), stop:0.495 rgba(39, 39, 39, 255), stop:0.505 rgba(39, 39, 39, 255), stop:1 rgba(0, 102, 184, 255));\n"
"border:none;\n"
"border-radius:5px;\n"
"color:rgb(255, 255, 255);\n"
"font:75 12pt \"\351\273\221\344\275\223\";\n"
"}\n"
"\n"
"QPushButton:!hover\n"
"{\n"
"background-color:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(0, 50, 150, 255), stop:0.495 rgba(0, 102, 184, 255), stop:0.505 rgba(0, 102, 184, 255), stop:1 rgba(39, 39, 39, 255) );\n"
"border:none;\n"
"border-radius:8px;\n"
"color:rgb(255, 255, 255);\n"
"font:75 12pt \"\351\273\221\344\275\223\";\n"
"}\n"
"\n"
"/*\347\212\266\346\200\201*/\n"
"#status\n"
"{\n"
"color:rgb(255, 255, 255);\n"
"font:75 13pt \"Adobe Arabic\";\n"
"} \n"
"\n"
"\n"
"/*\351\241\266\351\203\250\346\240\267\345\274\217*/\n"
"#head\n"
"{\n"
" background-color:#3c3c3a;\n"
"}\n"
"#l"
                        "ogo\n"
"{\n"
"	background-image: url(:/XViewer/img/logo_150_40.png);\n"
"}\n"
"#close\n"
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
"\n"
"/* body \346\274\224\347\244\272 */\n"
"#body\n"
"{\n"
"  background-color:#212121 ;\n"
"}\n"
"#left\n"
"{\n"
"  background-color:#252525;\n"
"}\n"
"#cams\n"
"{\n"
"background-color:#1e1e1e;\n"
"}\n"
"\n"
"/* tab \350\217\234\345\215\225\346\214\211\351\222\256\345\210\207\346\215\242\346\240\267\345\274\217*/\n"
"#preview\n"
"{\n"
"background-color:rgb(50, 50, 50);\n"
"} \n"
"#playback\n"
"{\n"
"background-color:rgb(50, 50, 50);\n"
"} \n"
"#push_stream\n"
"{\n"
"background-color:rgb(50, 50, 50);\n"
"} \n"
"\n"
"#preview:checked\n"
"{\n"
"background-color:rgb(0, 102, 184);\n"
"} \n"
"#playback:checked\n"
"{\n"
"background-color:rg"
                        "b(0, 102, 184);\n"
"}\n"
"#push_stream:checked\n"
"{\n"
"background-color:rgb(0, 102, 184);\n"
"}"));
        head = new QWidget(XViewerClass);
        head->setObjectName(QString::fromUtf8("head"));
        head->setGeometry(QRect(0, 0, 800, 50));
        head->setMaximumSize(QSize(16777215, 50));
        head->setStyleSheet(QString::fromUtf8(""));
        head_button = new QWidget(head);
        head_button->setObjectName(QString::fromUtf8("head_button"));
        head_button->setGeometry(QRect(670, 10, 120, 31));
        min = new QPushButton(head_button);
        min->setObjectName(QString::fromUtf8("min"));
        min->setGeometry(QRect(10, 0, 21, 20));
        min->setStyleSheet(QString::fromUtf8(""));
        min->setFlat(true);
        close = new QPushButton(head_button);
        close->setObjectName(QString::fromUtf8("close"));
        close->setGeometry(QRect(90, 0, 20, 20));
        close->setFlat(true);
        max = new QPushButton(head_button);
        max->setObjectName(QString::fromUtf8("max"));
        max->setGeometry(QRect(50, 0, 20, 20));
        max->setFlat(true);
        normal = new QPushButton(head_button);
        normal->setObjectName(QString::fromUtf8("normal"));
        normal->setGeometry(QRect(50, 0, 20, 20));
        normal->setFlat(true);
        min->raise();
        close->raise();
        normal->raise();
        max->raise();
        status = new QLabel(head);
        status->setObjectName(QString::fromUtf8("status"));
        status->setGeometry(QRect(560, 20, 101, 21));
        preview = new QPushButton(head);
        preview->setObjectName(QString::fromUtf8("preview"));
        preview->setGeometry(QRect(170, 20, 91, 31));
        preview->setCheckable(true);
        preview->setChecked(true);
        preview->setAutoExclusive(true);
        playback = new QPushButton(head);
        playback->setObjectName(QString::fromUtf8("playback"));
        playback->setGeometry(QRect(270, 20, 91, 31));
        playback->setCheckable(true);
        playback->setChecked(false);
        playback->setAutoExclusive(true);
        push_stream = new QPushButton(head);
        push_stream->setObjectName(QString::fromUtf8("push_stream"));
        push_stream->setGeometry(QRect(370, 20, 91, 31));
        push_stream->setCheckable(true);
        push_stream->setChecked(false);
        push_stream->setAutoExclusive(true);
        comboBox = new QComboBox(head);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setGeometry(QRect(480, 30, 69, 22));
        body = new QWidget(XViewerClass);
        body->setObjectName(QString::fromUtf8("body"));
        body->setGeometry(QRect(0, 50, 801, 611));
        cams = new QWidget(body);
        cams->setObjectName(QString::fromUtf8("cams"));
        cams->setGeometry(QRect(230, 10, 561, 541));
        left = new QWidget(body);
        left->setObjectName(QString::fromUtf8("left"));
        left->setGeometry(QRect(19, 10, 200, 600));
        left->setMaximumSize(QSize(200, 16777215));
        cam_list = new QListWidget(left);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/XViewer/img/cam.png"), QSize(), QIcon::Normal, QIcon::Off);
        QListWidgetItem *__qlistwidgetitem = new QListWidgetItem(cam_list);
        __qlistwidgetitem->setIcon(icon1);
        cam_list->setObjectName(QString::fromUtf8("cam_list"));
        cam_list->setGeometry(QRect(0, 49, 200, 951));
        cam_list->setDragEnabled(true);
        cam_list->setIconSize(QSize(30, 30));
        add_cam = new QPushButton(left);
        add_cam->setObjectName(QString::fromUtf8("add_cam"));
        add_cam->setGeometry(QRect(5, 5, 50, 40));
        add_cam_2 = new QPushButton(left);
        add_cam_2->setObjectName(QString::fromUtf8("add_cam_2"));
        add_cam_2->setGeometry(QRect(80, 5, 50, 40));
        add_cam_3 = new QPushButton(left);
        add_cam_3->setObjectName(QString::fromUtf8("add_cam_3"));
        add_cam_3->setGeometry(QRect(150, 5, 50, 40));
        playback_wid = new QWidget(body);
        playback_wid->setObjectName(QString::fromUtf8("playback_wid"));
        playback_wid->setGeometry(QRect(230, 10, 571, 541));
        time_list = new QListWidget(playback_wid);
        new QListWidgetItem(time_list);
        time_list->setObjectName(QString::fromUtf8("time_list"));
        time_list->setGeometry(QRect(0, 0, 150, 601));
        cal = new XCalendar(playback_wid);
        cal->setObjectName(QString::fromUtf8("cal"));
        cal->setGeometry(QRect(160, 0, 401, 541));
        push_wid = new XPushStreamWidget(body);
        push_wid->setObjectName(QString::fromUtf8("push_wid"));
        push_wid->setGeometry(QRect(390, 10, 411, 591));
        push_status = new QLabel(push_wid);
        push_status->setObjectName(QString::fromUtf8("push_status"));
        push_status->setGeometry(QRect(160, 190, 451, 141));
        push_status->setStyleSheet(QString::fromUtf8("font: 25 36pt \"Adobe \345\256\213\344\275\223 Std L\";"));
        push_status->setLineWidth(1);
        input_url = new QLineEdit(push_wid);
        input_url->setObjectName(QString::fromUtf8("input_url"));
        input_url->setGeometry(QRect(100, 30, 211, 20));
        push_lable = new QLabel(push_wid);
        push_lable->setObjectName(QString::fromUtf8("push_lable"));
        push_lable->setGeometry(QRect(35, 25, 71, 31));
        filter_type = new QComboBox(push_wid);
        filter_type->addItem(QString());
        filter_type->addItem(QString());
        filter_type->addItem(QString());
        filter_type->addItem(QString());
        filter_type->setObjectName(QString::fromUtf8("filter_type"));
        filter_type->setGeometry(QRect(330, 30, 69, 22));

        retranslateUi(XViewerClass);
        QObject::connect(close, SIGNAL(clicked()), XViewerClass, SLOT(close()));
        QObject::connect(min, SIGNAL(clicked()), XViewerClass, SLOT(showMinimized()));
        QObject::connect(max, SIGNAL(clicked()), XViewerClass, SLOT(MaxWindow()));
        QObject::connect(normal, SIGNAL(clicked()), XViewerClass, SLOT(NormalWindow()));
        QObject::connect(add_cam, SIGNAL(clicked()), XViewerClass, SLOT(AddCam()));
        QObject::connect(add_cam_2, SIGNAL(clicked()), XViewerClass, SLOT(SetCam()));
        QObject::connect(add_cam_3, SIGNAL(clicked()), XViewerClass, SLOT(DelCam()));
        QObject::connect(preview, SIGNAL(clicked()), XViewerClass, SLOT(Preview()));
        QObject::connect(playback, SIGNAL(clicked()), XViewerClass, SLOT(Playback()));
        QObject::connect(cam_list, SIGNAL(clicked(QModelIndex)), XViewerClass, SLOT(SelectCamera(QModelIndex)));
        QObject::connect(cal, SIGNAL(clicked(QDate)), XViewerClass, SLOT(SelectDate(QDate)));
        QObject::connect(time_list, SIGNAL(activated(QModelIndex)), XViewerClass, SLOT(PlayVideo(QModelIndex)));
        QObject::connect(push_stream, SIGNAL(clicked()), XViewerClass, SLOT(PlayPush()));
        QObject::connect(input_url, SIGNAL(returnPressed()), XViewerClass, SLOT(SetRtmpurl()));
        QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)), XViewerClass, SLOT(SetDecodeType(int)));
        QObject::connect(filter_type, SIGNAL(activated(QString)), XViewerClass, SLOT(SetFilterType(QString)));

        QMetaObject::connectSlotsByName(XViewerClass);
    } // setupUi

    void retranslateUi(QWidget *XViewerClass)
    {
        XViewerClass->setWindowTitle(QCoreApplication::translate("XViewerClass", "XViewer", nullptr));
        min->setText(QString());
        close->setText(QString());
        max->setText(QString());
        normal->setText(QString());
        status->setText(QCoreApplication::translate("XViewerClass", "\347\233\221\346\216\247\344\270\255......", nullptr));
        preview->setText(QCoreApplication::translate("XViewerClass", "\351\242\204\350\247\210", nullptr));
        playback->setText(QCoreApplication::translate("XViewerClass", "\345\233\236\346\224\276", nullptr));
        push_stream->setText(QCoreApplication::translate("XViewerClass", "\346\216\250\346\265\201", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("XViewerClass", "\350\275\257\350\247\243\347\240\201", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("XViewerClass", "\347\241\254\350\247\243+\350\275\257\346\270\262", nullptr));
        comboBox->setItemText(2, QCoreApplication::translate("XViewerClass", "\347\241\254\350\247\243\347\240\201", nullptr));


        const bool __sortingEnabled = cam_list->isSortingEnabled();
        cam_list->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = cam_list->item(0);
        ___qlistwidgetitem->setText(QCoreApplication::translate("XViewerClass", "camera_test", nullptr));
        cam_list->setSortingEnabled(__sortingEnabled);

        add_cam->setText(QCoreApplication::translate("XViewerClass", "\346\226\260\345\242\236", nullptr));
        add_cam_2->setText(QCoreApplication::translate("XViewerClass", "\344\277\256\346\224\271", nullptr));
        add_cam_3->setText(QCoreApplication::translate("XViewerClass", "\345\210\240\351\231\244", nullptr));

        const bool __sortingEnabled1 = time_list->isSortingEnabled();
        time_list->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem1 = time_list->item(0);
        ___qlistwidgetitem1->setText(QCoreApplication::translate("XViewerClass", "20:30:59", nullptr));
        time_list->setSortingEnabled(__sortingEnabled1);

        push_status->setText(QCoreApplication::translate("XViewerClass", "\346\213\226\345\210\260\346\255\244\345\244\204\350\277\233\350\241\214\346\216\250\346\265\201", nullptr));
        push_lable->setText(QCoreApplication::translate("XViewerClass", "RTMP\345\234\260\345\235\200\357\274\232", nullptr));
        filter_type->setItemText(0, QCoreApplication::translate("XViewerClass", "\345\216\237\345\247\213", nullptr));
        filter_type->setItemText(1, QCoreApplication::translate("XViewerClass", "\347\243\250\347\232\256", nullptr));
        filter_type->setItemText(2, QCoreApplication::translate("XViewerClass", "\347\276\216\347\231\275", nullptr));
        filter_type->setItemText(3, QCoreApplication::translate("XViewerClass", "\350\276\271\347\274\230", nullptr));

    } // retranslateUi

};

namespace Ui {
    class XViewerClass: public Ui_XViewerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XVIEWER_H
