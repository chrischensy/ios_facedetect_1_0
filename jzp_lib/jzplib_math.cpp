//
//  jzplib_math.cpp
//  OPENCV_HOTSHOTS
//
//  Created by Zhiping Jiang on 14-7-20.
//
//

#include "jzplib_math.h"

using namespace std;
using namespace cv;

Mat mat2gray(Mat original) {
    Mat temp ;

    switch (original.depth()) {
        case CV_8U:
            normalize(original, temp,0,255,NORM_MINMAX);
            break;
        case CV_16U:
            normalize(original, temp,0,255*256,NORM_MINMAX);
            break;
        case CV_32S:
            normalize(original, temp,0,255*256,NORM_MINMAX);
            break;
        case CV_32F:
            normalize(original, temp,0,1.0f,NORM_MINMAX);
            break;
        
        default:
            break;
    }
    
    return temp;
}

Mat gaussmf(Mat xpts, double sigma, double center) {
    Mat ypts;
    pow(xpts-center,2,ypts);
    ypts *= -1;
    ypts /= (2*sigma*sigma);
    
    exp(ypts,ypts);
    
    return ypts;
}

Mat linspace(double start, double end, double interval) {
    Mat a(1,1,CV_32FC1);
    vector<double> result;
    for (double s= start; s<end; s+=interval) {
        result.push_back(s);
    }
    
    Mat(result).convertTo(a, CV_32FC1);
    return a;
}

Mat diff(Mat base, int dim) {
    Mat working, temp;
    base.copyTo(working);
    
    if (dim == 1) {
        working = working.t();
    }
    
    if (working.rows >0) {
        
        temp = working.rowRange(1, working.rows);
        
        working.rowRange(0, working.rows-1) = temp - working.rowRange(0, working.rows-1);
        
        working = working.rowRange(0, working.rows-1);
        
    }
    
    
    if (dim == 1) {
        working = working.t();
    }
    
    return working;
    
}
