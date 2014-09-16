//
//  jzplib_detections.h
//  OPENCV_HOTSHOTS
//
//  Created by Zhiping Jiang on 14-7-18.
//
//

#ifndef __OPENCV_HOTSHOTS__jzplib_detections__
#define __OPENCV_HOTSHOTS__jzplib_detections__

#include <iostream>
#include <opencv2/opencv.hpp>
#include "jzplib_img.h"
#include "jzplib_draw.h"
#include "jzplib_bwOps.h"
#include "jzplib_math.h"
#include "skinColorDetection.h"
#include "GraphUtils.h"

#define Rect Rect_<int>
using namespace std;
using namespace cv;


Rect haarPatternDetection(CascadeClassifier classifier, Mat image, int imageWidthforDetection, Rect parentRect);

Point2f findMassCenter_BinaryBiggestBlob(const Mat& image);
void  findEyeCenterByColorSegmentation(const Mat& image, Point2f & eyeCord, float coordinateWeight= 0.4, int kmeansIterations = 3, int kmeansRepeats= 3, int blurSize= 5);

Mat calculateImageSymmetryScore(const Mat& image );

class SymmetryScore_tbb : public ParallelLoopBody {
public:
    SymmetryScore_tbb(Mat & gray, Mat & score);
    virtual void operator() (const cv::Range& range) const;
    
private:
    Mat flipped;
    Mat & score;
    Mat & gray_img;
};

#endif /* defined(__OPENCV_HOTSHOTS__jzplib_detections__) */
