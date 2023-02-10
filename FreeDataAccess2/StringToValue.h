#pragma once

#include "DataType.h"
#include "DataFormat.h"
#include <string>

using StringToValueFn = uint64_t(*)(const std::string& str);
StringToValueFn GetStringToValueFn(DataType type, size_t alignment, DataFormat format);
