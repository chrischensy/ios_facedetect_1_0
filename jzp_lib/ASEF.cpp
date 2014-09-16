//
//  ASEF.cpp
//  OPENCV_HOTSHOTS
//
//  Created by Zhiping Jiang on 14-7-18.
//
//

#include "ASEF.h"


ASEF_Algorithm::ASEF_Algorithm() {
    haar_cascade_path = "resources/EyeLocatorASEF128x128.fel";
    isInitialized = false;
}

int ASEF_Algorithm::initialize() {
    
    if (load_asef_filters(haar_cascade_path, &n_rows, &n_cols, &lrect,
                          &rrect, &lfilter, &rfilter))
        return -1;
    
    
    lfilter_dft = cvCreateMat(n_rows, n_cols, CV_32FC1);
    rfilter_dft = cvCreateMat(n_rows, n_cols, CV_32FC1);
    
    scaled_face_image_32fc1 = cvCreateMat(n_rows, n_cols, CV_32FC1);
    scaled_face_image_8uc1 = cvCreateMat(n_rows, n_cols, CV_8UC1);
    
    lcorr = cvCreateMat(n_rows, n_cols, CV_32FC1);
    rcorr = cvCreateMat(n_rows, n_cols, CV_32FC1);
    
    lroi = cvCreateMatHeader(n_rows, n_cols, CV_32FC1);
    rroi = cvCreateMatHeader(n_rows, n_cols, CV_32FC1);
    
    lut = cvCreateMat(256, 1, CV_32FC1);
    point = cvCreateMat(1, 2, CV_32FC1);
    if (!(lfilter_dft && rfilter_dft && scaled_face_image_32fc1 &&
          scaled_face_image_8uc1 && lcorr && rcorr && lroi &&
          rroi && lut)) {
        return -1;
    }
    
    cvDFT(lfilter, lfilter_dft, CV_DXT_FORWARD, 0);
    cvDFT(rfilter, rfilter_dft, CV_DXT_FORWARD, 0);
    
    cvGetSubRect(lcorr, lroi, lrect);
    cvGetSubRect(rcorr, rroi, rrect);
    
    for (int i = 0; i < 256; i++) {
        cvmSet(lut, i, 0, 1.0 + i);
    }
    cvLog(lut, lut);
    isInitialized = true;
    return 0;
}

void ASEF_Algorithm::detecteyes(Mat face_image) {
    
    if (isInitialized == false)
        this->initialize();
    
    double xscale = ((double) scaled_face_image_8uc1->cols) / ((double) face_image.cols);
    double yscale = ((double) scaled_face_image_8uc1->rows) / ((double) face_image.rows);
    
    CvMat temp = face_image;
    cvResize(&temp, scaled_face_image_8uc1, CV_INTER_LINEAR);
    cvLUT(scaled_face_image_8uc1, scaled_face_image_32fc1, lut);
    cvDFT(scaled_face_image_32fc1, scaled_face_image_32fc1, CV_DXT_FORWARD, 0);
    cvMulSpectrums(scaled_face_image_32fc1, lfilter_dft, lcorr, CV_DXT_MUL_CONJ);
    cvMulSpectrums(scaled_face_image_32fc1, rfilter_dft, rcorr, CV_DXT_MUL_CONJ);
    
    cvDFT(lcorr, lcorr, CV_DXT_INV_SCALE, 0);
    
    cvDFT(rcorr, rcorr, CV_DXT_INV_SCALE, 0);
    
    minMaxLoc(Mat(lroi), &MinValuel, NULL, NULL, &left_eye);
    minMaxLoc(Mat(rroi), &MinValuer, NULL, NULL, &right_eye);
    left_eye.x = (lrect.x + left_eye.x) / xscale;
    left_eye.y = (lrect.y + left_eye.y) / yscale;
    right_eye.x = (rrect.x + right_eye.x) / xscale;
    right_eye.y = (rrect.y + right_eye.y) / yscale;
}

void ASEF_Algorithm::findEyes(Mat image, Rect faceRect) {
    this->findEyes(image(faceRect).clone());
    leftEyePoint+= faceRect.tl();
    rightEyePoint += faceRect.tl();
    leftEyeRect +=faceRect.tl();
    rightEyeRect +=faceRect.tl();
    
}

