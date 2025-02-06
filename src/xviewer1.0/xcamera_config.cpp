#include "xcamera_config.h"
using namespace std;
#include <fstream>

void XCamearConfig::push(const XCameraData& data)
{
	unique_lock<mutex> lock(mux_);
	cams_.push_back(data); 
}

XCameraData XCamearConfig::GetCam(int index)
{
	unique_lock<mutex> lock(mux_);
	if (index<0 || index>cams_.size())
		return XCameraData(); 
	return cams_[index]; 
}

int XCamearConfig::GetCamCount()
{
	unique_lock<mutex> lock(mux_);
	return cams_.size();
}

bool XCamearConfig::SetCam(int index, const XCameraData& data)
{
	unique_lock<mutex> lock(mux_);
	if (index<0 || index>cams_.size())
		return false;
	cams_[index] = data;
	return true;
}

bool XCamearConfig::DelCam(int index)
{
	unique_lock<mutex> lock(mux_);
	if (index<0 || index>cams_.size())
		return false;
	cams_.erase(cams_.begin() + index);
	return true; 
}

bool XCamearConfig::Save(const char* path)
{
	if (!path) return false;
	ofstream ofs(path, ios::binary);
	if (!ofs) return false;
	unique_lock<mutex> lock(mux_);
	for (auto cam : cams_)
	{
		ofs.write((char*)&cam, sizeof(cam)); 
	}
	ofs.close(); 
	return true;
}

bool XCamearConfig::Load(const char* path)
{
	if (!path) return false;
	ifstream ifs(path, ios::binary);
	if (!ifs) return false;
	XCameraData data;
	unique_lock<mutex> lock(mux_);
	cams_.clear();
	for (;;)
	{
		ifs.read((char*)&data, sizeof(data));
		if (ifs.gcount() != sizeof(data))
		{
			ifs.close();
			return true;
		}

		cams_.push_back(data);
	}
	ifs.close();
	return true;
}