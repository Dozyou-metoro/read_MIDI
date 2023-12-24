#include<fstream>
#include<filesystem>
#include<vector>
#include<cstring>
#include<string>
#include<cassert>

#include"MIDI_track.hpp"

class MIDI_read
{
public:
	MIDI_read(std::string path);
	~MIDI_read();

	typedef struct {//ヘッダフォーマット
		uint16_t MIDI_format;//MIDIのフォーマット
		uint16_t MIDI_track;//トラック数
		uint16_t resolution;//分解能(4分音符1個当たり時間単位？
	}MIDI_heard;

	MIDI_heard* heard_data;//ヘッダを入れる
	std::vector<MIDI_track*> track_data;//トラックデータを入れる

private:

	std::vector<uint8_t> file_data;//データを置く配列

	template<typename TYPE> TYPE endian_change(TYPE* data);
	template<typename TYPE> void endian_change_overwrite(TYPE* data);


};


_inline MIDI_read::MIDI_read(std::string path)
{

	std::uintmax_t file_size = std::filesystem::file_size(path);//ファイルサイズを読みとり
	file_data.resize(file_size);

	size_t file_place = 0;//ファイルの場所を記録

	{
		std::ifstream ifs(path, std::ios::binary);
		if (!ifs) {
			ERROR_PRINT("ファイルを開けませんでした。", -1)
		}

		ifs.read(reinterpret_cast<char*>(&file_data[0]), file_size);//ファイルを読み込み

	}



	if (memcmp(&file_data[file_place], "MThd", 4)) {//ヘッダの"MThd"があるか確認
		ERROR_PRINT("ファイルのフォーマットが違います", 0)
	}

	file_place += 4;//ヘッダの"MYhd"(4byte)

	heard_data = (MIDI_read::MIDI_heard*)&file_data[file_place + 4];//データの先頭ポインタを渡す

	for (int i = 0; i < MIDI_read::endian_change((uint32_t*)&file_data[file_place]) / sizeof(uint16_t); i++) {
		MIDI_read::endian_change_overwrite((uint16_t*)&file_data[file_place + 4 + (i * sizeof(uint16_t))]);//エンディアンを変換
	}

	file_place += (4 + MIDI_read::endian_change((uint32_t*)&file_data[4]));//ヘッダの領域分進める

	if ((heard_data->MIDI_format == 0) && (heard_data->MIDI_track == 1)) {//フォーマット0

	}
	else if ((heard_data->MIDI_format == 1) && (heard_data->MIDI_track != 0)) {//フォーマット1

	}
	else if (heard_data->MIDI_format == 2) {//フォーマット2(非対応)
		ERROR_PRINT("フォーマット2は非対応です。", 0)
	}
	else {//エラー
		ERROR_PRINT("ファイルが破損しています。", 0)
	}




	track_data.resize(heard_data->MIDI_track);//トラックデータを入れるメモリを確保

	for (int i = 0; i < heard_data->MIDI_track; i++) {
		if (memcmp(&file_data[file_place], "MTrk", 4)) {//ヘッダの"MThd"があるか確認
			ERROR_PRINT("ファイルのフォーマットが違います", 0)
		}

		file_place += 4;//トラックの"MYrk"(4byte)

		track_data[i] = new MIDI_track(MIDI_read::endian_change((uint32_t*)&file_data[file_place]), &file_data[file_place + 4]);//トラックデータをクラスに渡す

		file_place += (MIDI_read::endian_change((uint32_t*)&file_data[file_place]) + 4);//トラックデータのサイズぶん進める
	}


}

_inline MIDI_read::~MIDI_read()
{

}

/// <summary>
/// エンディアン変換
/// </summary>
/// <typeparam name="TYPE"></typeparam>
/// <param name="data">変換する変数のポインタ(変換先の型のポインタでキャストすること)</param>
/// <returns></returns>
template<typename TYPE> _inline TYPE MIDI_read::endian_change(TYPE* data) {
	TYPE num = 0;
	uint8_t* p_buf = (uint8_t*)data;
	for (int i = 0; i < sizeof(TYPE); i++) {
		num += p_buf[i] << (8 * (sizeof(TYPE) - 1 - i));
	}
	return num;
}

/// <summary>
/// エンディアン変換して上書き
/// </summary>
/// <typeparam name="TYPE"></typeparam>
/// <param name="data">変換する変数のポインタ(変換先の型のポインタでキャストすること)</param>
/// <returns></returns>
template<typename TYPE> _inline void MIDI_read::endian_change_overwrite(TYPE* data) {
	TYPE num = 0;
	uint8_t* p_buf = (uint8_t*)data;
	for (int i = 0; i < sizeof(TYPE); i++) {
		num += p_buf[i] << (8 * (sizeof(TYPE) - 1 - i));
	}

	*data = num;
}


