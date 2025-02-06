 #include "xviewer.h"
#include"xplayvideo.h"
#include <QtWidgets/QApplication>
#include<Qdebug>
#include<QDir>
using namespace std;

#include "xcamera_config.h"
#define TEST_CAM_PATH "test.db"
#include "xcamera_record.h"



int main(int argc, char *argv[])
{
    
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    QApplication a(argc, argv);
    
    //XPlayVideo pv;
    //pv.Open("./source/IronMan.mp4");
    //pv.exec();
    //return 0;

    XViewer w;
    w.show();
    auto re = a.exec();
    return re;

}
