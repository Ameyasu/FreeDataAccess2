#include "DataItem.h"
#include "ValueToString.h"
#include "StringToValue.h"
#include <memory>
#include <format>

DataItem::DataItem(const char* str, DataFormat format)
	: m_type(DataType::String)
	, m_alignment(std::alignment_of_v<char>)
	, m_num()
	, m_format(format)
	, m_memory()
{
	size_t num = 0;
	for (; str[num] != '\0'; ++num);
	++num;
	m_memory.arrayPtr = new char[num];
	memcpy(m_memory.arrayPtr, str, std::alignment_of_v<char> * num);
	m_num = num;
}

DataItem::DataItem(FormatX, const char* fmt, size_t len)
	: m_type()
	, m_alignment()
	, m_num()
	, m_format()
	, m_memory()
{
	size_t c = 0;
	bool intOrUint = false;
	if (strncmp(fmt, "int", 3) == 0)
	{
		c = 3;
		m_type = DataType::Integer;
		intOrUint = true;
	}
	else if (strncmp(fmt, "uint", 4) == 0)
	{
		c = 4;
		m_type = DataType::UInteger;
		intOrUint = true;
	}
	if (intOrUint)
	{
		if (strncmp(fmt + c, "8/", 2) == 0)
		{
			m_alignment = 1;
			c += 2;
		}
		else
		{
			if (strncmp(fmt + c, "16/", 3) == 0)
				m_alignment = 2;
			else if (strncmp(fmt + c, "32/", 3) == 0)
				m_alignment = 4;
			else if (strncmp(fmt + c, "64/", 3) == 0)
				m_alignment = 8;
			else
				throw;
			c += 3;
		}
	}
	else if (strncmp(fmt, "float/", 6) == 0)
	{
		c = 6;
		m_type = DataType::Decimal;
		m_alignment = std::alignment_of_v<float>;
	}
	else if (strncmp(fmt, "double/", 7) == 0)
	{
		c = 7;
		m_type = DataType::Decimal;
		m_alignment = std::alignment_of_v<double>;
	}
	else if (strncmp(fmt, "bool/", 5) == 0)
	{
		c = 5;
		m_type = DataType::Boolean;
		m_alignment = std::alignment_of_v<bool>;
	}
	else if (strncmp(fmt, "char/", 5) == 0)
	{
		c = 5;
		m_type = DataType::Character;
		m_alignment = std::alignment_of_v<char>;
	}
	else if (strncmp(fmt, "str/", 4) == 0)
	{
		c = 4;
		m_type = DataType::String;
		m_alignment = std::alignment_of_v<char>;
		size_t strLen = len - c;
		m_num = strLen + 1;
		char* value = new char[m_num];
		m_memory.arrayPtr = value;
		memcpy(value, fmt + c, strLen);
		value[strLen] = '\0';
		return;
	}
	else
	{
		throw;
	}
	{
		size_t d = c;
		while (true)
		{
			if (d >= len) throw;
			if (fmt[d] == '/') break;
			++d;
		}
		m_num = std::stoull(std::string(fmt + c, d - c));
		c = d + 1;
	}

	if (strncmp(fmt + c, "def/", 4) == 0)
		m_format = DataFormat::Default;
	else if (strncmp(fmt + c, "bin/", 4) == 0)
		m_format = DataFormat::Binary;
	else if (strncmp(fmt + c, "dec/", 4) == 0)
		m_format = DataFormat::Decimal;
	else if (strncmp(fmt + c, "hex/", 4) == 0)
		m_format = DataFormat::Hexadecimal;
	else
		throw;
	c += 4;

	StringToValueFn fnStringToValue = GetStringToValueFn(m_type, m_alignment, m_format);

	if (m_num == 0)
	{
		// 値
		m_memory.value8 = fnStringToValue(std::string(fmt + c, len - c));
	}
	else
	{
		// 配列
		switch (m_alignment)
		{
		case 1:
		{
			if (m_type == DataType::Character && m_format == DataFormat::Default)
			{
				// char型の配列の場合 ',' が区切り文字と区別が付くように処理をする
				char* value = new char[m_num];
				m_memory.arrayPtr = value;
				for (size_t i = 0; i < m_num; ++i)
				{
					if (c >= len) throw;
					value[i] = fmt[c];
					c += 2;
				}
			}
			else
			{
				uint8_t* value = new uint8_t[m_num];
				m_memory.arrayPtr = value;
				for (size_t i = 0; i < m_num - 1; ++i)
				{
					size_t d = c;
					while (true)
					{
						if (d >= len - 1) throw;
						if (fmt[d] == ',') break;
						++d;
					}
					value[i] = static_cast<uint8_t>(fnStringToValue(std::string(fmt + c, d - c)));
					c = d + 1;
				}
				value[m_num - 1] = static_cast<uint8_t>(fnStringToValue(std::string(fmt + c, len - c)));
			}
		}
		break;
		case 2:
		{
			uint16_t* value = new uint16_t[m_num];
			m_memory.arrayPtr = value;
			for (size_t i = 0; i < m_num - 1; ++i)
			{
				size_t d = c;
				while (true)
				{
					if (d >= len) throw;
					if (fmt[d] == ',') break;
					++d;
				}
				value[i] = static_cast<uint16_t>(fnStringToValue(std::string(fmt + c, d - c)));
				c = d + 1;
			}
			value[m_num - 1] = static_cast<uint16_t>(fnStringToValue(std::string(fmt + c, len - c)));
		}
		break;
		case 4:
		{
			uint32_t* value = new uint32_t[m_num];
			m_memory.arrayPtr = value;
			for (size_t i = 0; i < m_num - 1; ++i)
			{
				size_t d = c;
				while (true)
				{
					if (d >= len) throw;
					if (fmt[d] == ',') break;
					++d;
				}
				value[i] = static_cast<uint32_t>(fnStringToValue(std::string(fmt + c, d - c)));
				c = d + 1;
			}
			value[m_num - 1] = static_cast<uint32_t>(fnStringToValue(std::string(fmt + c, len - c)));
		}
		break;
		case 8:
		{
			uint64_t* value = new uint64_t[m_num];
			m_memory.arrayPtr = value;
			for (size_t i = 0; i < m_num - 1; ++i)
			{
				size_t d = c;
				while (true)
				{
					if (d >= len) throw;
					if (fmt[d] == ',') break;
					++d;
				}
				value[i] = fnStringToValue(std::string(fmt + c, d - c));
				c = d + 1;
			}
			value[m_num - 1] = fnStringToValue(std::string(fmt + c, len - c));
		}
		break;
		default:
			throw;
		}
	}
}

