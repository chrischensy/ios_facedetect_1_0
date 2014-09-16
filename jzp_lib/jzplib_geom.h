//
//  jzplib_geom.h
//  OPENCV_HOTSHOTS
//
//  Created by Zhiping Jiang on 14-7-18.
//
//

#ifndef __OPENCV_HOTSHOTS__jzplib_geom__
#define __OPENCV_HOTSHOTS__jzplib_geom__

#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <boost/math/special_functions.hpp>
#define Point2f P2F
#define Point Point2i
#define P2F Point2f

#define Size Size2i

#define Rect Rect_<int>

using namespace std;
using namespace cv;


float rad2deg(float radian);
float deg2rad(float degree);
vector<Point2f> Mat2PointsVector(const Mat& mat);
float tileRadianBtwn2Pts(Point2f left, Point2f right);
float calculateEyePairTileAngle(const vector<Point2f>& canthusPts) ;
Point2f caculateEyePairCenter(const vector<Point2f>& canthusPts);
Point2f rotatePointByRotationMatrix(Point2f src,Mat M);
vector<Point2f> rotatePointsByRotationMatrix(const vector<Point2f>& original,const Mat& RM);
void fliplr(vector<Point2f>& points,Size imageSize);

Point rectBL(Rect rect);
Point rectTR(Rect rect);
Point rectBR(Rect rect);

Rect findBiggestSquare(const Mat& original_img);

vector<Point2f> generalizedBezierCurve(vector<Point2f>& controlPoints, vector<int>& pointsBezierOrders, double resolution );

vector<Point2f> curveAnimateSequence(vector<Point2f>& curvePoints, float distancePerFrame);

#endif /* defined(__OPENCV_HOTSHOTS__jzplib_geom__) */


