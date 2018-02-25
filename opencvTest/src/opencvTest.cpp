//============================================================================
// Name        : opencvTest.cpp
// Author      : zhakangning
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

string xmlPath="haarcascade_frontalface_default.xml";
//xmlpath 字符串记录那个.xml文件的路径
void detectAndDisplay(Mat image);
int main( int argc, char**argv )
{
	string path="picture";//以检测图片1.jpg为例
	Mat image =imread(path,-1);

	CascadeClassifier a;     //创建脸部对象
	if(!a.load(xmlPath))     //如果读取文件不出错，则检测人脸
	{
		cout<<"无法加载xml文件"<<endl;
		return 0;
	}
	detectAndDisplay(image);// 检测人脸
	return 0;

}

void detectAndDisplay(Mat image)
{
	CascadeClassifier ccf;      //创建脸部对象
	ccf.load(xmlPath);           //导入opencv自带检测的文件
	vector<Rect> faces;
	Mat gray;
	cvtColor(image,gray,CV_BGR2GRAY);
	cout<<"here1\n";
	equalizeHist(gray,gray);
	cout<<"here2\n";
	ccf.detectMultiScale(gray,faces,1.1,3,0,Size(50,50),Size(500,500));
	for(vector<Rect>::const_iterator iter=faces.begin();iter!=faces.end();iter++)
	{
		rectangle(image,*iter,Scalar(0,0,255),2,8); //画出脸部矩形
	}
//	Mat image1;
//
//	for(size_t i=0;i<faces.size();i++)
//	{
//		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
//		image1= image(Rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height));
//	}

	imshow("1",image);
	cvWaitKey(0);
}
