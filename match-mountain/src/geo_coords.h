#pragma once
#include <cmath>
#include "glm/trigonometric.hpp"
#include <tuple>

namespace geo{

struct Coord{
    int deg(){
        return static_cast<int>(value);
    }
    int minute(){
        return static_cast<int>((value - deg())*60);
    }
    int sec(){
        return static_cast<int>(std::lround((value - deg() - minute()/60.0)*60*60));
    }
    int totalSec(){
        return minute() * 60 + sec();
    }
    double value;
    // Constructs from Degrees and seconds.
    // This is not through ctor since it may be confusing without the name
    static Coord fromDegSec(int deg, int sec){
        return Coord{deg + sec/(60.0*60.0)};
    }
};

struct Position{
    Coord lat;
    Coord lon;
};


// Approximate len (meters) of 1 longitudal degree on a given latitude
inline double lonSecApprox(Coord latitude){
    constexpr double arcsecond = 1.0 / 60 / 60;// One arcsecond as a single double
    return 111.3 * glm::cos(glm::radians(latitude.value)) * (arcsecond);
};
// Same for latitudal (this does not depend on location)
constexpr double latSecondApprox = 90.0;

}
