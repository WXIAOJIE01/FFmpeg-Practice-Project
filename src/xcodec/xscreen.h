#pragma once
#include "xtools.h"
class XCODEC_API Xscreen
{
public:
	/// <summary>
	/// ≥ı ºªØDirectX
	/// </summary>
	/// <returns></returns>
	bool Init();
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	IDirect3DDevice9* get_device() { return device_; }
	IDirect3D9* get_d() { return d3d_; }
	IDirect3DSurface9* get_sur() { return sur_; }

	int get_width() { return screen_w_; }
	int get_height() { return screen_h_; }

	void close();
protected:
	IDirect3D9* d3d_ = nullptr;
	IDirect3DDevice9* device_ = nullptr;
	IDirect3DSurface9* sur_ = nullptr;
	int screen_w_ = 0;
	int screen_h_ = 0;
	
};

