#include "xviewer.h"
#include <QMouseEvent> 

#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QresizeEvent>
#include<QDebug>
#include<QContextMenuEvent>
#include<QGridlayout>
#include<QListwidget>
#include<QDialog>
#include<QFormLayout>
#include<QLineEdit>
#include<QMessageBox>
#include<sstream>
#include<QDir>
#include<map>
#include<vector>
#include "xcamera_config.h"
#include "xcamera_widget.h"
#include "xcamera_record.h"
#include "xplayvideo.h"

#define CAM_CONF_PATH "cams.db"
using namespace std;

//中文字符 
#define C(s) QString::fromLocal8Bit(s)
//渲染窗口
static XCameraWidget* cam_wids[16] = { 0 };

//视频录制
static vector<XCameraRecord*> records;

//存储日期时间
struct XCamVideo
{
    QString filepath;
    QDateTime datetime;
};

static map<QDate, vector<XCamVideo>> cam_videos;

void XViewer::View1()
{
    View(1);
}
void XViewer::View4()
{
    View(4);
}
void XViewer::View9()
{
    View(9);
}
void XViewer::View16()
{
    View(16);
}

void XViewer::View(int count)
{
    qDebug() << "View" << count;
    //2x2 3x3 4x4
    //确定行列 根号
    int cols = sqrt(count);
    //总窗口数量
    int wid_size = sizeof(cam_wids) / sizeof(XCameraWidget*);

    //初始化布局器
    auto lay = (QGridLayout*)ui.cams->layout();
    if (!lay)
    {
        lay = new QGridLayout();
        lay->setContentsMargins(0, 0, 0, 0);
        lay->setSpacing(2); //元素间距
        ui.cams->setLayout(lay); 
    }
    //初始化窗口
    for (int i = 0; i < count; i++)
    {
        if (!cam_wids[i])
        {
            cam_wids[i] = new XCameraWidget();
            cam_wids[i]->setStyleSheet("background-color:rgb(51,51,51) ");
        }
        lay->addWidget(cam_wids[i], i/cols, i%cols);
    }

    //清理多余的窗体
    for (int i = count; i < wid_size; i++)
    {
        if (cam_wids[i])
        {
            delete cam_wids[i];
            cam_wids[i] = nullptr; 
        }
    }
}


void XViewer::Create_push()
{

}


void XViewer::contextMenuEvent(QContextMenuEvent* event)
{
    //鼠标位置显示右键菜单
    left_menu_.exec(QCursor::pos());
    event->accept();
}


void XViewer::SetSec() {
    // 创建对话框和输入控件
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle(C("录制时间(秒)"));

    QLineEdit* recordSec = new QLineEdit(dialog);
    QPushButton* save = new QPushButton(C("确定"), dialog);

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->addWidget(recordSec);
    layout->addWidget(save);

    // 使用捕获列表捕获 dialog 和 recordSec
    connect(save, &QPushButton::clicked, this, [this, dialog, recordSec]() {
        int seconds = recordSec->text().toInt();  // 从文本转换为整数
        this->StartRecord(seconds);
        dialog->accept(); // 关闭对话框
        });

    // 显示对话框
    dialog->setLayout(layout);
    dialog->exec();
}

