//
//  jzplib_bwOps.cpp
//  OPENCV_HOTSHOTS
//
//  Created by Zhiping Jiang on 14-7-18.
//
//

#include "jzplib_bwOps.h"
#define Point2f P2F
#define Point Point2i
#define P2F Point2f

Mat fillHoleInBinary(Mat bwImage) {
    vector<vector<Point> > contours, onlyContours(1);
    vector<Vec4i> hierarchy;
    
    findContours( bwImage.clone(), contours, hierarchy,
                 CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );
    
    Mat mask(bwImage.size(),CV_8UC1,Scalar::all(0));
    
    drawContours(mask, contours, -1, Scalar(255), CV_FILLED);
    
    return mask;
    
}


Mat removeSmallBlobs(Mat bwImage) {
    vector<vector<Point> > contours, onlyContours(1);
    vector<Vec4i> hierarchy;
    
    findContours( bwImage.clone(), contours, hierarchy,
                 CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );
    vector<double> areas(contours.size());
    if (contours.size() >0 ){
        for(size_t i= 0 ; i < contours.size() ; i ++) {
            areas[i] = contourArea(contours[i]);
            //        cout<<areas[i]<<",";
        }
        
        long biggestIndex = distance(areas.begin(), max_element(areas.begin(),areas.end()));
        //    cout<<biggestIndex<<":"<<areas[biggestIndex]<<endl;
        onlyContours[0] =contours[biggestIndex];
        Mat mask(bwImage.size(),CV_8UC1,Scalar::all(0));
        
        drawContours(mask, onlyContours, -1, Scalar(255), CV_FILLED);
        return mask;
    }
    return bwImage;
}

Mat fillConvexHulls(Mat bwImage) {
    vector<vector<Point> > contours, onlyContours(1);
    vector<Vec4i> hierarchy;
    
    findContours( bwImage.clone(), contours, hierarchy,
                 CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE );
    
    // Find the convex hull object for each contour
    vector<vector<Point> >hull( contours.size() );
    for( int i = 0; i < contours.size(); i++ )
    {
        convexHull( Mat(contours[i]), hull[i], false );
    }
    
    Mat drawing = Mat::zeros( bwImage.size(), CV_8UC1 );
    for( int i = 0; i< contours.size(); i++ )
    {
        drawContours(drawing, contours, i, Scalar(255), CV_FILLED);
        drawContours( drawing, hull, i, Scalar(255), CV_FILLED);
    }

    return drawing;
}