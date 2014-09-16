/*****************************************************************************
 *   Non-Rigid Face Tracking
 ******************************************************************************
 *   by Jason Saragih, 5th Dec 2012
 *   http://jsaragih.org/
 ******************************************************************************
 *   Ch6 of the book "Mastering OpenCV with Practical Computer Vision Projects"
 *   Copyright Packt Publishing 2012.
 *   http://www.packtpub.com/cool-projects-with-opencv/book
 *****************************************************************************/
/*
 ft: header file for ft module
 Jason Saragih (2012)
 */

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "ft_data.hpp"
#include "patch_model.hpp"
#include "shape_model.hpp"
#include "face_detector.hpp"
#include "face_tracker.hpp"
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;


//==============================================================================
template <class T>
T load_ft(const string fname){
    T x;
    FileStorage f(fname,FileStorage::READ);
    f["ft object"] >> x; f.release(); return x;
}
//==============================================================================
template<class T>
void save_ft(const string fname,const T& x){
    FileStorage f(fname,FileStorage::WRITE);
    f << "ft object" << x; f.release();
}

template<class T>
void
write(FileStorage& fs,
      const string&,
      const T& x)
{
    x.write(fs);
}

template<class T>
void
read(const FileNode& node,
     T& x,
     const T& d)
{
    if(node.empty())x = d; else x.read(node);
}

bool is_number_asm(const std::string& s);
ft_data load_ft_jzp(string fname);
