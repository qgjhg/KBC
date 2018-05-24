#include "stdafx.h"
#include "MFCKinect.h"
#include "MFCKinectDlg.h"
#include <iostream>
#include <cstdio>
using namespace std;
using namespace cv;

extern int pCheckbox[25];

void StartGetFrame(void)
{


}

void outfile(string x, CString filepath)
{
	fstream fout;
	fout.open(filepath, ios::app | ios::app);//file path
	fout << x;
	fout.close();
}

void outpoint(int* pointSet, CString filepath)
{
	int i = 1;
	fstream fout;
	fout.open(filepath, ios::app | ios::app);//file path
	for (i = 1; i <= 50; i=i+2) {
		if (pCheckbox[(i-1)/2] == 1) {
			fout << pointSet[i] << "," << pointSet[i + 1] << ",";
		}
	}
	//fout << pointSet[0].X << "," << pointSet[0].Y <<","<< pointSet[1].X << "," << pointSet[1].Y << "," << pointSet[2].X << "," << pointSet[2].Y << "," << pointSet[3].X << "," << pointSet[3].Y << "," << pointSet[4].X << "," << pointSet[4].Y << "," << pointSet[5].X << "," << pointSet[5].Y << "," << pointSet[6].X << "," << pointSet[6].Y << "," << pointSet[7].X << "," << pointSet[7].Y << ","<< pointSet[8].X << "," << pointSet[8].Y << "," << pointSet[9].X << "," << pointSet[9].Y << "," << pointSet[10].X << "," << pointSet[10].Y << "," << pointSet[11].X << "," << pointSet[11].Y << "," << pointSet[12].X << "," << pointSet[12].Y << "," << pointSet[13].X << "," << pointSet[13].Y << "," << pointSet[14].X << "," << pointSet[14].Y << "," << pointSet[15].X << "," << pointSet[15].Y << "," << pointSet[16].X << "," << pointSet[16].Y << "," << pointSet[17].X << "," << pointSet[17].Y << "," << pointSet[18].X << "," << pointSet[18].Y << "," << pointSet[19].X << "," << pointSet[19].Y << "," << pointSet[20].X << "," << pointSet[20].Y << "," << pointSet[21].X << "," << pointSet[21].Y << "," << pointSet[22].X << "," << pointSet[22].Y << "," << pointSet[23].X << "," << pointSet[23].Y << "," << pointSet[24].X << "," << pointSet[24].Y << "," << "\n";
	//fout << pointSet[0].X << "," << pointSet[0].Y << "," << pointSet[1].X << "," << pointSet[1].Y << "," << pointSet[2].X << "," << pointSet[2].Y << "," << pointSet[3].X << "," << pointSet[3].Y << "," << pointSet[4].X << "," << pointSet[4].Y << "," << pointSet[5].X << "," << pointSet[5].Y << "," << pointSet[6].X << "," << pointSet[6].Y << "," << pointSet[7].X << "," << pointSet[7].Y << "," << pointSet[8].X << "," << pointSet[8].Y << "," << pointSet[9].X << "," << pointSet[9].Y << "," << pointSet[10].X << "," << pointSet[10].Y << "," << pointSet[11].X << "," << pointSet[11].Y << "," << pointSet[12].X << "," << pointSet[12].Y << "," << pointSet[13].X << "," << pointSet[13].Y << "," << pointSet[14].X << "," << pointSet[14].Y << "," << pointSet[15].X << "," << pointSet[15].Y << "," << pointSet[16].X << "," << pointSet[16].Y << "," << pointSet[17].X << "," << pointSet[17].Y << "," << pointSet[18].X << "," << pointSet[18].Y << "," << pointSet[19].X << "," << pointSet[19].Y << "," << pointSet[20].X << "," << pointSet[20].Y << "," << pointSet[21].X << "," << pointSet[21].Y << "," << pointSet[22].X << "," << pointSet[22].Y << "," << pointSet[23].X << "," << pointSet[23].Y << "," << pointSet[24].X << "," << pointSet[24].Y << "," << "\n";
	fout.close();
}

void newfile()
{
	CString outfilepath("C:\\Users\\AA\\Desktop\\output.txt");//输出路径
	//outfile("TEST file....", outfilepath);
	//outfile("Point1x,Point1y,Point2x,Point2y,Point3x,Point3y,Point4x,Point4y,Point5x,Point5y,Point6x,Point6y,Point7x,Point7y,Point8x,Point8y,Point9x,Point9y,Point10x,Point10y,Point11x,Point11y,Point12x,Point12y,Point13x,Point13y,Point14x,Point14y,Point15x,Point15y,Point16x,Point16y,Point17x,Point17y,Point18x,Point18y,Point19x,Point19y,Point20x,Point20y,Point21x,Point21y,Point22x,Point22y,Point23x,Point23y,Point24x,Point24y,Point25x,Point25y", outfilepath);
}

//void LineKinectreader(int Lineindex)//读取一行txt中的数据
//{
//
//	string pdata[50];
//	int size;
//	//while (!file.eof()) {
//	while (size<=50) {
//		//testline[size] = linedata;
//		//string(*pdata)[50] = new string[size][50];
//		for (i = 1; i <= 50; i++) {
//			file >> pdata[size][i];
//		}
//		size++;
//	}
//	file.close();
//
//
//	//int j;
//	//CString outfilepath("C:\\Users\\AA\\Desktop\\heihei.txt");
//	//outfile("TEST file....", outfilepath);
//	//fstream fout;
//	//fout.open(outfilepath, ios::app | ios::app);//file path
//	//for (i = 1; i <= size - 1; i++) {
//	//	for (j = 1; j <= 50; j++) {
//	//		fout << pdata[i][j];
//	//	}
//	//	fout << "\n";
//	//}
//	//fout.close();
//
//	//outfile("OK", outfilepath);
//	//for (int i = 0; i<size; i++)
//	//{
//	//	delete[]  pdata[i];   // 要在指针前加[] ， 否则的话 只释放p[i]所指的第一个单元所占的空间
//	//}
	//delete[] pdata;     //最后不要忘掉 释放掉开辟的指针数组  ：》
//
//}
void CVplayerclass::chushihua() {
	//用于播放的MAT
	cvplayer.create(cDepthHeight, cDepthWidth, CV_8UC4);
	cvplayer.setTo(0);

}


void CVplayerclass::update(double point[51]){
	cvplayer.setTo(0);

	process(point);


}

void CVplayerclass::process(double point[51]) {
	int i = 1;
	for (i = 1; i < 300;i=i+2){
		cvplay(point[i], point[i + 1]);
	}

	cv::imshow("view", cvplayer);
	cv::waitKey(30);


}
void CVplayerclass::cvplay(double x, double y)
{
	CvPoint P1 = cvPoint(x, y);
	CvScalar color = cvScalar(255, 255, 255);
	circle(cvplayer,
		cvPoint(x, y),
		1, color, -1);
}
//
//void split(const char * str, const char * deli, vector<string> *list)//分隔字符串
//{
//	char buff[1024];
//	snprintf(buff, sizeof(buff), str);
//	char * gg;
//	char *p = strtok_s(buff, deli, &gg);
//
//	list->clear();
//	while (p != NULL)
//	{
//		list->push_back(p);
//		p = strtok_s(NULL, deli, &gg);
//	};
//}

int main()
{
	
}