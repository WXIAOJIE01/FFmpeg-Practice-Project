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
	//����directx3D����
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
	//2�����Կ��豸����
	SAFE_RELEASE(device_);
	if (!device_)
	{
		D3DPRESENT_PARAMETERS pa;  //�������Կ��豸�Ĳ���
		//ֻ������Ҫ�Ĳ���
		ZeroMemory(&pa, sizeof(pa));

		pa.Windowed = true;
		pa.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;  //�����󱳻�����
		pa.SwapEffect = D3DSWAPEFFECT_DISCARD;          //���󱳻�����ˢ�µ�ǰ�˵�ģʽ��ֱ�����)
		pa.hDeviceWindow = GetDesktopWindow();          //������ʾ���-����

		//��d3d���󴴽��豸   ������(Ĭ��)/�豸����(Ӳ�����ٵ��豸)/�۽��Ĵ���(��������Ϊ0)/��Ӳ������������Ϣ/�򿪲���/��������(�豸)
		d3d_->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 0,
			D3DCREATE_HARDWARE_VERTEXPROCESSING, &pa, &device_);

		if (!device_)
		{
			cerr << "create_device error" << endl;
			return false;
		}
	}

	//3 ������������
	SAFE_RELEASE(sur_);
	screen_w_ = GetSystemMetrics(SM_CXSCREEN);   //��ȡ��Ļ��
	screen_h_ = GetSystemMetrics(SM_CYSCREEN);   //��ȡ��Ļ��
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
