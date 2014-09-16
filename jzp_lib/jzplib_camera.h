//
//  jzplib_camera.h
//  JZP_EYE_TRACKING
//
//  Created by Zhiping Jiang on 14-8-23.
//
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include "jzplib_utils.h"
#include <boost/lockfree/spsc_queue.hpp>
#define Size Size2i
#define Rect Rect_<int>
using namespace boost::lockfree;
using namespace cv;
using namespace std;

bool readCameraProfile(const fs::path & filepath, Mat & cameraMatrix, Mat & distCoeffs);

Mat cameraMatrixByCropNResize(Mat originalCameraMatrix, Size originalSize, Rect currentRect, float resizeFactor);

bool captureImage(VideoCapture& capture, Mat& color_img, bool flip = true);

void videoCaptureThreadFun(VideoCapture *capture, Mat *frame, int framerate, bool * finishSign);

void videoBufferThreadFun(spsc_queue<Mat,boost::lockfree::capacity<512> > * buffer, Mat *frame, int framerate, int totalFrames, bool * writeEnabled,bool * finishSign);

void videoWriterThreadFun(VideoWriter * writer, spsc_queue<Mat,boost::lockfree::capacity<512> > * buffer, int framerate, bool * writeEnabled, bool * finishSign);

vector<vector<Point3f> > calcBoardCornerPositions(int gridW, int gridH, float squareSize, int imagesCount);

bool chessboardCameraCalibration(int gridW, int gridH, float gridSize, vector<fs::path> imagePaths, Mat & cameraMatrix, Mat & distCoeffs, bool drawCorners);
