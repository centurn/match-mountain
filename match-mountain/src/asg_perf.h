#pragma once
// Helpers for performance measure

#include "asg_base.h"
#include <chrono>

#ifndef ASG_PROFILE

#define ASG_STOPWATCH(NAME)

#else
namespace asg{
// Measures performance of block of code and outputs the result
// Should be put in limited-visibility block as it measures in ctor/dtor
struct Stopwatch{
    explicit Stopwatch(const char* name)
        : name(name)
    {
        start = std::chrono::high_resolution_clock::now();
    }
    ~Stopwatch(){
        auto diff = std::chrono::high_resolution_clock::now() - start;
        long ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
        log_i("Perf: %s took %ld ms\n", name, ms);
    }
    std::chrono::high_resolution_clock::time_point start;
    const char* name;
};

#define ASG_STOPWATCH(NAME) Stopwatch stopwatch##__LINE__{(NAME)};

}
#endif
