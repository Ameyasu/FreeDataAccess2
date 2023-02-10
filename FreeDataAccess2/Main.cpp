#include "DataBox.h"

#include <iostream>
#include <format>

void usage()
{
	// ↓DataBox の使用方法

	{
		// 自機のステータス
		int meHP = 100;
		double meMP = 30.2;
		std::string meItem("回復薬x4, 解毒薬x1");
		bool abnormalCondition = false;

		// プログラム終了時に保存
		DataBox box;
		DataBox& meBox = box.AddBox("Me");
		meBox.AddItem("HP", meHP);
		meBox.AddItem("MP", meMP);
		meBox.AddItem("Item", meItem.c_str());
		meBox.AddItem("Abnormal Condition", abnormalCondition);

		// ファイル出力
		box.OutputFile("MeStatus.txt");
	}

	{
		// ファイル入力
		DataBox box;
		box.InputFile("MeStatus.txt");

		// 自機のステータスに反映
		DataBox& meBox = box["Me"];
		int meHP = meBox("HP");
		double meMP = meBox("MP");

		// 文字列を std::string で受け取るとき
		// 不安なときは明示キャスト
		std::string meItem(static_cast<char*>(meBox("Item")));

		// 文字列をそのまま char* で受け取るとき
		//DataItem& meDataItem = meBox("Item");
		//char* meItem = meDataItem;
		//size_t meItemLen = meDataItem.GetNum(); // '\0'含む

		bool abnormalCondition = meBox("Abnormal Condition");

		// こうもできるけど、何度も box["Me"] の処理が発生するのが無駄
		//int meHP = box["Me"]("HP");
		//int meMP = box["Me"]("MP");
		//char* meItem = box["Me"]("Item");
		//bool abnormalCondition = box["Me"]("Abnormal Condition");

		std::cout << "自機のステータス" << std::endl;
		std::cout << std::format("HP:{}, MP:{}, アイテム:{}, 状態異常:{}", meHP, meMP, meItem, abnormalCondition) << std::endl;
	}

	{
		DataItem item(10);
		std::cout << "item の中身を表示：" << item() << std::endl;

		item = 20;
		std::cout << "item の値を書き換えた：" << item() << std::endl;

		item = DataItem(DataItem::Shallow, new bool[]{true, false, false, true}, 4);
		std::cout << "item の中身を丸ごと入れ替えた：" << item() << std::endl;

		bool* pItem = item;
		pItem[2] = true;
		std::cout << "item の2番目の要素の値を書き換えた：" << item() << std::endl;

		/*
		* 出力
		* item の中身を表示：int32/0/def/10
		* item の値を書き換えた：int32/0/def/20
		* item の中身を丸ごと入れ替えた：bool/4/def/true,false,false,true
		* item の2番目の要素の値を書き換えた：bool/4/def/true,false,true,true
		*/
	}
}

