//
//  ViewController.m
//  Test
//
//  Created by csy on 14-9-15.
//  Copyright (c) 2014年 csy. All rights reserved.
//

#import "ViewController.h"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
using namespace cv;

@interface ViewController ()

@end

@implementation ViewController
@synthesize imageView;
@synthesize videoCamera;
- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    self.videoCamera = [[CvVideoCamera alloc] initWithParentView:imageView];
    self.videoCamera.delegate = self;
    self.videoCamera.defaultAVCaptureDevicePosition = AVCaptureDevicePositionFront;
    self.videoCamera.defaultAVCaptureSessionPreset = AVCaptureSessionPreset352x288;
    self.videoCamera.defaultAVCaptureVideoOrientation = AVCaptureVideoOrientationPortrait;
    self.videoCamera.defaultFPS = 60;
    self.videoCamera.grayscaleMode = NO;
    NSString * filePath  = [[NSBundle mainBundle]pathForResource:@"trackermodel" ofType:@"yaml"];
    //    NSLog(filePath);
    
    fs::path baseDirPath([filePath UTF8String]);
    pupilTracker.initASM_Gaze_Tracker(baseDirPath);
    
}

- (void)processImage:(Mat&)image;
{
    // Do some OpenCV stuff with the image
    Mat origin, im ;
    cvtColor(image, origin, CV_BGRA2BGR);
    
    float zoomRatio = 1.0f;
    
    imresize(origin,zoomRatio,im);
    bool succeeded = pupilTracker.featureTracking(im);
    if (succeeded)
        pupilTracker.calculatePupilCenter();

    fflush(stdout);
    
    if (succeeded) {
        drawPoints(im, pupilTracker.canthusPts);
        drawPoints(im, pupilTracker.nosePts);
        circle(im, pupilTracker.leftEyePoint, 3, Scalar(0,255,0));
        circle(im, pupilTracker.rightEyePoint, 3, Scalar(0,255,0));
    }

    
    
    cvtColor(im, image, CV_BGR2BGRA);
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)start:(id)sender {
    [videoCamera start];
}

- (IBAction)reDetectFace:(id)sender {
    pupilTracker.reDetectFace();
}
@end
