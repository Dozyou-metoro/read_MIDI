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

	typedef struct {//�w�b�_�t�H�[�}�b�g
		uint16_t MIDI_format;//MIDI�̃t�H�[�}�b�g
		uint16_t MIDI_track;//�g���b�N��
		uint16_t resolution;//����\(4������1�����莞�ԒP�ʁH
	}MIDI_heard;

	MIDI_heard* heard_data;//�w�b�_������
	std::vector<MIDI_track*> track_data;//�g���b�N�f�[�^������

private:

	std::vector<uint8_t> file_data;//�f�[�^��u���z��

	template<typename TYPE> TYPE endian_change(TYPE* data);
	template<typename TYPE> void endian_change_overwrite(TYPE* data);


};


_inline MIDI_read::MIDI_read(std::string path)
{

	std::uintmax_t file_size = std::filesystem::file_size(path);//�t�@�C���T�C�Y��ǂ݂Ƃ�
	file_data.resize(file_size);

	size_t file_place = 0;//�t�@�C���̏ꏊ���L�^

	{
		std::ifstream ifs(path, std::ios::binary);
		if (!ifs) {
			ERROR_PRINT("�t�@�C�����J���܂���ł����B", -1)
		}

		ifs.read(reinterpret_cast<char*>(&file_data[0]), file_size);//�t�@�C����ǂݍ���

	}



	if (memcmp(&file_data[file_place], "MThd", 4)) {//�w�b�_��"MThd"�����邩�m�F
		ERROR_PRINT("�t�@�C���̃t�H�[�}�b�g���Ⴂ�܂�", 0)
	}

	file_place += 4;//�w�b�_��"MYhd"(4byte)

	heard_data = (MIDI_read::MIDI_heard*)&file_data[file_place + 4];//�f�[�^�̐擪�|�C���^��n��

	for (int i = 0; i < MIDI_read::endian_change((uint32_t*)&file_data[file_place]) / sizeof(uint16_t); i++) {
		MIDI_read::endian_change_overwrite((uint16_t*)&file_data[file_place + 4 + (i * sizeof(uint16_t))]);//�G���f�B�A����ϊ�
	}

	file_place += (4 + MIDI_read::endian_change((uint32_t*)&file_data[4]));//�w�b�_�̗̈敪�i�߂�

	if ((heard_data->MIDI_format == 0) && (heard_data->MIDI_track == 1)) {//�t�H�[�}�b�g0

	}
	else if ((heard_data->MIDI_format == 1) && (heard_data->MIDI_track != 0)) {//�t�H�[�}�b�g1

	}
	else if (heard_data->MIDI_format == 2) {//�t�H�[�}�b�g2(��Ή�)
		ERROR_PRINT("�t�H�[�}�b�g2�͔�Ή��ł��B", 0)
	}
	else {//�G���[
		ERROR_PRINT("�t�@�C�����j�����Ă��܂��B", 0)
	}




	track_data.resize(heard_data->MIDI_track);//�g���b�N�f�[�^�����郁�������m��

	for (int i = 0; i < heard_data->MIDI_track; i++) {
		if (memcmp(&file_data[file_place], "MTrk", 4)) {//�w�b�_��"MThd"�����邩�m�F
			ERROR_PRINT("�t�@�C���̃t�H�[�}�b�g���Ⴂ�܂�", 0)
		}

		file_place += 4;//�g���b�N��"MYrk"(4byte)

		track_data[i] = new MIDI_track(MIDI_read::endian_change((uint32_t*)&file_data[file_place]), &file_data[file_place + 4]);//�g���b�N�f�[�^���N���X�ɓn��

		file_place += (MIDI_read::endian_change((uint32_t*)&file_data[file_place]) + 4);//�g���b�N�f�[�^�̃T�C�Y�Ԃ�i�߂�
	}


}

_inline MIDI_read::~MIDI_read()
{

}

/// <summary>
/// �G���f�B�A���ϊ�
/// </summary>
/// <typeparam name="TYPE"></typeparam>
/// <param name="data">�ϊ�����ϐ��̃|�C���^(�ϊ���̌^�̃|�C���^�ŃL���X�g���邱��)</param>
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
/// �G���f�B�A���ϊ����ď㏑��
/// </summary>
/// <typeparam name="TYPE"></typeparam>
/// <param name="data">�ϊ�����ϐ��̃|�C���^(�ϊ���̌^�̃|�C���^�ŃL���X�g���邱��)</param>
/// <returns></returns>
template<typename TYPE> _inline void MIDI_read::endian_change_overwrite(TYPE* data) {
	TYPE num = 0;
	uint8_t* p_buf = (uint8_t*)data;
	for (int i = 0; i < sizeof(TYPE); i++) {
		num += p_buf[i] << (8 * (sizeof(TYPE) - 1 - i));
	}

	*data = num;
}