int main(void)
{
	usage();

	// ↓DataItem の使用可能なパターンほぼ全部

	DataBox box;
	{
		DataBox& single = box.AddBox("single");
		{
			DataBox& integer = single.AddBox("integer");
			integer.AddItem("int8_t", int8_t(8));
			integer.AddItem("int16_t", int16_t(16));
			integer.AddItem("int32_t", int32_t(32));
			integer.AddItem("int64_t", int64_t(64));
		}
		{
			DataBox& uinteger = single.AddBox("uinteger");
			uinteger.AddItem("uint8_t", uint8_t(8));
			uinteger.AddItem("uint16_t", uint16_t(16));
			uinteger.AddItem("uint32_t", uint32_t(32));
			uinteger.AddItem("uint64_t", uint64_t(64));
		}
		{
			DataBox& deciaml = single.AddBox("decimal");
			deciaml.AddItem("float", float(1.41421356f));
			deciaml.AddItem("double", double(3.141592653));
		}
		{
			DataBox& boolean = single.AddBox("boolean");
			boolean.AddItem("bool_0", false);
			boolean.AddItem("bool_1", true);
		}
		{
			DataBox& character = single.AddBox("character");
			character.AddItem("char", 'a');
		}
	}
	{
		DataBox& multiple = box.AddBox("array");
		{
			DataBox& integer = multiple.AddBox("integer");
			int8_t a8[] = { 0, -1, 2, 3 };
			integer.AddItem("int8_t", a8);
			int16_t a16[] = { 2, 4, 6, 8, -10, 12 };
			integer.AddItem("int16_t", a16);
			int32_t a32[] = { 1, 3, 5, -7, 9, 11, 13, 15, 17, 19 };
			integer.AddItem("int32_t", a32);
			int64_t a64[] = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, -1, -2 };
			integer.AddItem("int64_t", a64);
		}
		{
			DataBox& uinteger = multiple.AddBox("uinteger");
			uint8_t a8[] = { 0, 1, 2, 3 };
			uinteger.AddItem("uint8_t", a8);
			uint16_t a16[] = { 2, 4, 6, 8, 10, 12 };
			uinteger.AddItem("uint16_t", a16);
			uint32_t a32[] = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19 };
			uinteger.AddItem("uint32_t", a32);
			uint64_t a64[] = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 10, 200 };
			uinteger.AddItem("uint64_t", a64);
		}
		{
			DataBox& deciaml = multiple.AddBox("decimal");
			float* pFloatArray = new float[3]{ 0.1f, 0.2f, 0.3f };
			deciaml.AddItem("float", DataItem(pFloatArray, 3));
			delete[] pFloatArray;

			double* pDoubleArray = new double[3]{ 0.01, 0.02, 0.03 };
			deciaml.AddItem("double_default_depp", pDoubleArray, 3);
			deciaml.AddItem("double_deep", DataItem::Deep, pDoubleArray, 3);
			deciaml.AddItem("double_shallow", DataItem::Shallow, pDoubleArray, 3);
			// shallow copy なので自分で delete[] しない
		}
		{
			DataBox& boolean = multiple.AddBox("boolean");
			bool a[] = { false, true, true, false };
			boolean.AddItem("bool", a);
		}
		{
			DataBox& character = multiple.AddBox("character");
			char a[] = { 'h', 'e', 'l', 'l', 'o' };
			character.AddItem("char", a);
		}
		{
			DataBox& str = multiple.AddBox("string");
			str.AddItem("string", "Hello World!");
		}
	}
	{
		box.AddBox("empty");
	}
	{
		DataBox& diff = box.AddBox("diff_format");
		{
			DataBox& integer = diff.AddBox("integer");
			integer.AddItem("int8_bin", int8_t(8), DataFormat::Binary);
			integer.AddItem("int8_dec", int8_t(8), DataFormat::Decimal);
			integer.AddItem("int8_hex", int8_t(8), DataFormat::Hexadecimal);
			integer.AddItem("int64_bin", int64_t(64), DataFormat::Binary);
			integer.AddItem("int64_dec", int64_t(64), DataFormat::Decimal);
			integer.AddItem("int64_hex", int64_t(64), DataFormat::Hexadecimal);
		}
		{
			DataBox& uinteger = diff.AddBox("uinteger");
			uinteger.AddItem("uint8_bin", uint8_t(8), DataFormat::Binary);
			uinteger.AddItem("uint8_dec", uint8_t(8), DataFormat::Decimal);
			uinteger.AddItem("uint8_hex", uint8_t(8), DataFormat::Hexadecimal);
			uinteger.AddItem("uint64_bin", uint64_t(64), DataFormat::Binary);
			uinteger.AddItem("uint64_dec", uint64_t(64), DataFormat::Decimal);
			uinteger.AddItem("uint64_hex", uint64_t(64), DataFormat::Hexadecimal);
		}
		{
			DataBox& deciaml = diff.AddBox("decimal");
			deciaml.AddItem("float_bin", float(1.41421356f), DataFormat::Binary);
			deciaml.AddItem("float_dec", float(1.41421356f), DataFormat::Decimal);
			deciaml.AddItem("float_hex", float(1.41421356f), DataFormat::Hexadecimal);
			deciaml.AddItem("double_bin", double(3.141592653), DataFormat::Binary);
			deciaml.AddItem("double_dec", double(3.141592653), DataFormat::Decimal);
			deciaml.AddItem("double_hex", double(3.141592653), DataFormat::Hexadecimal);
		}
		{
			DataBox& boolean = diff.AddBox("boolean");
			boolean.AddItem("bool_bin", true, DataFormat::Binary);
			boolean.AddItem("bool_dec", true, DataFormat::Decimal);
			boolean.AddItem("bool_hex", true, DataFormat::Hexadecimal);
		}
		{
			DataBox& character = diff.AddBox("character");
			character.AddItem("char_bin", 'a', DataFormat::Binary);
			character.AddItem("char_dec", 'a', DataFormat::Decimal);
			character.AddItem("char_hex", 'a', DataFormat::Hexadecimal);
		}
	}

	box.OutputFile("Data.txt");

	DataBox box2;
	box2.InputFile("Data.txt");
	box2.OutputFile("Data2.txt");

	return 0;
}
