//
//  skinColorDetection.cpp
//  OPENCV_HOTSHOTS
//
//  Created by Zhiping Jiang on 14-7-18.
//
//




//
//  SkinDetector.h
//  EyeTrackingFast_XCODE
//
//  Created by Zhiping Jiang on 14-6-30.
//  Copyright (c) 2014年 Zhiping Jiang. All rights reserved.
//

#ifndef EyeTrackingFast_XCODE_SkinDetector_h
#define EyeTrackingFast_XCODE_SkinDetector_h
#include "skinColorDetection.h"



using namespace cv;
using namespace std;

// skin region location using rgb limitation
Mat SkinRGB(Mat rgb_img)
{
    IplImage ipl_rgb = IplImage(rgb_img);
    IplImage* rgb = &ipl_rgb;
    IplImage* dst=cvCreateImage(cvGetSize(rgb),8,3);
    cvZero(dst);
    static const int R=2;
    static const int G=1;
    static const int B=0;
    
    
    
    for (int h=0;h<rgb->height;h++) {
        unsigned char* prgb=(unsigned char*)rgb->imageData+h*rgb->widthStep;
        unsigned char* pdst=(unsigned char*)dst->imageData+h*dst->widthStep;
        for (int w=0;w<rgb->width;w++) {
            if ((prgb[R]>95 && prgb[G]>40 && prgb[B]>20 &&
                 prgb[R]-prgb[B]>15 && prgb[R]-prgb[G]>15/*&&
                                                          !(prgb[R]>170&&prgb[G]>170&&prgb[B]>170)*/)||//uniform illumination
                (prgb[R]>200 && prgb[G]>210 && prgb[B]>170 &&
                 abs(prgb[R]-prgb[B])<=15 && prgb[R]>prgb[B]&& prgb[G]>prgb[B])//lateral illumination
                ) {
                memcpy(pdst,prgb,3);
            }
            prgb+=3;
            pdst+=3;
        }
    }
    
    return Mat(dst);
}


// skin detection in rg space
Mat cvSkinRG(Mat rgb_img)
{
	IplImage ipl_rgb = IplImage(rgb_img);
    IplImage* rgb = &ipl_rgb;
    
    IplImage* gray=cvCreateImage(cvGetSize(rgb),8,3);
    cvZero(gray);
	
	const int R=2;
	const int G=1;
	const int B=0;
    
	double Aup=-1.8423;
	double Bup=1.5294;
	double Cup=0.0422;
	double Adown=-0.7279;
	double Bdown=0.6066;
	double Cdown=0.1766;
	for (int h=0;h<rgb->height;h++) {
		unsigned char* pGray=(unsigned char*)gray->imageData+h*gray->widthStep;
		unsigned char* pRGB=(unsigned char* )rgb->imageData+h*rgb->widthStep;
		for (int w=0;w<rgb->width;w++)
		{
			int s=pRGB[R]+pRGB[G]+pRGB[B];
			double r=(double)pRGB[R]/s;
			double g=(double)pRGB[G]/s;
			double Gup=Aup*r*r+Bup*r+Cup;
			double Gdown=Adown*r*r+Bdown*r+Cdown;
			double Wr=(r-0.33)*(r-0.33)+(g-0.33)*(g-0.33);
			if (g<Gup && g>Gdown && Wr>0.004)
			{
				*pGray=255;
			}
			else
			{
				*pGray=0;
			}
			pGray++;
			pRGB+=3;
		}
	}
    return Mat(gray);
}
// implementation of otsu algorithm
// author: onezeros#yahoo.cn
// reference: Rafael C. Gonzalez. Digital Image Processing Using MATLAB
void cvThresholdOtsu(IplImage* src, IplImage* dst)
{
	int height=src->height;
	int width=src->width;
    
	//histogram
	float histogram[256]={0};
	for(int i=0;i<height;i++) {
		unsigned char* p=(unsigned char*)src->imageData+src->widthStep*i;
		for(int j=0;j<width;j++) {
			histogram[*p++]++;
		}
	}
	//normalize histogram
	int size=height*width;
	for(int i=0;i<256;i++) {
		histogram[i]=histogram[i]/size;
	}
    
	//average pixel value
	float avgValue=0;
	for(int i=0;i<256;i++) {
		avgValue+=i*histogram[i];
	}
    
	int threshold = 0;
	float maxVariance=0;
	float w=0,u=0;
	for(int i=0;i<256;i++) {
		w+=histogram[i];
		u+=i*histogram[i];
        
		float t=avgValue*w-u;
		float variance=t*t/(w*(1-w));
		if(variance>maxVariance) {
			maxVariance=variance;
			threshold=i;
		}
	}
    
	cvThreshold(src,dst,threshold,255,CV_THRESH_BINARY);
}

