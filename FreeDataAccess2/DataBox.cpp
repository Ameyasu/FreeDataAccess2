#include "DataBox.h"
#include <fstream>
#include <sstream>
#include <mbstring.h>

DataBox::DataBox()
{
}

DataBox::~DataBox()
{
}

bool DataBox::InputFile(const std::string& path)
{
	std::ifstream i(path, std::ios::in);
	if (!i)
		return false;

	Clear();

	std::string s((std::istreambuf_iterator<char>(i)), std::istreambuf_iterator<char>());
	size_t cursor = 0, indent = 0;
	InputFormat(s, cursor, indent);

	return true;
}

bool DataBox::OutputFile(const std::string& path) const
{
	std::ofstream o(path, std::ios::out);
	if (!o)
		return false;

	std::string s(OutputFormat(""));
	o.write(s.c_str(), s.size());

	return true;
}

void DataBox::InputFormat(const std::string& format, size_t& cursor, size_t& indent)
{
	const char* fmt = format.c_str();
	size_t size = format.size();

	// �S�p�����p�����������Ȃ���w�肵�����p�����̃C���f�b�N�X����������
	auto foundSBC = [fmt, size](size_t start, char sbc)
	{
		while (true)
		{
			if (start >= size)
				throw;
			if (_mbclen(reinterpret_cast<const unsigned char*>(fmt) + start) == 2)
			{
				start += 2;
				continue;
			}
			if (fmt[start] == sbc)
				break;
			++start;
		}
		return start;
	};
	// �C���f���g���J�E���g����
	auto countIndent = [fmt, size](size_t start)
	{
		size_t c = 0;
		while (true)
		{
			if (start >= size)
				return c;
			if (_mbclen(reinterpret_cast<const unsigned char*>(fmt) + start) == 2)
				return c;
			if (fmt[start] != '\t')
				return c;
			++c;
			++start;
		}
	};

	size_t i = cursor;

	// DataItem�^�O�ǉ����[�v
	while (i < size && fmt[i] == '@')
	{
		// ��؂蕶���̃C���f�b�N�X���擾
		size_t j = foundSBC(i + 1, '/');

		// ���s�C���f�b�N�X���擾
		size_t k = foundSBC(j + 1, '\n');

		// ���̏�
		// i            j              k
		// @DataItemName/DataItemFormat

		// AddItem(DataItemName, DataItemFormat, len)
		AddItem(format.substr(i + 1, j - i - 1).c_str(), DataItem::Format, fmt + j + 1, k - j - 1);

		// ���̍s
		i = k + 1;

		// �C���f���g�ɂ���ď�����ς���
		size_t c = countIndent(i);
		i += c;

		// ���������O
		if (c > indent) throw;

		// ��������I��
		if (c < indent)
		{
			cursor = i;
			indent = c;
			return;
		}

		// ������������������
	}

	// DataBox�^�O�ǉ����[�v
	while (i < size && fmt[i] == '$')
	{
		// ���s�C���f�b�N�X���擾
		size_t j = foundSBC(i + 1, '\n');

		// ���̏�
		// i           j
		// $DataBoxName

		// AddBox(DataBoxName)
		DataBox& box = AddBox(format.substr(i + 1, j - i - 1).c_str());

		// ���̍s
		i = j + 1;

		// �C���f���g�ɂ���ď�����ς���
		size_t c = countIndent(i);
		i += c;

		// ��������ċA
		if (c > indent)
			box.InputFormat(format, i, c);

		// ��������I��
		if (c < indent)
		{
			cursor = i;
			indent = c;
			return;
		}

		// ������������������
	}

	// �t�@�C���̏����ɊԈႢ������Ɨ�O
	if (i < size) throw;
}

std::string DataBox::OutputFormat(const std::string& indent) const
{
	std::stringstream s;

	for (auto& i : m_item)
		s << indent << '@' << i.first << '/' << i.second() << '\n';

	std::string n(indent + '\t');

	for (auto& i : m_box)
	{
		s << indent << '$' << i.first << "\n";
		s << i.second.OutputFormat(n);
	}

	return s.str();
}
