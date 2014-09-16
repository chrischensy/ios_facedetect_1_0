//
//  jzplib_img.cpp
//  OPENCV_HOTSHOTS
//
//  Created by Zhiping Jiang on 14-7-18.
//
//

#include "jzplib_img.h"
#include "jzplib_geom.h"
#include "jzplib_utils.h"



Mat sobelEdgeDerivatives(Mat src) {
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;
    Mat src_gray;
    //    GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
    
    if (src.channels() == 3) {
        cvtColor( src, src_gray, CV_BGR2GRAY );
    } else
        src_gray = src;
    
    
    Mat grad_x, grad_y, grad;
    Mat abs_grad_x, abs_grad_y;
    
    /// Gradient X
    //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
    Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );
    
    /// Gradient Y
    //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
    Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );
    
    /// Total Gradient (approximate)
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
    equalizeHist(grad, grad);
    return grad;
    
}

Mat magicEqualHist(Mat faceimg) {
    Mat gray_img = faceimg.clone();
    int w = gray_img.cols;
    int h = gray_img.rows;
    Mat wholeFace;
    equalizeHist(gray_img, wholeFace); int midX = w/2;
    Mat leftSide = gray_img(Rect(0,0, midX,h));
    Mat rightSide = gray_img(Rect(midX,0, w-midX,h));
    equalizeHist(leftSide, leftSide);
    equalizeHist(rightSide, rightSide);
    
    for (int y=0; y<h; y++) {
        for (int x=0; x<w; x++) {
            int v;
            if (x < w/4) {
                // Left 25%: just use the left face.
                v = leftSide.at<uchar>(y,x);
            }
            else if (x < w*2/4) {
                // Mid-left 25%: blend the left face & whole face.
                int lv = leftSide.at<uchar>(y,x);
                int wv = wholeFace.at<uchar>(y,x);
                // Blend more of the whole face as it moves
                // further right along the face.
                float f = (x - w*1/4) / (float)(w/4);
                v = cvRound((1.0f - f) * lv + (f) * wv);
            }
            else if (x < w*3/4) {
                // Mid-right 25%: blend right face & whole face.
                int rv = rightSide.at<uchar>(y,x-midX);
                int wv = wholeFace.at<uchar>(y,x);
                // Blend more of the right-side face as it moves
                // further right along the face.
                float f = (x - w*2/4) / (float)(w/4);
                v = cvRound((1.0f - f) * wv + (f) * rv);
            }
            else {
                // Right 25%: just use the right face.
                v = rightSide.at<uchar>(y,x-midX);
            }
            gray_img.at<uchar>(y,x) = v;
        }// end x loop
    }//end y loop
    
    
    return gray_img;
}

Mat imresize(const Mat& image, float scale) {
    Mat dst;
    imresize(image, scale, dst);
    return dst;
}

void imresize(const Mat& image, float scale, Mat& dst) {
    int imageWidthforDetection =image.size().width*scale;
    double imageSizeScale = 1.0*image.size().width*1.0/image.size().height;
    if (scale != 1.0f) {
        resize(image, dst, Size(imageWidthforDetection, cvRound(imageWidthforDetection/imageSizeScale)),0.0,0.0);
    } else if (scale == 1.0f)
        image.copyTo(dst);
}

void imresize(const Mat& image, int imageWidthforDetection, Mat& dst) {
    double imageSizeScale = 1.0*image.size().width*1.0/image.size().height;
    if (imageSizeScale != 1.0f) {
        resize(image, dst, Size(imageWidthforDetection, cvRound(imageWidthforDetection/imageSizeScale)),0.0,0.0);
    } else if (imageSizeScale == 1.0f)
        image.copyTo(dst);
}