void XViewer::SetCam(int index)
{
    auto c = XCamearConfig::Instance();
    XCameraData data;
    if (index == -2)
    {
        QMessageBox::information(0, C("创建屏幕"), C("创建成功"));
        strcpy(data.name, QString("Screen").toLocal8Bit());
        strcpy(data.url, QString("Screen").toLocal8Bit());
        strcpy(data.sub_url, QString("Screen").toLocal8Bit());
        strcpy(data.save_path, QString("./Video").toLocal8Bit());
    }
    else if (index == -3)
    {
        QMessageBox::information(0, C("创建摄像机"), C("创建成功"));
        strcpy(data.name, QString("Camera").toLocal8Bit());
        strcpy(data.url, QString("Camera").toLocal8Bit());
        strcpy(data.sub_url, QString("Camera").toLocal8Bit());
        strcpy(data.save_path, QString("./Video").toLocal8Bit());
    }
    else
    {
        QDialog dlg(this);
        dlg.resize(800, 200);
        QFormLayout lay;
        dlg.setLayout(&lay);
        // 标题1  输入框1
        // 标题2  输入框2
        QLineEdit name_edit;
        lay.addRow(C("名称"), &name_edit);

        QLineEdit url_edit;
        lay.addRow(C("主码流"), &url_edit);

        QLineEdit sub_url_edit;
        lay.addRow(C("辅码流"), &sub_url_edit);

        QLineEdit save_path_edit;
        lay.addRow(C("保存目录 "), &save_path_edit);

        QPushButton save;
        save.setText(C("保存"));

        connect(&save, SIGNAL(clicked()), &dlg, SLOT(accept()));

        lay.addRow(" ", &save);

        //编辑 读入原数据显示
        if (index > 0)
        {
            auto cam = c->GetCam(index);
            name_edit.setText(C(cam.name));
            url_edit.setText(C(cam.url));
            sub_url_edit.setText(C(cam.sub_url));
            save_path_edit.setText(C(cam.save_path));
        }

        for (;;)
        {
            if (dlg.exec() == QDialog::Accepted)  //点击了保存， 会一直阻塞在这里
            {
                if (name_edit.text().isEmpty())
                {
                    QMessageBox::information(0, "error", C("请输入名称"));
                    continue;
                }

                if (url_edit.text().isEmpty())
                {
                    QMessageBox::information(0, "error", C("请输入主码流"));
                    continue;
                }

                if (sub_url_edit.text().isEmpty())
                {
                    QMessageBox::information(0, "error", C("请输入辅码流"));
                    continue;
                }

                if (save_path_edit.text().isEmpty())
                {
                    QMessageBox::information(0, "error", C("请输入保存目录"));
                    continue;
                }

                break;
            }
            return;
        }
        strcpy(data.name, name_edit.text().toLocal8Bit());
        strcpy(data.url, url_edit.text().toLocal8Bit());
        strcpy(data.sub_url, sub_url_edit.text().toLocal8Bit());
        strcpy(data.save_path, save_path_edit.text().toLocal8Bit());
    }
  


    if (index > 0) //修改
    {
        c->SetCam(index, data);
    }
    else         //新增数据
    {
        c->push(data);
    }
 

    RefreshCams();
}

void XViewer::SetRtmpurl()
{
    auto url = ui.input_url->text();
    qDebug() << url;
    ui.push_wid->Set_url(url.toStdString());
    return;
}

void XViewer::AddCam()
{
    ComboBoxDialog choose(this);
    if (choose.exec() == ComboBoxDialog::Accepted)
    {
        if (choose.get_select() == "Camera")
        {
            qDebug() << "choose Camera";
            SetCam(-3);
        }
        if (choose.get_select() == "Screen")
        {
            qDebug() << "choose screen";
            SetCam(-2);
        }
        if (choose.get_select() == "RTSP")
        {
            qDebug() << "choose RTSP";
            SetCam(-1);
        }
        if (choose.get_select() == "RTMP")
        {
            qDebug() << "choose RTMP";
        }

    }

}

void XViewer::SetCam()
{
    int row = ui.cam_list->currentIndex().row();
    if (row < 0)
    {
        QMessageBox::information(this, "error", C("请选择摄像机"));
        return;
    }
    SetCam(row);
}


void XViewer::DelCam( )
{
    int row = ui.cam_list->currentIndex().row();
    if (row < 0)
    {
        QMessageBox::information(this, "error", C("请选择摄像机"));
        return;
    }
    stringstream ss;
    ss << "您确认要删除摄像机" << ui.cam_list->currentItem()->text().toLocal8Bit().constData() << "吗?"; 
    if (QMessageBox::information(this, "confirm", C(ss.str().c_str()), QMessageBox::Yes, QMessageBox::No)!= QMessageBox::Yes) 
    {
        return;
    }
    XCamearConfig::Instance()->DelCam(row);
    RefreshCams();  
}

void XViewer::RefreshCams()
{
    auto c = XCamearConfig::Instance();
    ui.cam_list->clear();
    int count = c->GetCamCount();
  
    for (int i = 0; i < count; i++)
    {
        auto cam = c->GetCam(i);
        auto item = new QListWidgetItem(QIcon(":/XViewer/img/cam.png"), C(cam.name));
        ui.cam_list->addItem(item);
    }

    c->Save(CAM_CONF_PATH);   //保存到文件 

}


