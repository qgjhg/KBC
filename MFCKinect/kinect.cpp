//

#include "stdafx.h"
#include "MFCKinect.h"
#include "MFCKinectDlg.h"
#include "jointfliter.h"
#include <iostream>

int IsRecord = 0;
extern CString savefile;
int deparray[6][5][51];//body,帧数,point
int outdeparray[51];
int lastoutputarray[6][51];
long lines=1;
extern int pCheckbox[25];
extern int gesture;
extern int onpicture;
extern int maxppbody;

float testout[51];

void CBodyBasics::InitializeDefaultSensor( int x )
{
	//用于判断每次读取操作的成功与否
	HRESULT hr;
	//搜索kinect
	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr)) {
		x=0;
	}

	//找到kinect设备
	if (m_pKinectSensor != NULL)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;//读取骨架
		IDepthFrameSource* pDepthFrameSource = NULL;//读取深度信息
		IColorFrameSource* pColorFrameSource = NULL;//读取彩图信息
		//!!IBodyIndexFrameSource* pBodyIndexFrameSource = NULL;//读取背景二值图

															//打开kinect
		hr = m_pKinectSensor->Open();

		//coordinatemapper
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		//bodyframe
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		//depth frame
		if (SUCCEEDED(hr)) {
			hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
		}

		if (SUCCEEDED(hr)) {
			hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
		}

		//color frame
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);  
		}

		//body index frame
		/*if (SUCCEEDED(hr)) {
			hr = m_pKinectSensor->get_BodyIndexFrameSource(&pBodyIndexFrameSource);
		}

		if (SUCCEEDED(hr)) {
			hr = pBodyIndexFrameSource->OpenReader(&m_pBodyIndexFrameReader);
		}*/

		SafeRelease(pBodyFrameSource);
		SafeRelease(pDepthFrameSource);
		/*SafeRelease(pBodyIndexFrameSource);*/
		x = 1;
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		x=0;
	}

	//skeletonImg,用于画骨架的MAT
	skeletonImg.create(cDepthHeight, cDepthWidth, CV_8UC3);
	skeletonImg.setTo(0);

	//depthImg,用于画深度信息的MAT
	depthImg.create(cDepthHeight, cDepthWidth, CV_8UC1);
	depthImg.setTo(0);

	//用于画3D模拟的MAT
	depthcolorImg.create(cDepthHeight, cDepthWidth, CV_8UC1);
	depthcolorImg.setTo(0);

	//if (SUCCEEDED(hr) && m_pKinectSensor) {
	//	return S_OK;
	//}

	//return hr;
}