Mat calculateColorHistogram(Mat image,Mat mask, int dims = 2, int colorSpace = CV_RGB2HSV) {
    Mat hsv, hist;
    cvtColor(image,hsv, colorSpace);
    const int channels1[] = {1};
    const int channels2[] = {0,1};
    const int channels3[] = {0, 1, 2};
    const int hbins = 100, sbins = 100, vbins = 100;
    const int hist1Size[] = { hbins , sbins};
    const int hist2Size[] = { hbins , sbins};
    const int hist3Size[] = { hbins , sbins, vbins};
    const float hranges[] = {0,180}; 
    const float sranges[] = {0,256};
    const float vranges[] = {0,100};
    const float* hist1Ranges[] = {hranges};
    const float* hist2Ranges[] = {hranges , sranges};
    const float* hist3Ranges[] = {hranges , sranges, vranges};
    switch (dims) {
        case 1:
            calcHist(&hsv, 1, channels1, mask, hist, dims, hist1Size, hist1Ranges);
            break;
        case 2:
            calcHist(&hsv, 1, channels2, mask, hist, dims, hist2Size, hist2Ranges);
            break;
        case 3:
            calcHist(&hsv, 1, channels3, mask, hist, dims, hist3Size, hist3Ranges);
            break;
        default:
            break;
    }
    Mat hist2 = hist;
    //    cout<<hist2.channels()<<" "<<hist2.dims<<endl;
    if (hist2.dims <=2)
        normalize( hist2, hist2, 0, 255, NORM_MINMAX);
    
    return hist;
    
}

Mat calculateColorBackProjection(Mat image, Mat hist,int colorSpace = CV_RGB2HSV) {
    Mat hsv ;
    cvtColor(image, hsv, colorSpace);
    Mat backproj;
    const int channels1[] = {1};
    const int channels2[] = {0,1};
    const int channels3[] = {0, 1, 2};
    const float hranges[] = {0,180};   //hue值取值范围是【0,179】
    const float sranges[] = {0,256};
    const float vranges[] = {0,100};
    const float* hist1Ranges[] = {hranges};
    const float* hist2Ranges[] = {hranges , sranges};
    const float* hist3Ranges[] = {hranges , sranges, vranges};
    //    cout<<hist.dims<<","<<hist.channels()<<" "<<hist.cols<<endl;
    switch (hist.dims) {
        case 2:
            if (hist.cols == 1)
                calcBackProject( &hsv, 1, channels1, hist, backproj, hist1Ranges,255);
            else
                calcBackProject( &hsv, 1, channels2, hist, backproj, hist2Ranges,255);
            break;
        case 3:
            calcBackProject( &hsv, 1, channels3, hist, backproj, hist3Ranges,255);
            break;
            
    }
    return backproj;
}

Mat rgbScaleColorSpace(Mat rgb) {
    vector<Mat> rgbChannels, scaledChannels(3);
    Mat scaled ;
    rgb.convertTo(rgb, CV_32FC3);
    split(rgb,rgbChannels);
    scaledChannels[0] = rgbChannels[0]/rgbChannels[1];
    scaledChannels[1] = rgbChannels[0]/rgbChannels[2];
    scaledChannels[2] = rgbChannels[1]/rgbChannels[2];
    merge(scaledChannels,scaled);
    
    //    cvtColor(scaled, scaled, CV_BGR2GRAY);
    return scaled;
}


Mat imcomplement(Mat gray) {
    return Scalar::all(255) - gray;
}

Mat matrixPointCoordinates(int rows, int cols,bool matlabColumnFirst)  {
//    cout<<s<<endl;
    Mat rowtemplate =linspace(0,cols, 1).t();
    Mat row = Mat(rows, 1, CV_32FC1, Scalar::all(1)) * rowtemplate;
//    cout<<row.size()<<endl;
    Mat col = linspace(0, rows, 1)* Mat(1,cols,CV_32FC1,Scalar::all(1));
//    cout<<col.size()<<endl;
    Mat coordiantes;
    if (matlabColumnFirst) {
        hconcat(row.reshape(1,1).t(), col.reshape(1,1).t(), coordiantes);
    } else
        hconcat(col.reshape(1,1).t(), row.reshape(1,1).t(), coordiantes);
    
    return coordiantes;
}


int sub2ind(const int row,const int col,const int cols,const int rows)
{
    return row*cols+col;
}

void ind2sub(const int sub,const int cols,const int rows,int &row,int &col)
{
    row=sub/cols;
    col=sub%cols;
}

Mat equalizeImage(const Mat& inputImage)
{
    if(inputImage.channels() >= 3)
    {
        Mat ycrcb;
        
        cvtColor(inputImage,ycrcb,CV_BGR2YCrCb);
        
        vector<Mat> channels;
        split(ycrcb,channels);
        
        equalizeHist(channels[0], channels[0]);
        
        Mat result;
        merge(channels,ycrcb);
        
        cvtColor(ycrcb,result,CV_YCrCb2BGR);
        
        return result;
    }
    
    if (inputImage.channels() == 1) {
        Mat gray ;
        equalizeHist(inputImage, gray);
        return gray;
    }
    return Mat();
}

