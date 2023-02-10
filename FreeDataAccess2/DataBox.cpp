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

	// 全角か半角かを見分けながら指定した半角文字のインデックスを検索する
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
	// インデントをカウントする
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

	// DataItemタグ追加ループ
	while (i < size && fmt[i] == '@')
	{
		// 区切り文字のインデックスを取得
		size_t j = foundSBC(i + 1, '/');

		// 改行インデックスを取得
		size_t k = foundSBC(j + 1, '\n');

		// 今の状況
		// i            j              k
		// @DataItemName/DataItemFormat

		// AddItem(DataItemName, DataItemFormat, len)
		AddItem(format.substr(i + 1, j - i - 1).c_str(), DataItem::Format, fmt + j + 1, k - j - 1);

		// 次の行
		i = k + 1;

		// インデントによって処理を変える
		size_t c = countIndent(i);
		i += c;

		// 増えたら例外
		if (c > indent) throw;

		// 減ったら終了
		if (c < indent)
		{
			cursor = i;
			indent = c;
			return;
		}

		// 同じだったらもう一周
	}

	// DataBoxタグ追加ループ
	while (i < size && fmt[i] == '$')
	{
		// 改行インデックスを取得
		size_t j = foundSBC(i + 1, '\n');

		// 今の状況
		// i           j
		// $DataBoxName

		// AddBox(DataBoxName)
		DataBox& box = AddBox(format.substr(i + 1, j - i - 1).c_str());

		// 次の行
		i = j + 1;

		// インデントによって処理を変える
		size_t c = countIndent(i);
		i += c;

		// 増えたら再帰
		if (c > indent)
			box.InputFormat(format, i, c);

		// 減ったら終了
		if (c < indent)
		{
			cursor = i;
			indent = c;
			return;
		}

		// 同じだったらもう一周
	}

	// ファイルの書式に間違いがあると例外
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