/// Main processing function
void CBodyBasics::Update()
{
	//每次先清空skeletonImg
	skeletonImg.setTo(0);
	depthImg.setTo(0);
	depthcolorImg.setTo(0);

	//如果丢失了kinect，则不继续操作
	if (!m_pBodyFrameReader)
	{
		return;
	}

	IBodyFrame* pBodyFrame = NULL;//骨架信息
	IDepthFrame* pDepthFrame = NULL;//深度信息
	IColorFrame* pColorFrame = NULL;//彩图信息
	//!!IBodyIndexFrame* pBodyIndexFrame = NULL;//背景二值图

											//记录每次操作的成功与否
	HRESULT hr = S_OK;

	//---------------------------------------背景二值图----------------------------------------------
	//if (SUCCEEDED(hr)) {
	//	hr = m_pBodyIndexFrameReader->AcquireLatestFrame(&pBodyIndexFrame);//获得背景二值图信息
	//}
	//if (SUCCEEDED(hr)) {
	//	BYTE *bodyIndexArray = new BYTE[cDepthHeight * cDepthWidth];//背景二值图是8为uchar，有人是黑色，没人是白色
	//	pBodyIndexFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, bodyIndexArray);

	//	//把背景二值图画到MAT里
	//	uchar* skeletonData = (uchar*)skeletonImg.data;
	//	for (int j = 0; j < cDepthHeight * cDepthWidth; ++j) {
	//		*skeletonData = bodyIndexArray[j]; ++skeletonData;
	//		*skeletonData = bodyIndexArray[j]; ++skeletonData;
	//		*skeletonData = bodyIndexArray[j]; ++skeletonData;
	//	}
	//	delete[] bodyIndexArray;
	//}
	//SafeRelease(pBodyIndexFrame);//必须要释放，否则之后无法获得新的frame数据

								 //-----------------------获取深度数据并显示--------------------------
	if (onpicture == 2) {
		if (SUCCEEDED(hr)) {
			hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);//获得深度数据
		}
		if (SUCCEEDED(hr)) {
			UINT16 *depthArray = new UINT16[cDepthHeight * cDepthWidth];//深度数据是16位unsigned int
			pDepthFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, depthArray);

			//把深度数据画到MAT中
			uchar* depthData = (uchar*)depthImg.data;
			for (int j = 0; j < cDepthHeight * cDepthWidth; ++j) {
				*depthData = depthArray[j];
				++depthData;
			}
			delete[] depthArray;
		}
		SafeRelease(pDepthFrame);//必须要释放，否则之后无法获得新的frame数据
		imshow("view", depthImg);
		cv::waitKey(5);
	}

	//-----------------------------获取骨架并显示----------------------------
	if (onpicture == 1 || onpicture == 4 || onpicture==5 ) {
		if (SUCCEEDED(hr)) {
			hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);//获取骨架信息
		}
		if (SUCCEEDED(hr))
		{
			IBody* ppBodies[BODY_COUNT] = { 0 };//每一个IBody可以追踪一个人，总共可以追踪六个人

			if (SUCCEEDED(hr))
			{
				//把kinect追踪到的人的信息，分别存到每一个IBody中
				hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
			}

			if (SUCCEEDED(hr))
			{
				//对每一个IBody，找到他的骨架信息，并且画出来
				ProcessBody(BODY_COUNT, ppBodies);
			}

			for (int i = 0; i < _countof(ppBodies); ++i)
			{
				SafeRelease(ppBodies[i]);//释放所有
			}
		}
		SafeRelease(pBodyFrame);//必须要释放，否则之后无法获得新的frame数据
	}



	//-----------------------------获取骨架与彩图并融合显示3D景深彩图----------------------------
	//if (onpicture == 3) {
	//	if (SUCCEEDED(hr)) {
	//		hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);//获得深度数据
	//	}
	//	IFrameDescription* pFrameDescription = NULL;
	//	int nWidth = 0;
	//	int nHeight = 0;
	//	USHORT nDepthMinReliableDistance = 0;
	//	USHORT nDepthMaxDistance = 0;
	//	UINT nBufferSize = 0;
	//	UINT16 *pBuffer = NULL;
	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pDepthFrame->get_FrameDescription(&pFrameDescription);
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pFrameDescription->get_Width(&nWidth);
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pFrameDescription->get_Height(&nHeight);
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pDepthFrame->get_DepthMinReliableDistance(&nDepthMinReliableDistance);
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		nDepthMaxDistance = USHRT_MAX;
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		ProcessDepth(pBuffer, nWidth, nHeight, nDepthMinReliableDistance, nDepthMaxDistance);
	//	}
	//	SafeRelease(pFrameDescription);
	//	SafeRelease(pDepthFrame);

	//	if (SUCCEEDED(hr)) {
	//		hr = m_pColorFrameReader->AcquireLatestFrame(&pColorFrame);//获得彩图数据
	//	}
	//	nWidth = 0;
	//	nHeight = 0;
	//	ColorImageFormat imageFormat = ColorImageFormat_None;
	//	nBufferSize = 0;
	//	RGBQUAD *ppBuffer = NULL;
	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pColorFrame->get_FrameDescription(&pFrameDescription);
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pFrameDescription->get_Width(&nWidth);
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pFrameDescription->get_Height(&nHeight);
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		if (imageFormat == ColorImageFormat_Bgra)
	//		{
	//			hr = pColorFrame->AccessRawUnderlyingBuffer(&nBufferSize, reinterpret_cast<BYTE**>(&ppBuffer));
	//		}
	//		else if (m_pColorRGBX)
	//		{
	//			ppBuffer = m_pColorRGBX;
	//			nBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);
	//			hr = pColorFrame->CopyConvertedFrameDataToArray(nBufferSize, reinterpret_cast<BYTE*>(ppBuffer), ColorImageFormat_Bgra);
	//		}
	//		else
	//		{
	//			hr = E_FAIL;
	//		}
	//	}

	//	if (SUCCEEDED(hr))
	//	{
	//		ProcessColor(ppBuffer, nWidth, nHeight);
	//	}
	//	SafeRelease(pFrameDescription);
	//	SafeRelease(pColorFrame);
	//}
}

