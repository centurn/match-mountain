#pragma once

#include <cstdio>

// TODO: Proper logging

// Error logging messages
#define log_e(...) std::fprintf(stderr, __VA_ARGS__)

// Normal info log - print always
#define log_i(...) std::printf(__VA_ARGS__)

#ifdef NDEBUG
#define log_d(...)
#else
// Debug log - print only in debug build
#define log_d(...) std::printf(__VA_ARGS__)
#endif
