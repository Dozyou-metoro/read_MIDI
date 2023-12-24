#define _CRT_SECURE_NO_WARNINGS

#include<fstream>
#include<filesystem>
#include<vector>
#include<cstring>
#include<string>
#include<cassert>

#include"ERROR_PRINT.h"

class MIDI_track
{
public:
	MIDI_track(size_t size, uint8_t* track_data);
	~MIDI_track();

	typedef struct {
		size_t time;
		size_t data_size;
		uint8_t* event_data;
	}track_data;

	MIDI_track::track_data get_track_data(uint16_t data_no);
private:
	uint8_t* p_track_data = nullptr;//�g���b�N�f�[�^�̎n�[�|�C���^
	size_t data_size = 0;

	uint16_t status_count = 1;
	size_t startcount_before = 0;

	uint8_t return_time(uint8_t* data, size_t* time_num);


};



_inline MIDI_track::MIDI_track(size_t size, uint8_t* data)
{
	assert(data);//nullptr�`�F�b�N
	p_track_data = data;
	data_size = size;
}

_inline MIDI_track::~MIDI_track()
{

}

_inline MIDI_track::track_data MIDI_track::get_track_data(uint16_t data_no) {
	size_t count = 0;
	size_t return_data_num = 0;



	MIDI_track::track_data return_buf = {};

	if (data_no >= status_count - 1) {
		count = startcount_before;
		status_count--;
	}
	else {
		status_count = 0;
		return_data_num = 0;
	}

	do {
		if (count >= data_size) {
			return_buf.data_size = 0;
			return return_buf;
		}

		startcount_before = count;
		count += return_time(&p_track_data[count], &return_buf.time);//���Ԃ�ǂݍ���
		return_data_num = count;
		status_count++;

		switch (p_track_data[count] & 0xf0) {
		case 0x80://�m�[�g�I�t 8n kk vv �`�����l���ԍ�n�̃m�[�g�ԍ�kk�̉����~�߂�B
			count += 3;
			return_buf.data_size = 3;

			break;

		case 0x90://�m�[�g�I�� 9n kk vv �`�����l��n�Ńm�[�g�ԍ�kk�̉����x���V�e�Bvv�Ŗ炷�B
			count += 3;
			return_buf.data_size = 3;
			break;

		case 0xA0://�|���t�H�j�b�N�L�[�v���b�V���[ An kk vv �`�����l��n�Ŕ������̃m�[�g�ԍ�kk�̉��ɑ΂��A�x���V�e�Bvv�� �v���b�V���[����^����B
			count += 3;
			return_buf.data_size = 3;
			break;

		case 0xB0://�R���g���[���`�F���W Bn cc vv �`�����l��n�ŁA�R���g���[���i���o�[cc�ɁA�lvv�𑗂�B
			count += 3;
			return_buf.data_size = 3;
			break;

		case 0xC0://�v���O�����`�F���W Cn pp  �`�����l��n�ŁA�v���O����(���F)��pp�ɕύX����B
			count += 2;
			return_buf.data_size = 2;
			break;

		case 0xD0://�`�����l���v���b�V���[ Dn vv �`�����l��n�ɑ΂��A�v���b�V���[���vv�𑗐M����B
			count += 2;
			return_buf.data_size = 2;
			break;

		case 0xE0://�s�b�`�x���h En mm ll �`�����l��n�ɑ΂��A�s�b�`�x���h�lllmm�𑗐M����B���g���G���f�B�A���Ȃ̂Œ��ӁB
			count += 3;
			return_buf.data_size = 3;
			break;

		case 0xF0://����Ȏw��

			switch (p_track_data[count]) {
			case 0xF0://F0�Ŏn�܂�sysex�C�x���g
				return_buf.data_size = (p_track_data[count + 1] + 2);
				count += (p_track_data[count + 1] + 2);

				break;

			case 0xF7://F7�Ŏn�܂�sysex�C�x���g
				return_buf.data_size = (p_track_data[count + 1] + 2);
				count += (p_track_data[count + 1] + 2);

				break;

			case 0xFF://���^�C�x���g
				return_buf.data_size = (p_track_data[count + 2] + 3);
				count += (p_track_data[count + 2] + 3);

				break;

			default:
				ERROR_PRINT("�t�@�C�����j�����Ă��܂��B", 0);
				break;
			}
			break;

		default:
			printf("\n\n%#x\n", p_track_data[count]);
			uint8_t a = p_track_data[count];
			ERROR_PRINT("�t�@�C�����j�����Ă��܂��B", 0);
			break;
		}

	} while (data_no >= status_count);

	return_buf.event_data = &p_track_data[return_data_num];
	return return_buf;

}

_inline uint8_t MIDI_track::return_time(uint8_t* data, size_t* time_num) {
	uint8_t timedata_size = 0;
	uint8_t data_count = 0;

	*time_num = 0;


	do {
		timedata_size++;
		*time_num = *time_num << 7;
		*time_num += (data[timedata_size - 1] & 0x7f);//���1�r�b�g�͉ϒ��̊Ǘ��g�p���邽�ߏ��O
	} while ((data[timedata_size - 1] & 0x80) != 0);//���1�r�b�g��1�Ȃ�p��

	return timedata_size;

}