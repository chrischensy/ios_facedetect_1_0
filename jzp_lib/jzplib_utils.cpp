//
//  jzplib_utils.cpp
//  JZP_EYE_TRACKING
//
//  Created by Zhiping Jiang on 14-8-23.
//
//

#include "jzplib_utils.h"

bool is_number(const std::string& s)
{
    try
    {
        boost::lexical_cast<double>(s);
        return true;
    }
    catch(boost::bad_lexical_cast &)
    {
        return false;
    }
}

bool copyDirRecursively(
             boost::filesystem::path const & source,
             boost::filesystem::path const & destination
             )
{
    namespace fs = boost::filesystem;
    try
    {
        // Check whether the function call is valid
        if(
           !fs::exists(source) ||
           !fs::is_directory(source)
           )
        {
            std::cerr << "Source directory " << source.string()
            << " does not exist or is not a directory." << '\n'
            ;
            return false;
        }
        if(fs::exists(destination))
        {
            std::cerr << "Destination directory " << destination.string()
            << " already exists." << '\n'
            ;
            return false;
        }
        // Create the destination directory
        if(!fs::create_directory(destination))
        {
            std::cerr << "Unable to create destination directory"
            << destination.string() << '\n'
            ;
            return false;
        }
    }
    catch(fs::filesystem_error const & e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    // Iterate through the source directory
    for(
        fs::directory_iterator file(source);
        file != fs::directory_iterator(); ++file
        )
    {
        try
        {
            fs::path current(file->path());
            if(fs::is_directory(current))
            {
                // Found directory: Recursion
                if(
                   !copyDirRecursively(
                            current,
                            destination / current.filename()
                            )
                   )
                {
                    return false;
                }
            }
            else
            {
                // Found file: Copy
                fs::copy_file(
                              current,
                              destination / current.filename()
                              );
            }
        }
        catch(fs::filesystem_error const & e)
        {
            std:: cerr << e.what() << '\n';
        }
    }
    return true;
}

vector<fs::path> listFilesRecursivelyWithExtension(fs::path folderPath, string prefix, string surfix)
{
    vector<fs::path> files;
    fs::recursive_directory_iterator iterend;
    for (fs::recursive_directory_iterator it(folderPath); it != iterend; it++) {
        if (boost::ends_with(it->path().filename().string(), surfix)
            && boost::starts_with(it->path().filename().string(), prefix) ) {
            files.push_back(it->path());
        }
    }
    return files;
}


vector<fs::path> listFilesWithExtension(fs::path folderPath, string prefix, string surfix) {
    vector<fs::path> files;
    fs::directory_iterator iterend;
    for (fs::directory_iterator it(folderPath); it != iterend; it++) {
        if (boost::ends_with(it->path().filename().string(), surfix)
            && boost::starts_with(it->path().filename().string(), prefix) ) {
            files.push_back(it->path());
        }
    }
    return files;
}

std::string simpleDataFormat(boost::posix_time::ptime date_time, std::string format)
{
    static boost::posix_time::time_facet * facet =
    new boost::posix_time::time_facet(format.c_str());
    std::ostringstream stream;
    stream.imbue(std::locale(stream.getloc(), facet));
    stream << date_time;
//    free(facet);
    return stream.str();
}

LowpassFPSTimer::LowpassFPSTimer(int lowpassLength) {
    this->lowpassLength = lowpassLength;
    durationValues = deque<double>();
    for (int i = 0 ; i < lowpassLength; i++) {
        durationValues.push_back(999.0);
    }
}

void LowpassFPSTimer::tick() {
    tickTime = microsec_clock::local_time();
}

double LowpassFPSTimer::tock() {
    tockTime = microsec_clock::local_time();
    durationValues.pop_front();
    durationValues.push_back((double)time_duration(tockTime-tickTime).total_milliseconds()/1000.0);
    
    double sum = 0.0;
    for (int i = 0; i < durationValues.size(); i++) {
        sum += durationValues[i];
    }
    sum /= durationValues.size();
    return sum;
}


FPScontroller::FPScontroller(int fps) {
    this->fps = fps;
    boost::asio::io_service ioservice;
    blockTimer = new deadline_timer(ioservice);
    nextFrameTimestamp = boost::asio::time_traits<boost::posix_time::ptime>::now();
}

void FPScontroller::controlledDelay() {
    if (fps <= 0 ) {
        return;
    }
    nextFrameTimestamp = nextFrameTimestamp + microsec(1000000/fps);
    blockTimer->expires_at(nextFrameTimestamp);
    blockTimer->wait();
    
}

boost::asio::deadline_timer * BlockDelayer::blockTimer;

void BlockDelayer::delay_microsec_private(double microsec) {
     if (blockTimer == NULL) {
         boost::asio::io_service ioservice;
         blockTimer = new deadline_timer(ioservice);
     }
    blockTimer->expires_from_now(boost::posix_time::microseconds(microsec));
    blockTimer->wait();
}

void BlockDelayer::delay_microsec(double microsec) {
    BlockDelayer::delay_microsec_private(microsec);
}

void BlockDelayer::delay_milisec(double milisec) {
    BlockDelayer::delay_microsec_private(milisec*1000);
}

void BlockDelayer::delay_sec(double sec) {
    BlockDelayer::delay_microsec_private(sec*1e6);
}

void CSVFileWriter::addSlot(vector<float> slot) {
    slots.push_back(slot);
}

void CSVFileWriter::setDelimeter(const string delimeter) {
    this->delimeter = delimeter;
}

void CSVFileWriter::writeToFile(boost::filesystem::path filePath) {
    namespace fs = boost::filesystem;
    io::stream_buffer<io::file_sink> outbuf(filePath.string());
    std::ostream out(&outbuf);
    int i = 0, j =0 ;
    for (i= 0; i< slots.size(); i++) {
        for (j = 0; j < slots[i].size()-1; j++) {
            out<<setprecision(10) <<slots[i][j]<<delimeter;
        }
        out<<setprecision(10) <<slots[i][j]<<endl;
    }
    out.flush();
    outbuf.close();
}