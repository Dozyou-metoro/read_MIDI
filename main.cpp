#define _CRT_SECURE_NO_WARNINGS
#include"MIDI_read.hpp"

int main(int argc,char** argv) {
	MIDI_read midi(argv[1]);
	for (int k = 0; k < midi.heard_data->MIDI_track; k++) {
		printf("----------TRACK%d----------\n", k);
		for (int j = 0;; j++) {
			if (midi.track_data[k]->get_track_data(j).data_size == 0) {
				break;
			}
			printf("TIMEF%3d@", (int)midi.track_data[k]->get_track_data(j).time);
			for (int i = 0; i < midi.track_data[k]->get_track_data(j).data_size; i++) {



				printf("0x%02x,", midi.track_data[k]->get_track_data(j).event_data[i]);
			}
			printf("\n");
		}
	}


}