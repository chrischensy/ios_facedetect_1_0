//
//  jzplib_detections.cpp
//  OPENCV_HOTSHOTS
//
//  Created by Zhiping Jiang on 14-7-18.
//
//
#include "jzplib_draw.h"
#include "jzplib_detections.h"
#include <queue>
#define Rect Rect_<int>



Rect haarPatternDetection(CascadeClassifier classifier, Mat image, int imageWidthforDetection, Rect parentRect=Rect()) {
    double imageSizeScale = 1.0*image.size().width*1.0/image.size().height;
    Mat color_img = image.clone();
    Mat face_img, gray_img;
    imresize(color_img, imageWidthforDetection,face_img);
    cvtColor(face_img, gray_img, CV_BGR2GRAY);
    equalizeHist(gray_img, gray_img);
    vector<Rect> faceRectsVector;
    classifier.detectMultiScale(gray_img, faceRectsVector,1.2,3,
                                CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE );
    
    Rect faceRect(0,0,0,0);
    
    for (int i = 0; i < faceRectsVector.size(); i++) {
        drawBox(face_img, faceRectsVector[i]);
        if (faceRect.area() < faceRectsVector[i].area())
            faceRect = faceRectsVector[i];
    }
    drawBox(face_img, faceRect);
    //    imshow("rect",face_img);
    double x = 1.0*faceRect.x/(double)imageWidthforDetection;
    double y = 1.0*faceRect.y/(double)cvRound(imageWidthforDetection/imageSizeScale);
    double w = 1.0*faceRect.width/(double)imageWidthforDetection;
    double h = 1.0*faceRect.height/(double)cvRound(imageWidthforDetection/imageSizeScale);
    
    int bx = cvRound(x*image.size().width) ;;//+ parentRect.tl().x;
    int by = cvRound(y*image.size().height) ;//+ parentRect.tl().y;
    int bw = cvRound(w*image.size().width);
    int bh = cvRound(h*image.size().height);
    
    Rect biggerRect(bx,by,bw,bh);
    biggerRect += parentRect.tl();
    
    return biggerRect;
}

Point2f findMassCenter_BinaryBiggestBlob(const Mat& bw_img) {
    Mat full_bw_img = removeSmallBlobs(bw_img);
    threshold(bw_img, bw_img, 254, 255, CV_THRESH_BINARY);
    
    vector<vector<Point> > contours,onlyContours(1);
    
    vector<Vec4i> hierarchy;
    
    findContours( bw_img.clone(), contours, hierarchy,
                 CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE );
    
    if (contours.size() >0) {
        vector<int> areas(contours.size());
        for(size_t i= 0 ; i < contours.size() ; i ++) {
            areas[i] = contourArea(contours[i]);
            //        cout<<areas[i]<<",";
        }
        
        long biggestIndex = distance(areas.begin(), max_element(areas.begin(),areas.end()));
        //    cout<<biggestIndex<<":"<<areas[biggestIndex]<<endl;
        onlyContours[0] =contours[biggestIndex];
        
        /// Get the moments
        vector<Moments> mu(contours.size() );
        for( int i = 0; i < contours.size(); i++ )
        { mu[i] = moments( contours[i], false ); }
        
        ///  Get the mass centers:
        vector<Point2f> mc( contours.size() );
        for( int i = 0; i < contours.size(); i++ )
        { mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); }
        
        return mc[0];
    } else
        return Point(0,0);
}

SymmetryScore_tbb::SymmetryScore_tbb(Mat & gray, Mat & score) : score(score), gray_img(gray){
    flip(gray_img, flipped, 1);
}

void SymmetryScore_tbb::operator() (const cv::Range& range) const {
    Mat temp, base, colsum;
    for (int i = range.start ; i < range.end; i++) {
        
        if (i<gray_img.cols/2) {
            flipped.colRange(flipped.cols-i-1, flipped.cols).copyTo(temp);
            gray_img.colRange(i+1, gray_img.cols).copyTo(base);
        } else {
            gray_img.colRange(i, gray_img.cols).copyTo(temp);
            flipped.colRange(flipped.cols-i, flipped.cols).copyTo(base);
        }
        base.colRange(0, temp.cols) += temp;
        reduce(base, colsum, 0, CV_REDUCE_SUM,CV_32FC1);
        colsum =  colsum.t();
        colsum.copyTo(score.colRange(i, i+1).rowRange(0, colsum.rows));
    }
}


Mat calculateImageSymmetryScore(const Mat& image) {
    Mat gray_img ;
    
    if (image.channels() == 3) {
        cvtColor(image, gray_img, CV_BGR2GRAY);
    } else {
        gray_img = image.clone();
    }
    
    if (gray_img.cols %2 == 1) {
        hconcat(gray_img, gray_img.col(gray_img.cols-1), gray_img);
    }
    gray_img.convertTo(gray_img, CV_32FC1);
    Mat score(gray_img.cols,gray_img.cols,CV_32FC1,Scalar::all(0));
    
    parallel_for_(Range(0,gray_img.cols), SymmetryScore_tbb(gray_img, score));
    
    return score;
}

