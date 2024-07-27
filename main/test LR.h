#include <stdio.h>
#include "Cprogram.h"
//#include <cstdint>
#include <stdint.h>

void main() {
	uint8_t bytes[] = {3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2 };
	uint8_t buffer[32];
	uint8_t l_buffer[16];
	uint8_t r_buffer[16];
	uint8_t *data2 = (uint8_t*) buffer;

	uint8_t* data = (uint8_t*)bytes;
	size_t item_size = 32;
	
	for (int i = 0; i < item_size; i++) {
		*(data2 + i) = *(data + i);
	
	}

	for (int i = 0; i < item_size; i++){
		printf("buffer_data %d\n", (int) *(data2 + i));

	}
	process_data_LR(data, l_buffer, r_buffer, data2, item_size);

	for (int i = 0; i < item_size/2; i++) {
		printf("out_data L buff %d\n", (int)*(l_buffer + i));

	}
	printf("size L buf %d\n", (int) sizeof(l_buffer));

	for (int i = 0; i < item_size/2; i++) {
		printf("out_data R buff %d\n", (int)*(r_buffer + i));

	}
	printf("size R buf %d\n", (int)sizeof(r_buffer));

	for (int i = 0; i < item_size; i++) {
		printf("Data R %d\n", (int)*(data2 + i));

	}
	printf("size R buffer %d\n", (int)sizeof(data2));

	for (int i = 0; i < item_size; i++) {
		printf("Data L %d\n", (int)*(data + i));

	}
	printf("size L Data %d\n", (int)sizeof(data));
	return 0;
}


static void process_data_LR(uint8_t* data, uint8_t* l_buff, uint8_t* r_buff, uint8_t* data2,  size_t item_size) {

	uint8_t* data_l =  data;
	uint8_t* data_r = data2;

	int y = 0, z = 0;
	for (int i = 0; i < item_size; i = i + 2) {
		l_buff[y] = data2[i];
		r_buff[y] = data2[i + 1];
		y++;
	}

	y = 1;
	for (int i = 0; i < item_size; i++) {
		*(data_l + y) = (int)l_buff[i];
		*(data_r + z) = (int)r_buff[i];
		y= y + 2;
		z= z + 2;
	}

	

}
