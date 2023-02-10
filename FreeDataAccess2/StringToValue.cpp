#include "StringToValue.h"

static uint64_t StringToInteger(const std::string& str) { int64_t v = std::stoll(str); return *reinterpret_cast<uint64_t*>(&v); }
static uint64_t StringToUIntegerBinary(const std::string& str) { return std::stoull(str, nullptr, 2); }
static uint64_t StringToUIntegerDecimal(const std::string& str) { return std::stoull(str, nullptr, 10); }
static uint64_t StringToUIntegerHexadecimal(const std::string& str) { return std::stoull(str, nullptr, 16); }
static uint64_t StringToFloat(const std::string& str) { float v = std::stof(str); return *reinterpret_cast<uint64_t*>(&v); }
static uint64_t StringToDouble(const std::string& str) { double v = std::stod(str); return *reinterpret_cast<uint64_t*>(&v); }
static uint64_t StringToBoolean(const std::string& str) { return str[0] == 't' ? 1 : 0; }
static uint64_t StringToCharacter(const std::string& str) { return str[0]; }

StringToValueFn GetStringToValueFn(DataType type, size_t alignment, DataFormat format)
{
	switch (format)
	{
	case DataFormat::Default:
		switch (type)
		{
		case DataType::Integer: return StringToInteger;
		case DataType::UInteger: return StringToUIntegerDecimal;
		case DataType::Decimal:
			switch (alignment)
			{
			case 4: return StringToFloat;
			case 8: return StringToDouble;
			}
			break;
		case DataType::Boolean:
			if (alignment == 1) return StringToBoolean;
			break;
		case DataType::Character:
			if (alignment == 1) return StringToCharacter;
			break;
		}
		break;
	case DataFormat::Binary: return StringToUIntegerBinary;
	case DataFormat::Decimal: return StringToUIntegerDecimal;
	case DataFormat::Hexadecimal: return StringToUIntegerHexadecimal;
	}
	throw;
}
