//
//  jzplib_camera.cpp
//  JZP_EYE_TRACKING
//
//  Created by Zhiping Jiang on 14-8-23.
//
//

#include "jzplib_camera.h"
#include <boost/lockfree/spsc_queue.hpp>
#define Point2f P2F
#define Point Point2i
#define P2F Point2f

using namespace boost::lockfree;

bool readCameraProfile(const fs::path & filepath, Mat & cameraMatrix, Mat & distCoeffs) {
    FileStorage fileLoader(filepath.string(),FileStorage::READ);
    fileLoader["cameraMatrix"]>>cameraMatrix;
    fileLoader["distCoeffs"] >> distCoeffs;
    fileLoader.release();
    if (cameraMatrix.empty() || distCoeffs.empty()) {
        return false;
    }
    else return true;
}

Mat cameraMatrixByCropNResize(const Mat originalCameraMatrix, Size originalSize, Rect currentRect, float resizeFactor) {
    Mat nowcm = originalCameraMatrix;
    cout<<nowcm.channels()<<" "<<nowcm.size()<<endl;
    Point tl = currentRect.tl();
    nowcm.at<double>(0, 2) -= double(tl.x);
    nowcm.at<double>(1, 2) -= double(tl.y);
    float s = nowcm.at<double>(0,1);
    nowcm *= (resizeFactor);
    nowcm.at<double>(0,1) = s;
    nowcm.at<double >(2,2) = 1.0;
    cout<<nowcm<<endl;
    return nowcm;
}

bool captureImage(VideoCapture& capture, Mat& color_img, bool flipImage) {
    Rect squareRect;
    Mat origin;
    if (&capture && capture.isOpened() && capture.read(origin)) {
        if (flipImage)
            flip(origin, color_img, 1);
        else {
            origin.copyTo(color_img);
        }
        return true;
    } else {
        return false;
    }
    
}

void videoCaptureThreadFun(VideoCapture *capture, Mat *frame, int framerate, bool * finishSign){
    //loop infinitely
    int frameCount = 0;
    cout<<"start capturing @ "<<framerate<<" fps..."<<endl;
    LowpassFPSTimer timer(10);
    FPScontroller delayer(framerate);
    while (!(*finishSign) && capture->isOpened()) {
        timer.tick();
		captureImage((*capture), (*frame));
        frameCount ++;
        delayer.controlledDelay();
	}
    cout<<"capture stopped. totally "<<frameCount<<" frames , or "<<(double)frameCount/framerate<<" seconds."<<endl;
}

void videoBufferThreadFun(spsc_queue<Mat,boost::lockfree::capacity<512> > * buffer, Mat *frame, int framerate, int totalFrames, bool * writeEnabled,bool * finishSign) {
    using namespace boost::posix_time;
    int frameCount = 0;
    cout<<"start recording @ "<<framerate<<" fps..."<<endl;
    LowpassFPSTimer timer(1);
    FPScontroller * delayer;
    Mat temp;
	while ((*finishSign)==false && (totalFrames <=0 || frameCount <totalFrames)) {
        
        if ((*writeEnabled)) {
            buffer->push((*frame));
            if(frameCount ==0) {
                delayer = new FPScontroller(framerate);
            }
            
            frameCount ++;
            timer.tick();
            delayer->controlledDelay();
        }
        
	}
    cout<<"buffer finished, totally "<<frameCount<<" frames , or "<<(double)frameCount/framerate<<" seconds."<<endl;
}

void videoWriterThreadFun(VideoWriter * writer, spsc_queue<Mat,boost::lockfree::capacity<512> >* buffer, int framerate, bool * writeEnabled, bool * finishSign) {
    using namespace boost::posix_time;
    int frameCount = 0;
    cout<<"start writing @ "<<framerate<<" fps..."<<endl;
    FPScontroller delayer(framerate*2);
	while ((*finishSign)==false || !buffer->empty()) {
        if (!buffer->empty()) {
            Mat frame;
            buffer->pop(frame);
            (*writer) << frame;
            frameCount ++;
        }
        delayer.controlledDelay();
	}
    cout<<"writer finished, totally "<<frameCount<<" frames."<<endl;
}


vector<vector<Point3f> > calcBoardCornerPositions(int gridW, int gridH, float squareSize, int imagesCount)
{
    vector<vector<Point3f> > objectPoints(imagesCount);
    for (int k = 0 ; k <imagesCount; k++) {
        objectPoints[k] = vector<Point3f>(0);
        for( int i = 0; i < gridH; i++ )
            for( int j = 0; j < gridW; j++ )
                objectPoints[k].push_back(Point3f(float( j*squareSize ), float( i*squareSize ), 0));
    }
    //    objectPoints.resize(imagesCount,objectPoints[0]);
//    cout<<objectPoints.size()<<" "<<objectPoints[0].size()<<endl;
    return objectPoints;
}

bool chessboardCameraCalibration(int gridW, int gridH, float gridSize, vector<fs::path> imagePaths, Mat & cameraMatrix, Mat & distCoeffs, bool drawCorners) {
    vector<Mat> images;
    for (int i = 0; i <imagePaths.size(); i++) {
        //        cout<<files[i].string()<<endl;
        images.push_back(imread(imagePaths[i].string()));
        //        imshow("chessboard"+boost::lexical_cast<std::string>(i),images[i]);
    }
    
    Size grids(gridW,gridH);
    vector<vector<Point2f> > imagePoints;
    vector<int> usefulImgIndeces;
    for (int i = 0 ; i <images.size() ; i ++) {
        vector<Point2f> corners;
        bool found = findChessboardCorners(images[i], grids, corners,CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE+CALIB_CB_FAST_CHECK);
        
        if (found) {
            usefulImgIndeces.push_back(i);
            Mat gray;
            cvtColor(images[i], gray, CV_BGR2GRAY);
            cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
                         TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
            imagePoints.push_back(corners);
            if (drawCorners) {
                drawChessboardCorners(images[i], grids, Mat(corners), found);
                imshow("chessboard"+boost::lexical_cast<std::string>(i),images[i]);
            }
        }
    }
    
    if (usefulImgIndeces.empty()) {
        cout<<"no chessboard found."<<endl;
        return false;
    }
    
    vector<vector<Point3f> > objectPoints = calcBoardCornerPositions(gridW,gridH, gridSize, imagePoints.size());
    Size imageSize = images[0].size();
    vector<Mat> rvecs, tvecs;
    double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs);
    
    return true;
}