Mat cvSkinOtsu(Mat rgb_img)
{
    IplImage ipl_rgb = IplImage(rgb_img);
    IplImage* src = &ipl_rgb;
    IplImage* dst=cvCreateImage(cvGetSize(src),8,1);
    cvZero(dst);
    
	IplImage* ycrcb=cvCreateImage(cvGetSize(src),8,3);
	IplImage* cr=cvCreateImage(cvGetSize(src),8,1);
	cvCvtColor(src,ycrcb,CV_BGR2YCrCb);
	cvSplit(ycrcb,0,cr,0,0);
    
	cvThresholdOtsu(cr,cr);
	cvCopy(cr,dst);
	cvReleaseImage(&cr);
	cvReleaseImage(&ycrcb);
    
    return Mat(dst);
}

Mat cvSkinYUV(Mat rgb_img, bool grayOrColor)
{
    IplImage ipl_rgb = IplImage(rgb_img);
    IplImage* src = &ipl_rgb;
    IplImage* dst=cvCreateImage(cvGetSize(src),8,3);
    IplImage* gray=cvCreateImage(cvGetSize(src),8,1);
    cvZero(dst);
    
	IplImage* ycrcb=cvCreateImage(cvGetSize(src),8,3);
	//IplImage* cr=cvCreateImage(cvGetSize(src),8,1);
	//IplImage* cb=cvCreateImage(cvGetSize(src),8,1);
	cvCvtColor(src,ycrcb,CV_BGR2YCrCb);
	//cvSplit(ycrcb,0,cr,cb,0);
    
	static const int Cb=2;
	static const int Cr=1;
    //	static const int Y=0;
    
	
	cvZero(dst);
    
	for (int h=0;h<src->height;h++) {
        unsigned char* pGray=(unsigned char*)gray->imageData+h*gray->widthStep;
		unsigned char* pycrcb=(unsigned char*)ycrcb->imageData+h*ycrcb->widthStep;
		unsigned char* psrc=(unsigned char*)src->imageData+h*src->widthStep;
		unsigned char* pdst=(unsigned char*)dst->imageData+h*dst->widthStep;
		for (int w=0;w<src->width;w++) {
			if (pycrcb[Cr]>=133&&pycrcb[Cr]<=173&&pycrcb[Cb]>=77&&pycrcb[Cb]<=127)
			{
                memcpy(pdst,psrc,3);
                *pGray = 255;
			} else {
                *pGray = 0;
            }
			pycrcb+=3;
			psrc+=3;
			pdst+=3;
            pGray ++;
		}
	}
	//cvCopyImage(dst,_dst);
	//cvReleaseImage(&dst);
    if (grayOrColor)
        return Mat(gray);
    else return Mat(dst);
}

Mat cvSkinHSV(Mat rgb_img)
{
    IplImage ipl_rgb = IplImage(rgb_img);
    IplImage* src = &ipl_rgb;
    IplImage* dst=cvCreateImage(cvGetSize(src),8,3);
    cvZero(dst);
    
	IplImage* hsv=cvCreateImage(cvGetSize(src),8,3);
	//IplImage* cr=cvCreateImage(cvGetSize(src),8,1);
	//IplImage* cb=cvCreateImage(cvGetSize(src),8,1);
	cvCvtColor(src,hsv,CV_BGR2HSV);
	//cvSplit(ycrcb,0,cr,cb,0);
    
	static const int H=0;
    
	//IplImage* dst=cvCreateImage(cvGetSize(_dst),8,3);
	cvZero(dst);
    
	for (int h=0;h<src->height;h++) {
		unsigned char* phsv=(unsigned char*)hsv->imageData+h*hsv->widthStep;
		unsigned char* psrc=(unsigned char*)src->imageData+h*src->widthStep;
		unsigned char* pdst=(unsigned char*)dst->imageData+h*dst->widthStep;
		for (int w=0;w<src->width;w++) {
			if (phsv[H]>=7&&phsv[H]<=29)
			{
                memcpy(pdst,psrc,3);
			}
			phsv+=3;
			psrc+=3;
			pdst+=3;
		}
	}
	//cvCopyImage(dst,_dst);
	//cvReleaseImage(&dst);
    return Mat(dst);
}


#endif