/// Handle new body data
void CBodyBasics::ProcessBody(int nBodyCount, IBody** ppBodies)
{
	//记录操作结果是否成功
	HRESULT hr;
	int nowbody = 0;
	//对于每一个IBody
	lines = lines + 1;
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody)//
		{
				BOOLEAN bTracked = false;
				hr = pBody->get_IsTracked(&bTracked);

				if (SUCCEEDED(hr) && bTracked)
				{
					if (nowbody < maxppbody) {
					nowbody++;
					Joint joints[JointType_Count];//存储关节点类
					HandState leftHandState = HandState_Unknown;//左手状态
					HandState rightHandState = HandState_Unknown;//右手状态

																 //获取左右手状态
					pBody->get_HandLeftState(&leftHandState);
					pBody->get_HandRightState(&rightHandState);

					//存储深度坐标系中的关节点位置
					DepthSpacePoint *depthSpacePosition = new DepthSpacePoint[_countof(joints)];

					//获得关节点类
					hr = pBody->GetJoints(_countof(joints), joints);
					if (SUCCEEDED(hr))
					{
						//jointfliter
						FilterDoubleExponential fliter1;
						fliter1.Update(joints);
						//-----------------test-----------------
						/*for (int k = 1; k <= 50; k=k+2) {
							testout[k] = joints[(k - 1) / 2].Position.X;
							testout[k + 1] = joints[(k - 1) / 2].Position.Z;
						}*/
						//-----------------test-----------------

						if (onpicture == 4) {
							for (int j = 0; j < _countof(joints); j++)
							{
								joints[j].Position.X = joints[j].Position.Z - joints[4].Position.Z;
							}
						}
						if (onpicture == 5) {
							for (int j = 0; j < _countof(joints); j++)
							{
								joints[j].Position.Y = joints[j].Position.Z - joints[4].Position.Z;
							}
						}

						for (int j = 0; j < _countof(joints); ++j)
						{
							//将关节点坐标从摄像机坐标系（-1~1）转到深度坐标系（424*512）
							m_pCoordinateMapper->MapCameraPointToDepthSpace(joints[j].Position, &depthSpacePosition[j]);
							
						}

						//------------------------hand state left-------------------------------
						if (gesture == 1)
						{
							DrawHandState(depthSpacePosition[JointType_HandLeft], leftHandState);
							DrawHandState(depthSpacePosition[JointType_HandRight], rightHandState);
						}
						//滤波
						Filtering(depthSpacePosition, i);

						DrawBone(i);//呈现


						if (IsRecord == 1) {
							CMFCKinectDlg a;
							//a.filename = "C:\\Users\\AA\\Desktop\\output.txt";
							a.filename = savefile;
							outpoint(outdeparray, a.filename);
						}
					}
					delete[] depthSpacePosition;
				}
			}
		}
	}
	cv::imshow("view", skeletonImg);
	cv::waitKey(5);
	if (IsRecord == 1) {
		CMFCKinectDlg a;
		a.filename = savefile;
		string x="\n";
		outfile(x,a.filename);
	}
}

