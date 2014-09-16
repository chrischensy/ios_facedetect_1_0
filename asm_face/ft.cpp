//
//  ft_io.cpp
//  JZP_EYE_TRACKING
//
//  Created by Zhiping Jiang on 14-8-31.
//
//
#include "ft.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

bool is_number_asm(const std::string& s)
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

ft_data load_ft_jzp(string fname) {
    fs::path filepath(fname);
    if (fs::exists(filepath)) {
        if (fs::is_directory(filepath)) {
            fs::path annotationfilePath(filepath / "annotations.yaml");
            if (fs::exists(annotationfilePath)) {
                ft_data data = load_ft<ft_data>(annotationfilePath.string().c_str());
                data.baseDir = filepath.string()+fs::path("/").make_preferred().native();
                return data;
            } else {
                cout<<annotationfilePath<<" does NOT exists. Return default one."<<endl;
                return ft_data();
            }
        }
        if (fs::is_regular_file(filepath) && boost::iequals(filepath.filename().string(), "annotations.yaml")) {
            ft_data data = load_ft<ft_data>(filepath.string().c_str());
            data.baseDir = filepath.parent_path().string()+fs::path("/").make_preferred().native();
            return data;
            
        } else {
            cout<<filepath<<" is not annotations.yaml. Return default one."<<endl;
            return ft_data();
        }
    }
    return ft_data();
    
}
