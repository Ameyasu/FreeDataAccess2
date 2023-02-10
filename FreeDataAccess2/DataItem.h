// �v���~�e�B�u�^�𓮓I�ɕύX�\�Ȓl����������N���X
// �ݒ肳�ꂽ�^�ƈႤ�^�Œl���擾�E�ݒ肵�悤�Ƃ���Ɨ�O�𓊂���
// char�^�̔z�����������ꍇ
// �@'\0'�ŏI���@�@�@���@�N���X�����Ŕ��肵�āu�k���I�[������v��������
// �@'\0'�ŏI���Ȃ��@���@�N���X�����Ŕ��肵�āuchar�^�̔z��v��������
//

#pragma once

#include "DataType.h"
#include "DataFormat.h"
#include <string>

class DataItem
{
public:
	class DeepX {}; static constexpr DeepX Deep{};
	class ShallowX {}; static constexpr ShallowX Shallow{};
	class FormatX {}; static constexpr FormatX Format{};

public:
	// �l �� deep copy ���� DataItem �𐶐�
	template<typename T, typename = std::enable_if_t<!std::is_reference_v<T> && !std::is_pointer_v<T>>>
	DataItem(const T value, DataFormat format = DataFormat::Default) : DataItem(TypenameToDataType<T>(), std::alignment_of_v<T>, format, &value) {}

	// �z�� �� deep copy ���� DataItem �𐶐�
	template<typename T, typename = std::enable_if_t<std::is_pointer_v<T> && !std::is_pointer_v<std::remove_pointer_t<T>>>>
	DataItem(const T arrayPtr, size_t num, DataFormat format = DataFormat::Default) : DataItem(TypenameToDataType<T>(), std::alignment_of_v<std::remove_pointer_t<T>>, num, format, arrayPtr) {}

	// �z�� �� deep copy ���� DataItem �𐶐� (deep �ł��邱�Ƃ𖾎�)
	template<typename T, typename = std::enable_if_t<std::is_pointer_v<T> && !std::is_pointer_v<std::remove_pointer_t<T>> && !std::is_const_v<T>>>
	DataItem(DeepX, T arrayPtr, size_t num, DataFormat format = DataFormat::Default) : DataItem(TypenameToDataType<T>(), std::alignment_of_v<std::remove_pointer_t<T>>, num, format, arrayPtr) {}

	// �z�� �� shallow copy ���� DataItem �𐶐� (delete[] �ł��Ȃ����̂�n���Ȃ�)
	template<typename T, typename = std::enable_if_t<std::is_pointer_v<T> && !std::is_pointer_v<std::remove_pointer_t<T>> && !std::is_const_v<T>>>
	DataItem(ShallowX, T arrayPtr, size_t num, DataFormat format = DataFormat::Default) : DataItem(Shallow, TypenameToDataType<T>(), std::alignment_of_v<std::remove_pointer_t<T>>, num, format, arrayPtr) {}

	// �Œ蒷�z�� �� deep copy ���� DataItem �𐶐�
	template<typename T, std::size_t N, typename = std::enable_if_t<!std::is_reference_v<T> && !std::is_pointer_v<T>>>
	DataItem(const T(&arrayPtr)[N], DataFormat format = DataFormat::Default) : DataItem(TypenameToDataType<T>(), std::alignment_of_v<T>, N, format, arrayPtr) {}

	// �k���I�[������ �� deep copy ���� DataItem �𐶐�
	DataItem(const char* str, DataFormat format = DataFormat::Default);

	// DataItem �ŗL�̏����̕����񂩂� DataItem �𐶐�
	DataItem(FormatX, const char* fmt, size_t len);

	// DataItem �ŗL�̏����̕�����𐶐�
	std::string operator()() const;

	// �f�[�^�擾
	template<typename T, typename = std::enable_if_t<!std::is_reference_v<T> && !std::is_pointer_v<std::remove_pointer_t<T>>>>
	operator T()
	{
		if constexpr (std::is_same_v<std::remove_pointer_t<T>, char>)
		{
			if (m_type != DataType::Character && m_type != DataType::String)
				throw;
		}
		else
		{
			if (m_type != TypenameToDataType<T>())
				throw;
		}
		if (m_alignment != std::alignment_of_v<std::remove_pointer_t<T>>)
			throw;
		if constexpr (std::is_pointer_v<T>)
		{
			if (m_num > 0)
				// �z��
				return reinterpret_cast<T>(m_memory.arrayPtr);
		}
		else
		{
			if (m_num == 0)
				// �l
				return *reinterpret_cast<T*>(&m_memory);
		}
		throw;
	}

	template<typename T, typename = std::enable_if_t<!std::is_reference_v<T> && !std::is_pointer_v<T>>>
	void operator=(T value)
	{
		if (m_type != TypenameToDataType<T>() || m_alignment != std::alignment_of_v<T> || m_num > 0)
			throw;
		switch (m_alignment)
		{
		case 1: m_memory.value1 = *reinterpret_cast<uint8_t*>(&value); break;
		case 2: m_memory.value2 = *reinterpret_cast<uint16_t*>(&value); break;
		case 4: m_memory.value4 = *reinterpret_cast<uint32_t*>(&value); break;
		case 8: m_memory.value8 = *reinterpret_cast<uint64_t*>(&value); break;
		default: throw;
		}
	}

	DataType GetType() const { return m_type; }
	size_t GetAlignment() const { return m_alignment; }
	size_t GetNum() const { return m_num; }
	DataFormat GetFormat() const { return m_format; }
	void SetFormat(DataFormat format) { m_format = format; }

private:
	DataItem(const DataItem&) = delete;
	DataItem& operator=(const DataItem&) = delete;

public:
	DataItem(DataItem&& rhs) noexcept;
	DataItem& operator=(DataItem&& rhs) noexcept;
	~DataItem();

private:
	DataItem(DataType type, size_t alignment, DataFormat format, const void* value);
	DataItem(DataType type, size_t alignment, size_t num, DataFormat format, const void* arrayPtr);
	DataItem(ShallowX, DataType type, size_t alignment, size_t num, DataFormat format, void* arrayPtr);

private:
	DataType m_type;
	size_t m_alignment;
	size_t m_num;
	DataFormat m_format;
	union Memory
	{
		uint8_t value1;
		uint16_t value2;
		uint32_t value4;
		uint64_t value8;
		void* arrayPtr;
	} m_memory;
};