XViewer::XViewer(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    //去除原窗口边框
    setWindowFlags(Qt::FramelessWindowHint); 

    //布局head和body  垂直布局器
    auto vlay = new QVBoxLayout();
    //边框间距
    vlay->setContentsMargins(0, 0, 0, 0);
    //元素间距
    vlay->setSpacing(0);
    vlay->addWidget(ui.head);
    vlay->addWidget(ui.body);
    this->setLayout(vlay);

    //相机列表预览和相机预览
    //水平布局器
    auto hlay = new QHBoxLayout();
    ui.body->setLayout(hlay);
    //边框间距
    vlay->setContentsMargins(0, 0, 0, 0);
    hlay->addWidget(ui.left);       //左侧相机列表
    hlay->addWidget(ui.cams);       //右侧预览窗口
    hlay->addWidget(ui.playback_wid);    //回放窗口
    hlay->addWidget(ui.push_wid);    //推流窗口

    /////////////////////////////////////////////////////////
    ////初始化右键菜单
    //视图=》1 窗口
    //       4 窗口
    auto m = left_menu_.addMenu(C("视图"));
    auto a = m->addAction(C("1窗口"));
    connect(a, SIGNAL(triggered()), this, SLOT(View1()));
    a = m->addAction(C("4窗口"));
    connect(a, SIGNAL(triggered()), this, SLOT(View4()));
    a = m->addAction(C("9窗口"));
    connect(a, SIGNAL(triggered()), this, SLOT(View9()));
    a = m->addAction(C("16窗口"));
    connect(a, SIGNAL(triggered()), this, SLOT(View16()));
    
    //录制按钮
    //a = left_menu_.addAction(C("全部开始录制"));
    //connect(a, SIGNAL(triggered()), this, SLOT(StartRecord()));
    //a = left_menu_.addAction(C("全部停止录制"));
    //connect(a, SIGNAL(triggered()), this, SLOT(StopRecord())); 

    a = left_menu_.addAction(C("全部开始录制"));
    connect(a, SIGNAL(triggered()), this, SLOT(SetSec()));
    a = left_menu_.addAction(C("全部停止录制"));
    connect(a, SIGNAL(triggered()), this, SLOT(StopRecord()));


    //默认9窗口
    View9();

    //刷新左侧相机列表
    XCamearConfig::Instance()->Load(CAM_CONF_PATH);
    //{
    //    XCameraData cd;
    //    strcpy(cd.name, "carmera1");
    //    strcpy(cd.save_path, ".\\cameral\\");
    //    strcpy(cd.url, "rtsp"); 
    //    strcpy(cd.sub_url, " ");
    //    XCamearConfig::Instance()->push(cd);
    //}

    ui.time_list->clear();
    RefreshCams();
    
    //启动定时器渲染视频
    startTimer(30);


    //默认显示预览
    Playback();
}


static bool mouse_press = false;
static QPoint mouse_point;

//开始全部摄像头录制
void XViewer::StartRecord(int sec)
{
    qDebug() << "开始全部摄像头录制"; 
    ui.status->setText(C("录制中......"));

    //获取配置列表
    auto conf = XCamearConfig::Instance();
    int count = conf->GetCamCount();
    for (int i = 0; i < count; i++)
    {
        auto cam = conf->GetCam(i);
        stringstream ss;
        ss << cam.save_path << "/" << i << "/";
        QDir dir;
        dir.mkdir(ss.str().c_str());
        XCameraRecord* rec = new XCameraRecord();
        if (cam.name == C("Screen") && cam.sub_url == C("Screen") && cam.url == C("Screen"))
        {
            rec->set_type(1);
            rec->set_file_sec(sec);
            rec->set_save_path(ss.str());
            rec->Start();
        }
        else if(cam.name == C("Camera") && cam.sub_url == C("Camera") && cam.url == C("Camera"))  //普通rtsp接口
        {
            rec->set_type(0);
            rec->set_save_path(ss.str());
            rec->set_file_sec(sec);
            rec->Start();
        }
        else
        {
            rec->set_type(2);
            rec->set_rtsp_url(cam.url);
            rec->set_save_path(ss.str());
            rec->set_file_sec(sec);
            rec->Start();
        }

        records.push_back(rec); 

    }
    // 创建录制目录 
    //分别开始录制线程
}
//停止全部摄像头录制 
void XViewer::StopRecord()
{
    ui.status->setText(C("监控中......"));
    for (auto rec : records)
    {
        rec->Stop();
        delete rec;
    }
    records.clear();
}

void XViewer::mouseMoveEvent(QMouseEvent* ev)
{
    if (!mouse_press)
    {
        QWidget::mouseMoveEvent(ev);
        return;
    }
    this->move(ev->globalPos() - mouse_point);
}
void XViewer::mousePressEvent(QMouseEvent* ev)
{
    if (ev->button() == Qt::LeftButton)
    {
        mouse_press = true;
        mouse_point = ev->pos();
    }
}
void XViewer::mouseReleaseEvent(QMouseEvent* ev)
{
    mouse_press = false;
}


void XViewer::MaxWindow()
{
    ui.max->setVisible(false);
    ui.normal->setVisible(true);
    showMaximized();
}
void XViewer::NormalWindow()
{
    ui.max->setVisible(true);
    ui.normal->setVisible(false);
    showNormal();
}

//窗口大小发生变化
void XViewer::resizeEvent(QResizeEvent* ev)
{
    int x = width() - ui.head_button->width();
    int y = ui.head_button->y();
    ui.head_button->move(x, y);
    auto psize = ui.playback_wid->size();
    auto tsize = ui.time_list->size();
    auto csize = ui.cam_list->size();
    ui.time_list->resize(tsize.width(), psize.height());
    ui.cal->resize(psize.width() - tsize.width(), psize.height());
    ui.cam_list->resize(csize.width(), psize.height());
    
}

