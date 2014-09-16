//
//  ASMPupilTracker.h
//  JZP_EYE_TRACKING
//
//  Created by Zhiping Jiang on 14-8-31.
//
//

#include <iostream>
#include "ft.hpp"
#include "jzplib_all.h"
#include <boost/thread.hpp>

class ASM_Gaze_Tracker {
public:
    ASM_Gaze_Tracker();
    void initASM_Gaze_Tracker(const fs::path & trackermodel, const fs::path & cameraProfile = fs::path());
    bool featureTracking(const cv::Mat & im);
    void reDetectFace();
    bool getTracker();
    
    bool calculatePupilCenter();
    
    bool estimateFacePose();
    void projectPoints(const vector<Point3f> & sourcePoints, vector<Point2f> & destPoints);
    float distanceToCamera();
    void findBestFrontalFaceShapeIn3D() ;
    
    vector<float> toDataSlot();
    
    
    
    Point2f glabellaPoint;
    float eyePairTileAngle;
    Point2f leftEyePoint, rightEyePoint;
    vector<Point2f> canthusPts;
    vector<Point2f> nosePts;
    
    
    Mat rotationMatrix;
    bool isTrackingSuccess;
    
    Mat rvec;
    Mat tvec;
    Mat cameraMatrix;
    Mat distCoeffs;
    vector<Point3f> facialPointsIn3D;
    
    fs::path trackerFilePath;
    face_tracker tracker;
    
    
private:
    
    Mat im;
    Mat rotated_img;
};
