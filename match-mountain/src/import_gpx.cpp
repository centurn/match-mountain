#include "import_gpx.h"
#include <sstream>

#include "pugixml.hpp"

namespace geo{

Track importGPX(const char* filename){
    Track result;
    pugi::xml_document doc;
    pugi::xml_parse_result parse_result = doc.load_file(filename);

    if(!parse_result){
        log_e("XML [%s] parsed with errors, attr value: [%s]\n", filename,  doc.child("node").attribute("attr").value());
        log_e("Error description: %s\n", parse_result.description());
        log_e("Error offset: %d\n", int(parse_result.offset) );
        throw asg::GenericException("XML parse error");
    }

    for(auto trk: doc.child("gpx").children("trk")){
        result.name = trk.child_value("name");

        std::istringstream istr;
        istr.imbue(std::locale("C"));// That's to be independent of user's locale

        for(auto trkseg: trk.children("trkseg")){
            result.data.push_back(Track::Segment{});
            auto& segment = result.data.back();
            for(auto trkpt: trkseg.children("trkpt")){
                segment.push_back(Position{});
                Position& point =segment.back();

                istr.str(trkpt.attribute("lat").value());
                istr.seekg(0);
                istr >> point.lat.value;

                istr.str(trkpt.attribute("lon").value());
                istr.seekg(0);
                istr >> point.lon.value;

//                istr.str(trkpt.child_value("time"));
//                istr.seekg(0);
//                std::tm t = {};
//                // Format sample:         2016-08-25T15:35:02Z
//                istr >> std::get_time(&t, "%Y-%m-%dT%H:%M:%SZ");;
//                if(istr.fail())
//                    return false;
//                point.time = mktime(&t);
            }
            if(segment.size() < 2)
                result.data.pop_back();// Too small to be of any use
        }

    }
    return result;
}

}