void XViewer::timerEvent(QTimerEvent* ev)
{
    //总窗口数量
    int wid_size = sizeof(cam_wids) / sizeof(XCameraWidget*);
    for (int i = 0; i < wid_size; i++)
    {
        if (cam_wids[i])
        {
            //渲染多窗口视频
            cam_wids[i]->Draw();
        }
    }
}

void XViewer::Preview()       //预览界面
{
    ui.cams->show();
    ui.playback_wid->hide();
    ui.push_wid->close();
    ui.preview ->setChecked(true);

}
void XViewer::Playback()      //回放界面
{
    ui.cams->hide();
    ui.playback_wid->show();
    ui.push_wid->close();
    ui.playback->setChecked(true); 
}

void XViewer::PlayPush()      //推流界面
{
    ui.cams->hide();
    ui.playback_wid->hide();
    ui.push_stream->setChecked(true);
    ui.push_wid->show();
    ui.input_url->setText("rtmp://192.168.1.125/live");
    SetRtmpurl();
}

void XViewer::SetDecodeType(int index)
{
    qDebug() << C("当前选中的解码方式是：" )<< index;
    decodetype_ = index;
    if (index == 0) {
        XCamearConfig::Instance()->Setdecodetype(false);
    }
    else if (index == 1) {
        XCamearConfig::Instance()->Setdecodetype(true);
    }
}


void XViewer::SetFilterType(QString name)
{
    qDebug() << C("当前选中的滤镜是：") << name;
    std::string nameStdString = name.toLocal8Bit().constData();
    ui.push_wid->Set_filter(nameStdString);
}


void XViewer::SelectCamera(QModelIndex index)    //选择摄像机
{
    qDebug() << "SelectCamera" << index.row();
    //通过配置文件获取到相机
    auto conf = XCamearConfig::Instance();
    auto cam = conf->GetCam(index.row());        //获取相机参数：row()
    if (cam.name[0] == '\0')
    {
        return;
    }
    //相机视频存储路径
    stringstream ss;
    ss << cam.save_path << "/" << index.row() << "/";
    qDebug() << ss.str().c_str();

    //遍历此目录
    QDir dir(C(ss.str().c_str()));
    if (!dir.exists())
    {
        ui.cal->ClearDate();
        //重新显示日期
        ui.cal->showNextMonth();
        ui.cal->showPreviousMonth();
        return;
    }


    //获取目录下文件列表
    QStringList filters;
    filters << "*.mp4" << "*.avi";
    dir.setNameFilters(filters);   //筛选

    //清理其他相机的数据
    ui.cal->ClearDate(); 

    //所有文件列表
    auto files = dir.entryInfoList();
    ui.cal->ClearDate();
    ui.time_list->clear();
    cam_videos.clear();
    for (auto file : files)
    {
        qDebug() << file.fileName();
        QString filename = file.fileName();
        //"cam_2024_04_24_15_56_43.mp4"
        //去掉cam_和.mp4
        auto tmp = filename.left(filename.size() - 4);
        tmp = tmp.right(tmp.length() - 4);
        //2024_04_24_15_56_43
        auto dt = QDateTime::fromString(tmp,  "yyyy_MM_dd_hh_mm_ss");
        ui.cal->AddDate(dt.date());

        XCamVideo video;
        video.datetime = dt;
        video.filepath = file.absoluteFilePath();
        cam_videos[dt.date()].push_back(video);
    }
    //重新显示日期
    ui.cal->showNextMonth();
    ui.cal->showPreviousMonth();
 
}
void XViewer::SelectDate(QDate date)             //选择日期
{
    qDebug() << "SelectDate" << date.toString();
    auto dates = cam_videos[date];
    ui.time_list->clear();
    for (auto d : dates)
    {
        auto item = new QListWidgetItem(d.datetime.time().toString());

        //item 添加自定义数据 文件路径
        item->setData(Qt::UserRole, d.filepath); 
        ui.time_list->addItem(item); 
    }
}
void XViewer::PlayVideo(QModelIndex index)       //选择时间播放视频
{
    qDebug() << "PlayVideo" << index.row();
    auto item = ui.time_list->currentItem();
    if (!item) return;
    QString path = item->data(Qt::UserRole).toString();
    qDebug() << path;
    XPlayVideo play;
    if (decodetype_ == 1)
        play.Sethw(true, true);
    if (decodetype_ == 2)
        play.Sethw(true, false);
    play.Open(path.toLocal8Bit());
    play.exec();

}
