//
//  ViewController.h
//  Test
//
//  Created by csy on 14-9-15.
//  Copyright (c) 2014年 csy. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <opencv2/highgui/ios.h>
#include "ASMGazeTracker.h"

@interface ViewController : UIViewController<CvVideoCameraDelegate>{
    CvVideoCamera* videoCamera;
    ASM_Gaze_Tracker pupilTracker;
}
@property (nonatomic,strong) CvVideoCamera* videoCamera;
@property (weak, nonatomic) IBOutlet UIImageView *imageView;
- (IBAction)start:(id)sender;
- (IBAction)reDetectFace:(id)sender;

@end
