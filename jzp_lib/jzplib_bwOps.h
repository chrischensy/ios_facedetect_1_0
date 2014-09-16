//
//  jzplib_bwOps.h
//  OPENCV_HOTSHOTS
//
//  Created by Zhiping Jiang on 14-7-18.
//
//

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


Mat fillHoleInBinary(Mat bwImage);
Mat removeSmallBlobs(Mat bwImage);
Mat fillConvexHulls(Mat bwImage) ;
