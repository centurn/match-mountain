#pragma once

#include "asg_base.h"
#include <vector>

namespace asg {

std::vector<uint8_t> readWholeFile(const char* filename);

// This supposed to be platform-independent modal file selection dialog
std::string fileOpenDialog(const char* default_fn, span<const char*> filters, const char* description);

}
