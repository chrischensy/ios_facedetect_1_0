//
//  jzplib_draw.cpp
//  OPENCV_HOTSHOTS
//
//  Created by Zhiping Jiang on 14-7-18.
//
//

#include "jzplib_draw.h"
#include "GraphUtils.h"

#define Size Size2i
map<string, vector<double> > liveDataMap;


void drawBox(Mat& image, CvRect box, Scalar color, int thick){
    rectangle( image, cvPoint(box.x, box.y), cvPoint(box.x+box.width,box.y+box.height),color, thick);
}

void drawConnectedPoints(Mat& image, vector<Point2f> points, Scalar color, int thick){
    for (int i = 0; i<points.size()-1 && points.size()>0; i++) {
        line(image, points[i], points[i+1], color,thick,CV_AA);
    }
}

void drawPoints(Mat& image, vector<Point2f> points){
    Scalar color=cv::Scalar::all(255);
    for( vector<Point2f>::const_iterator i = points.begin(), ie = points.end(); i != ie; ++i )
    {
        Point center( cvRound(i->x ), cvRound(i->y));
        circle(image,*i,2,color,1);
    }
}


void drawRotatedRect(Mat image, RotatedRect eyeRRect) {
    Point2f rect_points[4];
    eyeRRect.points( rect_points );
    for( int j = 0; j < 4; j++ )
        line( image, rect_points[j], rect_points[(j+1)%4], Scalar(0,0,255), 1, 8 );
    
    
}

void drawStringAtPoint(Mat img, const string text, Point position) {
    
    putText(img,text,Point(position.x+1,position.y+1),FONT_HERSHEY_COMPLEX,0.6f,
            Scalar::all(0),1,CV_AA);
    putText(img,text,Point(position.x,position.y),FONT_HERSHEY_COMPLEX,0.6f,
            Scalar::all(255),1,CV_AA);
    
    
}

void
drawStringAtTopLeftCorner(Mat img,                       //image to draw on
                          const string text)             //text to draw
{
    Size size = getTextSize(text,FONT_HERSHEY_COMPLEX,0.6f,1,NULL);
    drawStringAtPoint(img, text, Point(1,size.height+1));
}

void drawColorHistGram(Mat hist) {
    double maxVal=0;
    minMaxLoc(hist, 0, &maxVal, 0, 0);
    
    int hbins = 30, sbins = 32;
    int scale = 10;
    Mat histImg = Mat::zeros(sbins*scale, hbins*10, CV_8UC3);
    
    for( int h = 0; h < hbins; h++ )
        for( int s = 0; s < sbins; s++ )
        {
            float binVal = hist.at<float>(h, s);
            int intensity = cvRound(binVal*255/maxVal);
            rectangle( histImg, Point(h*scale, s*scale),
                      Point( (h+1)*scale - 1, (s+1)*scale - 1),
                      Scalar::all(intensity),
                      CV_FILLED );
        }
    
    
    namedWindow( "H-S Histogram", 1 );
    imshow( "H-S Histogram", histImg );
}

void imagesc(string windowName, Mat image, int colormap) {
    Mat display;
    float Amin = *min_element(image.begin<float>(), image.end<float>());
    float Amax = *max_element(image.begin<float>(), image.end<float>());
    Mat A_scaled = (image - Amin)/(Amax - Amin);
    A_scaled.convertTo(display, CV_8UC1, 255.0, 0);
    applyColorMap(display, display, cv::COLORMAP_JET);
    imshow(windowName, display);
}

void plotVectors(string windowName, Mat values) {
    Mat data , bgimg;
    values.convertTo(data, CV_32F);

    vector<float>  tempvector;
    if (data.cols >0) {
        data.col(0).copyTo(tempvector);
//        cout<<"vector size"<<tempvector.size()<<endl;
        bgimg = Mat(drawFloatGraph(&tempvector[0], tempvector.size()));
//        cout<<bgimg.size()<<endl;
        tempvector.clear();
    }
    IplImage dst = bgimg;
    if (data.cols>1)
    for (int i = 0 ; i < data.cols; i++) {
        data.col(i).copyTo(tempvector);
        drawFloatGraph(&tempvector[0], tempvector.size(),&dst);
    }
    imshow(windowName, bgimg);
}

void plotLiveData(string windowName, double value, int length) {
    if (liveDataMap.find(windowName) != liveDataMap.end()) {
        liveDataMap[windowName].push_back(value);
        if (liveDataMap[windowName].size()>length) {
            liveDataMap[windowName].erase(liveDataMap[windowName].begin());
        }
        
    } else {
        liveDataMap[windowName] = vector<double>(1);
        liveDataMap[windowName].push_back(value);
        if (liveDataMap[windowName].size()>length) {
            liveDataMap[windowName].erase(liveDataMap[windowName].begin());
        }
    }
    Mat data(liveDataMap[windowName]);
    plotVectors(windowName, data);
    data.release();
}
