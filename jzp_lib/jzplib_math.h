//
//  jzplib_math.h
//  OPENCV_HOTSHOTS
//
//  Created by Zhiping Jiang on 14-7-20.
//
//

#ifndef __OPENCV_HOTSHOTS__jzplib_math__
#define __OPENCV_HOTSHOTS__jzplib_math__

#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace std;
using namespace cv;

Mat mat2gray(Mat original);
Mat gaussmf(Mat xpts, double sigma, double center);
Mat linspace(double start, double end, double interval);
Mat diff(Mat base, int dim = 0);

#endif /* defined(__OPENCV_HOTSHOTS__jzplib_math__) */
