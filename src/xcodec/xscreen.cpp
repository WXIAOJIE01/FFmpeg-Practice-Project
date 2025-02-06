#include <d3d9.h>
#include <fstream>
#include "xscreen.h"
#include "xtools.h"
#include "xvideo_view.h"
#include"xtools.h"

using namespace std;
#include<string>

#pragma comment(lib, "d3d9.lib")

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

bool Xscreen::Init()
{
	//创建directx3D对象
	SAFE_RELEASE(d3d_)
	if (!d3d_)
	{
		d3d_ = Direct3DCreate9(D3D_SDK_VERSION);
		if (!d3d_)
		{
			cerr << "create_d3d_ error" << endl;
			return false;
		}
	}
	//2创建显卡设备对象
	SAFE_RELEASE(device_);
	if (!device_)
	{
		D3DPRESENT_PARAMETERS pa;  //用来打开显卡设备的参数
		//只配置想要的参数
		ZeroMemory(&pa, sizeof(pa));

		pa.Windowed = true;
		pa.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;  //锁定后背缓冲区
		pa.SwapEffect = D3DSWAPEFFECT_DISCARD;          //将后背缓冲区刷新到前端的模式（直接清除)
		pa.hDeviceWindow = GetDesktopWindow();          //设置显示句柄-桌面

		//对d3d对象创建设备   适配器(默认)/设备类型(硬件加速的设备)/聚焦的窗口(截屏设置为0)/用硬件来处理顶点信息/打开参数/传出参数(设备)
		d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 0,
			D3DCREATE_HARDWARE_VERTEXPROCESSING, &pa, &device_);

		if (!device_)
		{
			cerr << "create_device error" << endl;
			return false;
		}
	}

	//3 创建离屏表面
	SAFE_RELEASE(sur_);
	screen_w_ = GetSystemMetrics(SM_CXSCREEN);   //获取屏幕长
	screen_h_ = GetSystemMetrics(SM_CYSCREEN);   //获取屏幕宽
	if (!sur_)
	{
		device_->CreateOffscreenPlainSurface(screen_w_, screen_h_,
			D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &sur_, 0);

		if (!sur_)
		{
			cerr << "create_surface error" << endl;
			return false;
		}
	}
	return true;
}


void Xscreen::close()
{
	SAFE_RELEASE(d3d_);
	SAFE_RELEASE(device_);
	SAFE_RELEASE(sur_);
}
