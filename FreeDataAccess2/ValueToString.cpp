#include "ValueToString.h"
#include <format>

template<typename T>
static std::string ValueToStringDefault(const void* p, size_t i) { return std::format("{}", static_cast<const T*>(p)[i]); }
template<typename T>
static std::string ValueToStringBinary(const void* p, size_t i) { return std::format("{:b}", static_cast<const T*>(p)[i]); }
template<typename T>
static std::string ValueToStringDecimal(const void* p, size_t i) { return std::format("{:d}", static_cast<const T*>(p)[i]); }
template<typename T>
static std::string ValueToStringHexadecimal(const void* p, size_t i) { return std::format("{:X}", static_cast<const T*>(p)[i]); }

ValueToStringFn GetValueToStringFn(DataType type, size_t alignment, DataFormat format)
{
	switch (type)
	{
	case DataType::Integer:
		switch (format)
		{
		case DataFormat::Default:
			switch (alignment)
			{
			case 1: return ValueToStringDefault<int8_t>;
			case 2: return ValueToStringDefault<int16_t>;
			case 4: return ValueToStringDefault<int32_t>;
			case 8: return ValueToStringDefault<int64_t>;
			}
			break;
		case DataFormat::Binary:
			switch (alignment)
			{
			case 1: return ValueToStringBinary<int8_t>;
			case 2: return ValueToStringBinary<int16_t>;
			case 4: return ValueToStringBinary<int32_t>;
			case 8: return ValueToStringBinary<int64_t>;
			}
			break;
		case DataFormat::Decimal:
			switch (alignment)
			{
			case 1: return ValueToStringDecimal<int8_t>;
			case 2: return ValueToStringDecimal<int16_t>;
			case 4: return ValueToStringDecimal<int32_t>;
			case 8: return ValueToStringDecimal<int64_t>;
			}
			break;
		case DataFormat::Hexadecimal:
			switch (alignment)
			{
			case 1: return ValueToStringHexadecimal<int8_t>;
			case 2: return ValueToStringHexadecimal<int16_t>;
			case 4: return ValueToStringHexadecimal<int32_t>;
			case 8: return ValueToStringHexadecimal<int64_t>;
			}
			break;
		default: throw;
		}
		break;
	case DataType::UInteger:
		switch (format)
		{
		case DataFormat::Default:
			switch (alignment)
			{
			case 1: return ValueToStringDefault<uint8_t>;
			case 2: return ValueToStringDefault<uint16_t>;
			case 4: return ValueToStringDefault<uint32_t>;
			case 8: return ValueToStringDefault<uint64_t>;
			}
			break;
		case DataFormat::Binary:
			switch (alignment)
			{
			case 1: return ValueToStringBinary<uint8_t>;
			case 2: return ValueToStringBinary<uint16_t>;
			case 4: return ValueToStringBinary<uint32_t>;
			case 8: return ValueToStringBinary<uint64_t>;
			}
			break;
		case DataFormat::Decimal:
			switch (alignment)
			{
			case 1: return ValueToStringDecimal<uint8_t>;
			case 2: return ValueToStringDecimal<uint16_t>;
			case 4: return ValueToStringDecimal<uint32_t>;
			case 8: return ValueToStringDecimal<uint64_t>;
			}
			break;
		case DataFormat::Hexadecimal:
			switch (alignment)
			{
			case 1: return ValueToStringHexadecimal<uint8_t>;
			case 2: return ValueToStringHexadecimal<uint16_t>;
			case 4: return ValueToStringHexadecimal<uint32_t>;
			case 8: return ValueToStringHexadecimal<uint64_t>;
			}
			break;
		default: throw;
		}
		break;
	case DataType::Decimal:
		switch (alignment)
		{
		case 4: return ValueToStringDefault<float>;
		case 8: return ValueToStringDefault<double>;
		}
		break;
	case DataType::Boolean:
		if (alignment == 1) return ValueToStringDefault<bool>;
		break;
	case DataType::Character:
		if (alignment == 1) return ValueToStringDefault<char>;
		break;
	}
	throw;
}