std::string DataItem::operator()() const
{
	std::string s;
	switch (m_type)
	{
	case DataType::Integer: s = std::format("int{}/", m_alignment * 8); break;
	case DataType::UInteger: s = std::format("uint{}/", m_alignment * 8); break;
	case DataType::Decimal: s = m_alignment == 4 ? "float/" : "double/"; break;
	case DataType::Boolean: s = "bool/"; break;
	case DataType::Character: s = "char/"; break;
	case DataType::String: return std::format("str/{}", static_cast<const char*>(m_memory.arrayPtr));
	default: throw;
	}
	s.append(std::format("{}/", m_num));
	switch (m_format)
	{
	case DataFormat::Default: s.append("def/"); break;
	case DataFormat::Binary: s.append("bin/"); break;
	case DataFormat::Decimal: s.append("dec/"); break;
	case DataFormat::Hexadecimal: s.append("hex/"); break;
	}
	DataType type = (m_format == DataFormat::Default) ? m_type : DataType::UInteger;
	ValueToStringFn fnValueToString = GetValueToStringFn(type, m_alignment, m_format);

	size_t num = m_num;
	if (num == 0)
		return s.append(fnValueToString(&m_memory, 0));

	for (size_t i = 0; i < num - 1; ++i)
		s.append(fnValueToString(m_memory.arrayPtr, i) + ',');
	s.append(fnValueToString(m_memory.arrayPtr, num - 1));

	return s;
}