//深度图处理（融合彩图）
//void CBodyBasics::ProcessDepth(const UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth)
//{
//	// Make sure we've received valid data  
//	if (m_pDepthRGBX && pBuffer && (nWidth == cDepthWidth) && (nHeight == cDepthHeight))
//	{
//		RGBQUAD* pRGBX = m_pDepthRGBX;
//
//		// end pixel is start + width*height - 1  
//		const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);
//
//		while (pBuffer < pBufferEnd)
//		{
//			USHORT depth = *pBuffer;
//
//			BYTE intensity = static_cast<BYTE>((depth >= nMinDepth) && (depth <= nMaxDepth) ? (depth % 256) : 0);     //深度数据由黑白图像显示//，每256个单位是一个有黑到白的周期  
//
//
//			pRGBX->rgbRed = intensity;
//			pRGBX->rgbGreen = intensity;
//			pRGBX->rgbBlue = intensity;
//
//			++pRGBX;
//			++pBuffer;
//		}
//
//		// Draw the data with OpenCV  
//		Mat DepthImage(nHeight, nWidth, CV_8UC4, m_pDepthRGBX);
//		Mat show = DepthImage.clone();
//
//		resize(DepthImage, show, Size(cDepthWidth*1.437, cDepthHeight*1.437));
//		depthcolorImg = show.clone();
//
//	}
//}
////彩图处理（融合深度图）
//void CBodyBasics::ProcessColor(RGBQUAD* pBuffer, int nWidth, int nHeight)
//{
//	// Make sure we've received valid data     
//	if (pBuffer && (nWidth == cColorWidth) && (nHeight == cColorHeight))
//	{
//		// Draw the data with OpenCV  
//		Mat ColorImage(nHeight, nWidth, CV_8UC4, pBuffer);
//		Mat showImage = ColorImage.clone();
//		resize(ColorImage, showImage, Size(nWidth / 2, nHeight / 2));
//		Rect rect(145, 0, 702, 538);
//		int x = 33, y = -145;
//		//CV_8UC4   
//		for (int i = 0; i <540; i++)
//			for (int j = 145; j < 960 - 114; j++)
//				showImage.at<Vec4b>(i, j) = depthcolorImg.at<Vec4b>(i + x, j + y)*0.6 + showImage.at<Vec4b>(i, j)*0.4;
//		Mat image_roi = showImage(rect);
//		//imshow("ColorImage", showImage);//imshow("ColorImage", ColorImage);  
//		cv::imshow("view", image_roi);
//		cv::waitKey(5);
//	}
//}


//画手的状态
void CBodyBasics::DrawHandState(const DepthSpacePoint depthSpacePosition, HandState handState)
{
	//给不同的手势分配不同颜色
	CvScalar color;
	switch (handState) {
	case HandState_Open:
		color = cvScalar(255, 0, 0);
		break;
	case HandState_Closed:
		color = cvScalar(0, 255, 0);
		break;
	case HandState_Lasso:
		color = cvScalar(0, 0, 255);
		break;
	default://如果没有确定的手势，就不画
		return;
	}

	circle(skeletonImg,
		cvPoint(depthSpacePosition.X, depthSpacePosition.Y),
		10, color, -1);
}


/// Draws one bone of a body (joint to joint)
void CBodyBasics::DrawBone(int pbody)
{
	//TrackingState joint0State = pJoints[joint0].TrackingState;

	//// If we can't find this joints, exit
	//if ((joint0State == TrackingState_NotTracked))
	//{
	//	return;
	//}

	//if ((joint0State == TrackingState_Inferred))
	//{
	//	return;
	//}

	//CvPoint p1 = cvPoint(depthSpacePosition[joint0].X, depthSpacePosition[joint0].Y);

	CvScalar color;
	switch (pbody)
	{
	case 5:
		color= cvScalar(255, 255, 255);
		break;
	case 4:
		color = cvScalar(139, 37, 0);
		break;
	case 3:
		color = cvScalar(238, 59, 59);
		break;
	case 2:
		color = cvScalar(255, 0, 255);
		break;
	case 1:
		color = cvScalar(0, 255, 0);
		break;
	case 0:
		color = cvScalar(65, 105, 225);
		break;
	default:
		break;
	}
	for (int i = 1; i < 50; i=i+2)
	{
		if (pCheckbox[(i - 1) / 2] == 1) {
			circle(skeletonImg, cvPoint(outdeparray[i], outdeparray[i + 1]), 1, color, -1);
			//circle(skeletonImg, cvPoint(testout[i]*212+212, testout[i + 1]), 1, color, -1);
		}
	}
	


	// We assume all drawn bones are inferred unless BOTH joints are tracked
	//if ((joint0State == TrackingState_Tracked) && (joint1State == TrackingState_Tracked))
	//{
	//	//非常确定的骨架，用白色直线
	//	line(skeletonImg, p1, p2, cvScalar(255, 255, 255));
	//}
	//else
	//{
	//	//不确定的骨架，用红色直线
	//	line(skeletonImg, p1, p2, cvScalar(0, 0, 255));
	//}
	
}


