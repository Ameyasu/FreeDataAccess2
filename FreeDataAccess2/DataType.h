#pragma once

#include <type_traits>

enum class DataType : uint8_t
{
	Integer,
	UInteger,
	Decimal,
	Boolean,
	Character,
	String
};

template<typename T, typename U = std::remove_const_t<std::remove_pointer_t<std::remove_reference_t<T>>>, typename = std::enable_if_t<!std::is_pointer_v<std::remove_pointer_t<T>>>>
constexpr DataType TypenameToDataType()
{
	if constexpr (std::is_same_v<U, float> || std::is_same_v<U, double>)
		return DataType::Decimal;
	else if (std::is_same_v<U, bool>)
		return DataType::Boolean;
	else if (std::is_same_v<U, char>)
		return DataType::Character;
	else if (std::is_unsigned_v<U>)
		return DataType::UInteger;
	else
		return DataType::Integer;
}
