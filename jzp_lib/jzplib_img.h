//
//  jzplib_img.h
//  OPENCV_HOTSHOTS
//
//  Created by Zhiping Jiang on 14-7-18.
//
//

#ifndef __OPENCV_HOTSHOTS__jzplib_img__
#define __OPENCV_HOTSHOTS__jzplib_img__

#include <iostream>
#include <opencv2/opencv.hpp>
#include "jzplib_math.h"

using namespace std;
using namespace cv;


Mat sobelEdgeDerivatives(Mat src);
Mat magicEqualHist(Mat faceimg);
Mat equalizeImage(const Mat& inputImage);

Mat imresize(const Mat& image, float scale);
void imresize(const Mat& image, float scale, Mat& dst);
void imresize(const Mat& image, int imageWidthforDetection, Mat& dst);

Mat calculateColorHistogram(Mat image,Mat mask, int dims , int colorSpace );
Mat calculateColorBackProjection(Mat image, Mat hist,int colorSpace ) ;

Mat rgbScaleColorSpace(Mat rgb);

Mat imcomplement(Mat gray);

int sub2ind(const int row,const int col,const int cols,const int rows);
void ind2sub(const int sub,const int cols,const int rows,int &row,int &col);

Mat matrixPointCoordinates(int rows, int cols ,bool matlabColumnFirst) ;

#endif /* defined(__OPENCV_HOTSHOTS__jzplib_img__) */
