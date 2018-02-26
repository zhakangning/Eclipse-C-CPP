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
string HashValue(Mat &src);      //得到图片的哈希值
int HanmingDistance(string &str1,string &str2);       //求两张图片的汉明距离
void detectAndDisplay(Mat image, Mat &face);


int main( int argc, char**argv )
{
	//	int a, x, i;
	int i;
	int diff[9];
	Mat image1, image2;
	image1 = imread("picture",-1);
	string str1, str2, path2;
	//	cvNamedWindow("选择的图片",1);
	/*cvResizeWindow("选择的图片",700,500);*/
	//	imshow("选择的图片", image1);
	detectAndDisplay(image1, image1);
	str1 = HashValue(image1);
	//	cvWaitKey(0);
	image2 = imread("pic4", -1);
//	detectAndDisplay(image2, image2);
	str2 = HashValue(image2);
	cout<<str1<<endl;
	cout<<str2<<endl;
	cout<<"汉明距离 = "<<HanmingDistance(str1, str2);
	return 0;
	//	for(i=1;i<=8;i++)//因为我完成的就是8张图片的检测，所以循环值为8
	//	{
	//		path2=format("C:\\Users\\yu\\Documents\\Visual Studio 2010\\Projects\\test\\images\\%d.jpg",i);
	//		image2=imread(path2,-1);
	//		detectAndDisplay(image2);
	//		str2=HashValue(image2);
	//		diff[i]=HanmingDistance(str1,str2);
	//	}


	int min=1000,t;
	for(i=1;i<=8;i++)    //循环值为8，求与原图片汉明距离最小的那张图片
	{
		if(min>diff[i]&&diff[i]!=0)
		{
			min=diff[i];
			t=i;}           //检测出的标记为t
	}
	path2=format("C:\\Users\\yu\\Documents\\Visual Studio 2010\\Projects\\test\\images\\%d.jpg",t);
	image2=imread(path2, -1);//将图片t显示出来
	cvNamedWindow("相似的图片",1);
	imshow("相似的图片",image2);//这时显示的就是最相似的照片
	cvWaitKey(0);
	cin.get();                    //吃掉回车符

	return 0;
}

string HashValue(Mat &src)      //得到图片的哈希值
{
	string rst(64,'\0');
	Mat img;
	if(src.channels() == 3)
		cvtColor(src,img,CV_BGR2GRAY);
	else
		img=src.clone();

	resize(img,img,Size(8,8));
	uchar *pData;
	for(int i=0;i<img.rows;i++)
	{
		pData=img.ptr<uchar>(i);
		for(int j=0;j<img.cols;j++)
		{
			pData[j]=pData[j]/4;
		}
	}

	int average = mean(img).val[0];
	Mat mask=(img>=(uchar)average);
	int index=0;
	for(int i=0;i<mask.rows;i++)
	{
		pData = mask.ptr<uchar>(i);
		for(int j=0;j<mask.cols;j++)
		{
			if(pData[j]==0)
				rst[index++]='0';
			else
				rst[index++]='1';
		}
	}
	return rst;
}

int HanmingDistance(string &str1,string &str2)       //求两张图片的汉明距离
{
	if((str1.size()!=64)||(str2.size()!=64))
		return -1;
	int diff=0;
	for(int i=0;i<64;i++)
	{
		if(str1[i]!=str2[i])
			diff++;
	}
	return diff;
}

void detectAndDisplay(Mat image, Mat &face)
{
	CascadeClassifier ccf;      //创建脸部对象
	ccf.load(xmlPath);           //导入opencv自带检测的文件
	vector<Rect> faces;
	Mat gray;
	cvtColor(image, gray, CV_BGR2GRAY);
	equalizeHist(gray, gray);
	ccf.detectMultiScale(gray,faces,1.1,3,0,Size(50,50),Size(500,500));
	for(vector<Rect>::const_iterator iter=faces.begin();iter!=faces.end();iter++)
	{
		rectangle(image, *iter, Scalar(0, 0, 255), 2, 8); //画出脸部矩形
	}
	for(size_t i=0;i<faces.size();i++)
	{
		Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
		face = image(Rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height));
	}

//	imshow("1",image);
//	cvWaitKey(0);
}