void findEyeCenterByColorSegmentation(const Mat& image, Point2f & eyeCoord, float coordinateWeight, int kmeansIterations, int kmeansRepeats, int blurSize)  {
    
    Mat img, gray_img;
    Mat colorpoints, kmeansPoints;
    
    img = equalizeImage(image);
    
    medianBlur(img, img, blurSize);
    cvtColor(image, gray_img, CV_BGR2GRAY);
    gray_img = imcomplement(gray_img);
    vector<Mat> layers(3);
    split(img, layers);
    for (int i = 0 ; i < layers.size(); i++) {
        layers[i] = layers[i].reshape(1,1).t();
    }
    hconcat(layers, colorpoints);
    
    // add coordinates
    colorpoints.convertTo(colorpoints, CV_32FC1);
    Mat coordinates = matrixPointCoordinates(img.rows,img.cols,false) *coordinateWeight;
    hconcat(colorpoints, coordinates, kmeansPoints);
    
    Mat locIndex(img.size().area(),kmeansIterations,CV_32FC1,Scalar::all(-1));
    linspace(0, img.size().area(), 1).copyTo(locIndex.col(0));
    Mat index_img(img.rows,img.cols,CV_32FC1,Scalar::all(0));
    Mat bestLabels, centers, clustered , colorsum , minColorPtIndex;
    for(int it = 1 ; it < kmeansIterations ; it++) {
        if (kmeansPoints.rows < 2) {
            break;
        }
        kmeans(kmeansPoints,2,bestLabels,TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, kmeansRepeats, 0.001),kmeansRepeats,KMEANS_PP_CENTERS,centers);
        reduce(centers.colRange(0, 3), colorsum, 1, CV_REDUCE_SUM);
        
        if (colorsum.at<float>(0) < colorsum.at<float>(1)) {
            
            findNonZero(bestLabels==0, minColorPtIndex);
        }
        else {
            findNonZero(bestLabels==1, minColorPtIndex);
        }
        
        minColorPtIndex = minColorPtIndex.reshape(1).col(1);
        
        for (int  i = 0; i <minColorPtIndex.rows ; i ++) {
            locIndex.at<float>(i,it) = locIndex.at<float>(minColorPtIndex.at<int>(i),it-1);
        }
        Mat temp;
        for (int  i = 0; i <minColorPtIndex.rows ; i ++) {
            temp.push_back(kmeansPoints.row(minColorPtIndex.at<int>(i)));
        }
        temp.copyTo(kmeansPoints);
        temp.release();
        for (int i = 0 ; i < minColorPtIndex.rows ; i ++) {
            int r, c;
            ind2sub(locIndex.at<float>(i,it), index_img.cols, index_img.rows, r, c);
            index_img.at<float>(r,c) +=1;
        }
    }
    //    imagesc("layered",mat2gray(index_img));
    Mat layerweighted_img = index_img.mul(index_img);
    layerweighted_img = mat2gray(layerweighted_img);
    gray_img.convertTo(gray_img, CV_32FC1,1/255.0);
    Mat composed  = gray_img.mul(layerweighted_img);
    float zoomRatio = 5.0f;
    Mat zoomed;
    imresize(composed, zoomRatio, zoomed);
    Mat score = calculateImageSymmetryScore(zoomed);
//    imagesc("score", score);
    Mat scoresum;
    reduce(score.rowRange(0, zoomed.cols/6), scoresum, 0, CV_REDUCE_SUM,CV_32FC1);
//    plotVectors("scoresum", scoresum.t());
    double minVal , maxVal;
    Point minLoc, maxLoc;
    minMaxLoc(scoresum,&minVal,&maxVal,&minLoc,&maxLoc);
    float initialHC = (float)maxLoc.x/zoomRatio;
    line(zoomed, Point(maxLoc.x,0), Point(maxLoc.x,zoomed.rows-1), Scalar::all(255));
//    imshow("zoomed", zoomed);
    int bestx = 0,bestlayer = 0;
    Mat bestIndex_img = index_img >=1;
    minMaxLoc(index_img,&minVal,&maxVal,&minLoc,&maxLoc);
    for (int i = 1 ; i<=maxVal; i++) {
        Mat indexlayer_img = index_img >=i;
        medianBlur(indexlayer_img, indexlayer_img, 5);
        erode(indexlayer_img, indexlayer_img, blurSize);
        erode(indexlayer_img, indexlayer_img, blurSize);
        indexlayer_img = removeSmallBlobs(indexlayer_img);
        
        indexlayer_img = fillHoleInBinary(indexlayer_img);
        indexlayer_img = fillConvexHulls(indexlayer_img);
        Mat score = calculateImageSymmetryScore(indexlayer_img);
        Mat scoresum;
        reduce(score.rowRange(0, indexlayer_img.cols/6), scoresum, 0, CV_REDUCE_SUM,CV_32FC1);
        minMaxLoc(scoresum,&minVal,&maxVal,&minLoc,&maxLoc);
        if (abs(maxLoc.x - initialHC) < abs(bestx - initialHC)) {
            
            if (sum(indexlayer_img)[0]/255 < indexlayer_img.size().area()/5*2 &&
                sum(indexlayer_img)[0]/255 > indexlayer_img.size().area()/6) {
                bestx = maxLoc.x;
                bestlayer = i;
                bestIndex_img = indexlayer_img.clone();
                
            }
            
        }
    }
    
    Point2f massCenter = findMassCenter_BinaryBiggestBlob(bestIndex_img);
    
    
    eyeCoord =  Point2f(initialHC,massCenter.y);
}


