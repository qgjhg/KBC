
// MFCKinect.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include <Kinect.h>
#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <string>//字符串
#include <conio.h>
#include <windows.h>

using namespace std;
using namespace cv;



// CMFCKinectApp: 
// 有关此类的实现，请参阅 MFCKinect.cpp
//

class CMFCKinectApp : public CWinApp
{
public:
	CMFCKinectApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CMFCKinectApp theApp;




// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

class CBodyBasics
{
	//kinect 2.0 的深度空间的高*宽是 424 * 512，在官网上有说明
	static const int        cDepthWidth = 512;
	static const int        cDepthHeight = 424;

	static const int        cColorWidth = 1920;   //彩色图的大小  
	static const int        cColorHeight = 1080;

public:
	CBodyBasics();
	~CBodyBasics();
	void                    Update();//获得骨架、背景二值图和深度、彩图等等信息
	void                 InitializeDefaultSensor( int x);//用于初始化kinect

	//void                    ProcessDepth(const UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth);   //处理得到的深度图数据  
	//void                    ProcessColor(RGBQUAD* pBuffer, int nWidth, int nHeight);   //处理得到的彩色图数据  

private:
	IKinectSensor*          m_pKinectSensor;//kinect源
	ICoordinateMapper*      m_pCoordinateMapper;//用于坐标变换
	IBodyFrameReader*       m_pBodyFrameReader;//用于骨架数据读取
	IDepthFrameReader*      m_pDepthFrameReader;//用于深度数据读取
	RGBQUAD*                m_pDepthRGBX;
	IColorFrameReader*      m_pColorFrameReader;// Color reader  
	RGBQUAD*                m_pColorRGBX;
												//!!IBodyIndexFrameReader*  m_pBodyIndexFrameReader;//用于背景二值图读取

												//通过获得到的信息，把骨架和背景二值图画出来
	void                    ProcessBody(int nBodyCount, IBody** ppBodies);
	//画骨架函数
	void DrawBone(int pbody);
	//画手的状态函数
	void DrawHandState(const DepthSpacePoint depthSpacePosition, HandState handState);

	//显示图像的Mat
	cv::Mat skeletonImg;
	cv::Mat depthImg;
	cv::Mat depthcolorImg;
};

void outfile(string x, CString filepath);

void outpoint(int* pointSet, CString filepath);

void newfile();

void Filtering(DepthSpacePoint* pointSet, int pbody);

//void LineKinectreader(int Lineindex);
class CVplayerclass {
	static const int        cDepthWidth = 512;
	static const int        cDepthHeight = 424;

public:
	void chushihua();//初始化
	void update(double point[51]);
	void process(double point[51]);
	void cvplay(double x, double y);
	cv::Mat cvplayer;
};

//void split(const char * str, const char * deli, vector<string> *list);//分隔字符串