void ASEF_Algorithm::findEyes(Mat faceImage) {
    vector<Rect> eyeRectsVector(2);
    eyeFound = false;
    if (faceImage.size().area() >0) {
        input_image = faceImage;
        int widthBase  = 150;
        double imageSizeScale = 1.0*faceImage.size().width*1.0/faceImage.size().height;
        Mat color_img = faceImage.clone(), eye_img, gray_img;
        resize(color_img, eye_img, Size(widthBase, cvRound(widthBase/imageSizeScale)));
        cvtColor(eye_img, gray_img, CV_BGR2GRAY);
        equalizeHist(gray_img, gray_img);
        this->detecteyes(gray_img);
        
        double x = 1.0*this->left_eye.x/(double)widthBase*faceImage.size().width;
        double y = 1.0*this->left_eye.y/(double)widthBase*imageSizeScale*faceImage.size().height;
        Point cC(faceImage.size().width/8,faceImage.size().width/14);
        
        leftEyePoint = Point(cvRound(x),cvRound(y));
        leftEyeRect = Rect(0,0,faceImage.size().width/4,faceImage.size().width/7);
        leftEyeRect += leftEyePoint;
        leftEyeRect -= cC;
        
        x = 1.0*this->right_eye.x*1.0/(double)widthBase*faceImage.size().width;
        y = 1.0*this->right_eye.y/(double)widthBase*imageSizeScale*faceImage.size().height;
        
        rightEyePoint = Point(cvRound(x),cvRound(y));
        rightEyeRect = Rect(0,0,faceImage.size().width/4,faceImage.size().width/7);
        rightEyeRect += rightEyePoint;
        rightEyeRect -= cC;
        
        eyeFound = true;
    }
    
}

Rect ASEF_Algorithm::getCanthus_LL_Rect(Rect leftEyeRect) {
    return Rect(leftEyeRect.x+leftEyeRect.width/10*7,leftEyeRect.y+leftEyeRect.height/5*2,leftEyeRect.width/10*3, leftEyeRect.height/5*3);
}

Rect ASEF_Algorithm::getCanthus_RR_Rect(Rect rightEyeRect) {
    return Rect(rightEyeRect.x,rightEyeRect.y+rightEyeRect.height/5*2,rightEyeRect.width/10*3, rightEyeRect.height/5*3);
}

Rect ASEF_Algorithm::getCanthus_LR_Rect(Rect leftEyeRect) {
    return Rect(leftEyeRect.x,leftEyeRect.y,leftEyeRect.width/4, leftEyeRect.height);
}

Rect ASEF_Algorithm::getCanthus_RL_Rect(Rect rightEyeRect) {
    return Rect(rightEyeRect.x+rightEyeRect.width/4*3,rightEyeRect.y,rightEyeRect.width/4, rightEyeRect.height);
}

vector<Point2f> ASEF_Algorithm::getInnerCanthus(Mat input_image, Rect leftEyeRect, Rect rightEyeRect) {
    vector<Point2f> canthus(2);
    Mat gray_img;
    Rect canthus_ll_rect = getCanthus_LL_Rect(leftEyeRect);
    Rect canthus_rr_rect = getCanthus_RR_Rect(rightEyeRect);
    Mat  eyeCornerMask = Mat::zeros(input_image.size(), CV_8UC1);
    eyeCornerMask(canthus_ll_rect) = 255;
    eyeCornerMask(canthus_rr_rect) = 255;
    if (input_image.channels() == 3) {
        cvtColor(input_image, gray_img, CV_BGR2GRAY);
    } else gray_img = input_image;
    
    //    equalizeHist(gray_img(leftEyeRect), gray_img(leftEyeRect));
    //    equalizeHist(gray_img(rightEyeRect), gray_img(rightEyeRect));
    
    Mat masked ;
    gray_img.copyTo(masked, eyeCornerMask);
    
    goodFeaturesToTrack(gray_img, canthus, 2 , 0.05, 40,eyeCornerMask,6,false);
    
    imshow("canthus gray",masked);
    
    if (canthus[0].x > canthus[1].x) {
        swap(canthus[0],canthus[1]);
    }
    
    return canthus;
}

