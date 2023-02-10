// DataItem クラスを階層構造で所持するクラス
// DataBox は DataBox と DataItem を所持する
// ファイル入出力を使用する場合の禁則文字
// ・DataItem の name の「/」
// ・DataBox の name, DataItem の name, DataItem の値の char (単体,配列,文字列のすべて) のすべての「\n」
//

#pragma once

#include "DataItem.h"
#include <string>
#include <map>

class DataBox
{
public:
	DataBox();
	~DataBox();

private:
	DataBox(const DataBox&) = delete;
	DataBox& operator=(const DataBox&) = delete;
	DataBox(DataBox&&) = delete;
	DataBox& operator=(DataBox&&) = delete;

public:
	DataBox& AddBox(const char* name) { return m_box[name]; }

	template <typename... Args>
	DataItem& AddItem(const char* name, Args&&... args) { return m_item.try_emplace(name, DataItem(std::forward<Args>(args)...)).first->second; }

	void RemoveBox(const char* name) { m_box.erase(name); }
	void RemoveItem(const char* name) { m_item.erase(name); }

	bool HasBox(const char* name) const { return m_box.count(name) == 1; }
	bool HasItem(const char* name) const { return m_item.count(name) == 1; }

	const DataBox& operator[](const char* name) const { return m_box.at(name); }
	DataBox& operator[](const char* name) { return m_box.at(name); }

	const DataItem& operator()(const char* name) const { return m_item.at(name); }
	DataItem& operator()(const char* name) { return m_item.at(name); }

	void Clear() { m_box.clear(); m_item.clear(); }

	bool InputFile(const std::string& path);
	bool OutputFile(const std::string& path) const;

private:
	void InputFormat(const std::string& format, size_t& cursor, size_t& indent);
	std::string OutputFormat(const std::string& indent) const;

private:
	std::map<std::string, DataBox, std::less<>> m_box;
	std::map<std::string, DataItem, std::less<>> m_item;
};