DataItem::DataItem(DataItem&& rhs) noexcept
	: m_type(rhs.m_type)
	, m_alignment(rhs.m_alignment)
	, m_num(rhs.m_num)
	, m_format(rhs.m_format)
	, m_memory(rhs.m_memory)
{
	rhs.m_num = 0;
	rhs.m_memory.arrayPtr = nullptr;
}

DataItem& DataItem::operator=(DataItem&& rhs) noexcept
{
	if (m_num > 0)
		delete[] m_memory.arrayPtr;

	m_type = rhs.m_type;
	m_alignment = rhs.m_alignment;
	m_num = rhs.m_num;
	m_format = rhs.m_format;
	m_memory = rhs.m_memory;
	rhs.m_num = 0;
	rhs.m_memory.arrayPtr = nullptr;
	return *this;
}

DataItem::~DataItem()
{
	if (m_num > 0)
		delete[] m_memory.arrayPtr;
}

DataItem::DataItem(DataType type, size_t alignment, DataFormat format, const void* value)
	: m_type(type)
	, m_alignment(alignment)
	, m_num(0)
	, m_format(format)
	, m_memory()
{
	switch (alignment)
	{
	case 1: m_memory.value1 = *reinterpret_cast<const uint8_t*>(value); break;
	case 2: m_memory.value2 = *reinterpret_cast<const uint16_t*>(value); break;
	case 4: m_memory.value4 = *reinterpret_cast<const uint32_t*>(value); break;
	case 8: m_memory.value8 = *reinterpret_cast<const uint64_t*>(value); break;
	default: throw;
	}
}

DataItem::DataItem(DataType type, size_t alignment, size_t num, DataFormat format, const void* arrayPtr)
	: m_type(type)
	, m_alignment(alignment)
	, m_num(num)
	, m_format(format)
	, m_memory()
{
	// サイズ 0 の配列は禁止
	if (alignment == 0 || num == 0)
		throw;

	// size_t の最大値を超えるバイト数のメモリを確保する配列は禁止
	if (num > UINT64_MAX / alignment)
		throw;

	// ヌル終端文字列 なら String 扱い
	if (type == DataType::Character && static_cast<const char*>(arrayPtr)[num - 1] == '\0')
		m_type = DataType::String;

	switch (alignment)
	{
	case 1: m_memory.arrayPtr = new uint8_t[num]; break;
	case 2: m_memory.arrayPtr = new uint16_t[num]; break;
	case 4: m_memory.arrayPtr = new uint32_t[num]; break;
	case 8: m_memory.arrayPtr = new uint64_t[num]; break;
	default: throw;
	}

	// ちゃんとサイズ管理しているので警告には消えてもらう
#pragma warning(push)
#pragma warning(disable:6386)
	memcpy(m_memory.arrayPtr, arrayPtr, alignment * num);
#pragma warning(pop)
}

DataItem::DataItem(ShallowX, DataType type, size_t alignment, size_t num, DataFormat format, void* arrayPtr)
	: m_type(type)
	, m_alignment(alignment)
	, m_num(num)
	, m_format(format)
	, m_memory()
{
	// サイズ 0 の配列は禁止
	if (alignment == 0 || num == 0)
		throw;

	// size_t の最大値を超えるバイト数のメモリを確保する配列は禁止
	if (num > UINT64_MAX / alignment)
		throw;

	// ヌル終端文字列 なら String 扱い
	if (type == DataType::Character && static_cast<const char*>(arrayPtr)[num - 1] == '\0')
		m_type = DataType::String;

	m_memory.arrayPtr = arrayPtr;
}
