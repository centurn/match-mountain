#pragma once

#include "mam_log.h"
#include <exception>

// Say 'goodbye' and die!
#define PANIC(...){\
    log_e(__VA_ARGS__);\
    std::abort();\
}
