//
//  jzplib_utils.h
//  JZP_EYE_TRACKING
//
//  Created by Zhiping Jiang on 14-8-23.
//
//

#ifndef __JZP_EYE_TRACKING__jzplib_utils__
#define __JZP_EYE_TRACKING__jzplib_utils__

#include <iostream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/timer/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/asio.hpp>
#include <boost/asio/time_traits.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <deque>
#include <numeric> 

using namespace std;
using namespace boost::timer;
using namespace boost::posix_time;
using namespace boost::asio;
namespace io = boost::iostreams;
namespace fs = boost::filesystem;

bool is_number(const std::string& s);
bool copyDirRecursively(
             boost::filesystem::path const & source,
             boost::filesystem::path const & destination
             );
vector<fs::path> listFilesWithExtension(fs::path folderPath, string prefix, string surfix);
vector<fs::path> listFilesRecursivelyWithExtension(fs::path folderPath, string prefix, string surfix)
;
std::string simpleDataFormat(boost::posix_time::ptime now, std::string format);

class LowpassFPSTimer {
private:
    ptime tickTime, tockTime;
    deque<double> durationValues;
    int lowpassLength;
public:
    LowpassFPSTimer(int lowpassLength = 50);
    void tick();
    double tock();
};

class FPScontroller {
public:
    FPScontroller (int fps);
    void controlledDelay();
    
private:
    boost::asio::deadline_timer * blockTimer;
    int fps;
    time_duration td;
    ptime currentFrameTimestamp;
    ptime nextFrameTimestamp ;
};

class BlockDelayer {
public:
    static void delay_milisec(double milisec);
    static void delay_microsec(double microsec);
    static void delay_sec(double sec);
private:
    static boost::asio::deadline_timer * blockTimer;
    static void delay_microsec_private(double microsec);
    
};

class CSVFileWriter {
public:
    void addSlot(vector<float> slot);
    void setDelimeter(const string delimeter);
    void writeToFile(boost::filesystem::path filePath);
private:
    vector< vector<float> > slots;
    string delimeter = " ";
};

#endif /* defined(__JZP_EYE_TRACKING__jzplib_utils__) */
