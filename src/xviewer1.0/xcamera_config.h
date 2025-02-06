#pragma once
#include<vector>
#include<mutex>

struct XCameraData
{
	char name[1024] = { 0 };
	char url[4096] = { 0 };		//摄像机主码流
	char sub_url[4096] = { 0 };  //摄像机辅码流
	char save_path[4096] = { 0 }; //视频录制存放目录
};

class XCamearConfig
{
public:
	//唯一对象实例 单件模式
	static XCamearConfig* Instance()
	{
		static XCamearConfig xc;
		return &xc; 
	}

	//插入摄像机 线程安全
	void push(const XCameraData& data);

	/// <summary>
	/// 获取摄像头
	/// </summary>
	/// <param name="index">摄像头索引 从0开始</param>
	/// <returns>返回空对象，可以判断name为空字符串</returns>
	XCameraData GetCam(int index);

	/// <summary>
	/// 修改摄像机数据
	/// </summary>
	/// <param name="index"></param>
	/// <param name="data"></param>
	/// <returns></returns>
	bool SetCam(int index, const XCameraData& data);

	bool DelCam(int index);

	/// <summary>
	/// 获取相机数量
	/// </summary>
	/// <returns></returns>
	int GetCamCount();

	/// <summary>
	/// 存储配置文件
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	bool Save(const char* path);


	/// <summary>
	/// 读取配置，存入cams 
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	bool Load(const char* path);

	//设置硬编码
	void Setdecodetype(bool is_hw) { is_hw_ = is_hw; }

	//返回是否支持硬件解码
	bool is_hw() { return is_hw_; }
private:
	XCamearConfig() {}		//构造私有 单件模式
	bool is_hw_ = false;     //是否支持硬解码
	std::vector<XCameraData> cams_;
	std::mutex mux_;

};

