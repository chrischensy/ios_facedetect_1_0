//
//  skinColorDetection.h
//  OPENCV_HOTSHOTS
//
//  Created by Zhiping Jiang on 14-7-18.
//
//

#ifndef __OPENCV_HOTSHOTS__skinColorDetection__
#define __OPENCV_HOTSHOTS__skinColorDetection__

#include <iostream>

using namespace cv;
using namespace std;

Mat SkinRGB(Mat rgb_img);
Mat cvSkinRG(Mat rgb_img);
void cvThresholdOtsu(IplImage* src, IplImage* dst);
Mat cvSkinOtsu(Mat rgb_img);
Mat cvSkinYUV(Mat rgb_img, bool grayOrColor);
Mat cvSkinHSV(Mat rgb_img);


#endif /* defined(__OPENCV_HOTSHOTS__skinColorDetection__) */