/// Constructor
CBodyBasics::CBodyBasics()
{
	m_pKinectSensor = NULL;
	m_pCoordinateMapper = NULL;
	m_pBodyFrameReader = NULL;
	m_pColorFrameReader = NULL;
	m_pDepthFrameReader = NULL;
	m_pDepthRGBX = new RGBQUAD[cDepthWidth * cDepthHeight];// create heap storage for color pixel data in RGBX format  ，开辟一个动态存储区域  
	m_pColorRGBX = new RGBQUAD[cColorWidth * cColorHeight];// create heap storage for color pixel data in RGBX format  
}

/// Destructor
CBodyBasics::~CBodyBasics()
{
	SafeRelease(m_pBodyFrameReader);
	SafeRelease(m_pCoordinateMapper);
	if (m_pDepthRGBX)
	{
		delete[] m_pDepthRGBX;            //删除动态存储区域  
		m_pDepthRGBX = NULL;
	}
	SafeRelease(m_pDepthFrameReader);// done with depth frame reader  
	if (m_pColorRGBX)
	{
		delete[] m_pColorRGBX;
		m_pColorRGBX = NULL;
	}
	SafeRelease(m_pColorFrameReader);// done with color frame reader
	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}
	SafeRelease(m_pKinectSensor);
}


void Filtering(DepthSpacePoint* pointSet, int pbody)
{
	//中值平均滤波
	int j = 0;
	int k = 0;
	int max[51];
	int min[51];
	for (k = 0; k < 4; k++)
	{
		for (j = 1; j <= 50; j++)
		{
			deparray[pbody][k][j] = deparray[pbody][k + 1][j];
		}
	}
	for (j = 1; j <= 50; j = j + 2) {
		deparray[pbody][4][j] = pointSet[(j - 1) / 2].X;
		deparray[pbody][4][j+1] = pointSet[(j - 1) / 2].Y;
	}

	for (j = 1; j <= 50; j++)
	{
		max[j] = deparray[pbody][0][j];
		min[j] = deparray[pbody][0][j];
	}

	for (j = 1; j <= 50; j++)
	{
		for (k = 1; k <= 4; k++)
		{
			if (max[j] < deparray[pbody][k][j]) max[j] = deparray[pbody][k][j];
			if (min[j] > deparray[pbody][k][j]) min[j] = deparray[pbody][k][j];
		}
	}

	for (j=1; j <= 50; j++)
	{
		outdeparray[j] = (deparray[pbody][0][j] + deparray[pbody][1][j] + deparray[pbody][2][j] + deparray[pbody][3][j] + deparray[pbody][4][j] - max[j] - min[j]) / 3;
	}

	//一像素点抖动防止
	for (j = 1; j <= 49; j=j+2)
	{
		if (abs(outdeparray[j] - lastoutputarray[pbody][j]) + abs(outdeparray[j + 1] - lastoutputarray[pbody][j + 1]) == 1) 
		{ 
			outdeparray[j] = lastoutputarray[pbody][j]; 
			outdeparray[j + 1] = lastoutputarray[pbody][j + 1];
		}
	}

}
