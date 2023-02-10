#pragma once

#include "DataType.h"
#include "DataFormat.h"
#include <string>

using ValueToStringFn = std::string(*)(const void*, size_t);
ValueToStringFn GetValueToStringFn(DataType type, size_t alignment, DataFormat format);
