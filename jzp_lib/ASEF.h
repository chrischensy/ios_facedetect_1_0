//
//  ASEF.h
//  OPENCV_HOTSHOTS
//
//  Created by Zhiping Jiang on 14-7-18.
//
//

#ifndef __OPENCV_HOTSHOTS__ASEF__
#define __OPENCV_HOTSHOTS__ASEF__

#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;


#define LINE_BUF_SIZE 1024

 int load_asef_filters(string file_name, int *p_n_rows, int *p_n_cols,
                             CvRect *left_eye_region, CvRect *right_eye_region, //CvRect »∑∂®æÿ–Œ∑ΩøÚ
                             CvMat **p_left_filter, CvMat **p_right_filter); // æÿ’Û±‰ªª∫Ø ˝£®‘ı√¥±‰£ø£©

 int read_line(FILE *fp, char *buf, int size);
static double MinValuel, MinValuer;

class ASEF_Algorithm {
public:
    Mat input_image;
    Point leftEyePoint, rightEyePoint;
    Rect  leftEyeRect , rightEyeRect;
    ASEF_Algorithm();
    bool isInitialized ;
    int initialize();
    bool eyeFound;
    void findEyes(Mat faceImage);
    void findEyes(Mat image, Rect faceRect);
    static Rect getCanthus_LL_Rect(Rect leftEyeRect);
    static Rect getCanthus_RR_Rect(Rect leftEyeRect);
    static Rect getCanthus_LR_Rect(Rect leftEyeRect);
    static Rect getCanthus_RL_Rect(Rect leftEyeRect);
    static vector<Point2f> getInnerCanthus(Mat input_image, Rect leftEyeRect, Rect rightEyeRect);
    static vector<Point2f> getOutterCanthus(Mat input_image, Rect leftEyeRect, Rect rightEyeRect);
private:
    
    Point left_eye, right_eye;
    
    CvMat *lfilter, *rfilter;
    int n_rows, n_cols;
    CvRect lrect, rrect;
    CvMat *lfilter_dft, *rfilter_dft;
    
    Mat face_image;
    CvMat *scaled_face_image_32fc1, *scaled_face_image_8uc1;
    
    CvMat *lcorr, *rcorr;
    CvMat *lroi, *rroi;
    CvMat *lut;
    CvMat *point;
    
    
    void detecteyes(Mat a);
    string haar_cascade_path;
};



#endif /* defined(__OPENCV_HOTSHOTS__ASEF__) */
