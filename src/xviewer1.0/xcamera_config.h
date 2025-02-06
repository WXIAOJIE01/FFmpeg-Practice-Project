#pragma once
#include<vector>
#include<mutex>

struct XCameraData
{
	char name[1024] = { 0 };
	char url[4096] = { 0 };		//�����������
	char sub_url[4096] = { 0 };  //�����������
	char save_path[4096] = { 0 }; //��Ƶ¼�ƴ��Ŀ¼
};

class XCamearConfig
{
public:
	//Ψһ����ʵ�� ����ģʽ
	static XCamearConfig* Instance()
	{
		static XCamearConfig xc;
		return &xc; 
	}

	//��������� �̰߳�ȫ
	void push(const XCameraData& data);

	/// <summary>
	/// ��ȡ����ͷ
	/// </summary>
	/// <param name="index">����ͷ���� ��0��ʼ</param>
	/// <returns>���ؿն��󣬿����ж�nameΪ���ַ���</returns>
	XCameraData GetCam(int index);

	/// <summary>
	/// �޸����������
	/// </summary>
	/// <param name="index"></param>
	/// <param name="data"></param>
	/// <returns></returns>
	bool SetCam(int index, const XCameraData& data);

	bool DelCam(int index);

	/// <summary>
	/// ��ȡ�������
	/// </summary>
	/// <returns></returns>
	int GetCamCount();

	/// <summary>
	/// �洢�����ļ�
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	bool Save(const char* path);


	/// <summary>
	/// ��ȡ���ã�����cams 
	/// </summary>
	/// <param name="path"></param>
	/// <returns></returns>
	bool Load(const char* path);

	//����Ӳ����
	void Setdecodetype(bool is_hw) { is_hw_ = is_hw; }

	//�����Ƿ�֧��Ӳ������
	bool is_hw() { return is_hw_; }
private:
	XCamearConfig() {}		//����˽�� ����ģʽ
	bool is_hw_ = false;     //�Ƿ�֧��Ӳ����
	std::vector<XCameraData> cams_;
	std::mutex mux_;

};