vector<Point2f> ASEF_Algorithm::getOutterCanthus(Mat input_image, Rect leftEyeRect, Rect rightEyeRect)
{
    vector<Point2f> canthus(2);
    Mat gray_img;
    
    Rect canthus_lr_rect = getCanthus_LR_Rect(leftEyeRect);
    Rect canthus_rl_rect = getCanthus_RL_Rect(rightEyeRect);
    
    Mat  eyeCornerMask = Mat::zeros(input_image.size(), CV_8UC1);
    eyeCornerMask(canthus_lr_rect) = 255;
    eyeCornerMask(canthus_rl_rect) = 255;
    Mat eyeCornerMasked ;
    cvtColor(input_image, gray_img, CV_BGR2GRAY);
    goodFeaturesToTrack(gray_img, canthus, 2 , 0.05, 40,eyeCornerMask,6,false);
    
    if (canthus[0].x < canthus[1].x)
        iter_swap(canthus.begin(), canthus.end());
    
    return canthus;
}


 int load_asef_filters(string file_name, int *p_n_rows, int *p_n_cols,
                             CvRect *left_eye_region, CvRect *right_eye_region,
                             CvMat **p_left_filter, CvMat **p_right_filter) {
    
    long rv;
    
    char buf[LINE_BUF_SIZE];
    
    FILE *fp = fopen(file_name.c_str(), "rb");
    
    if (!fp) {
        return -1;
    }
    
    int n_rows, n_cols; // row and column size
    
    
    read_line(fp, buf, LINE_BUF_SIZE);
    printf("%s\n", buf);
    if (strcmp(buf, "CFEL")) { //±»Ωœ◊÷∑˚¥Æs1∫Õs2°£œ‡µ» ±∑µªÿ0£¨◊Û–°∏∫£¨”“–°’˝
        return -1;
    }
    
    // Print comments and copyright
    for (int i = 0; i < 2; i++) {
        if (read_line(fp, buf, LINE_BUF_SIZE) <= 0) {
            return -1;
        }
        printf("%s\n", buf);
    }
    
    read_line(fp, buf, LINE_BUF_SIZE);
    sscanf(buf, "%d %d", &n_rows, &n_cols); // - ¥”“ª∏ˆ◊÷∑˚¥Æ÷–∂¡Ω¯”Î÷∏∂®∏Ò Ωœ‡∑˚µƒ ˝æ›.±£¥ÊŒ™ ÆΩ¯÷∆µƒ–Œ Ω
    *p_n_rows = n_rows;
    *p_n_cols = n_cols;
    
    int rect_x, rect_y, rect_width, rect_hight;
    read_line(fp, buf, LINE_BUF_SIZE);
    sscanf(buf, "%d %d %d %d", &rect_x, &rect_y, &rect_width, &rect_hight);
    
    if (left_eye_region) {
        *left_eye_region = cvRect(rect_x, rect_y, rect_width, rect_hight);
    }
    
    read_line(fp, buf, LINE_BUF_SIZE);
    sscanf(buf, "%d %d %d %d", &rect_x, &rect_y, &rect_width, &rect_hight);
    
    if (right_eye_region) {
        *right_eye_region = cvRect(rect_x, rect_y, rect_width, rect_hight);
    }
    
    uint32_t endien_checker;
    unsigned long endianness;
    read_line(fp, buf, LINE_BUF_SIZE);
    endien_checker = *(uint32_t*) buf;
    
    
    if (!strcmp(buf, "ABCD")) {
        // Big endian
        endianness = 0;
    } else if (!strcmp(buf, "DCBA")) {
        // Little endian
        // Almost always this case on your x86 machine.
        // Not sure about ARM (Android/iOS), you can test it out :)
        endianness = 1;
    } else {
        endianness = -1;
    }
    
    // TODO: handle big endian with byte swap;
    
    size_t filter_data_size = n_rows * n_cols * sizeof (float);
    CvScalar mean, std_dev;
    
    
    unsigned char* lfilter_data = (unsigned char*) malloc(filter_data_size);
    rv = fread(lfilter_data, 1, filter_data_size, fp);
    assert(rv == filter_data_size);
    
    if (p_left_filter) {
        CvMat *left_filter = cvCreateMatHeader(n_rows, n_cols, CV_32FC1);
        cvSetData(left_filter, lfilter_data, CV_AUTO_STEP);
        cvAvgSdv(left_filter, &mean, &std_dev, NULL);
        cvScale(left_filter, left_filter, 1.0 / std_dev.val[0], -mean.val[0]*1.0 / std_dev.val[0]);
        *p_left_filter = left_filter;
        
    } else {
        free(lfilter_data);
    }
    
    unsigned char* rfilter_data = (unsigned char*) malloc(filter_data_size);
    rv = fread(rfilter_data, 1, filter_data_size, fp);
    assert(rv == filter_data_size);
    
    if (p_right_filter) {
        CvMat *right_filter = cvCreateMatHeader(n_rows, n_cols, CV_32FC1);
        cvSetData(right_filter, rfilter_data, CV_AUTO_STEP);
        cvAvgSdv(right_filter, &mean, &std_dev, NULL);
        cvScale(right_filter, right_filter, 1.0 / std_dev.val[0], -mean.val[0]*1.0 / std_dev.val[0]);
        *p_right_filter = right_filter;
    } else {
        free(rfilter_data);
    }
    
    fclose(fp);
    
    return 0;
    
}

int read_line(FILE* fp, char* buf, int size) {
    int c, i = 0;
    while (i < (size - 1) && (c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            break;
        }
        buf[i++] = c;
    }
    
    buf[i] = '\0';
    return i;
}
