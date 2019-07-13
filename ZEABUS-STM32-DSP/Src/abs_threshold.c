/*
 * abs_threshold.c
 *
 *  Created on: Jun 2, 2018
 *      Author: Vasut
 */
#include "common.h"
#include "abs_threshold.h"

float  power[4];
uint32_t signal_temp_int[4];
float signal_temp[4];


int abs_threshold(){

	if(g_adc1_2_buffer[g_raw_data_index] > g_raw_front_thres){
		g_pulse_detect_index = g_raw_data_index;
		return 1;
	}

	return 0;
}

int abs_threshold_CFAR() {

	float temp[4];
	int i,k;
	int num_samples = 50;
	uint8_t ch_status = 0x00;

	signal_temp_int[0] = g_adc1_2_buffer[g_raw_data_index];
	signal_temp_int[1] = g_adc1_2_buffer[g_raw_data_index + 1];
	signal_temp_int[2] = g_adc3_4_buffer[g_raw_data_index];
	signal_temp_int[3] = g_adc3_4_buffer[g_raw_data_index + 1];

	signal_temp[0] = (float32_t)((float32_t)(g_adc1_2_buffer[g_raw_data_index])  - 32768.0) / 65535.0;
	signal_temp[1] = (float32_t)((float32_t)(g_adc1_2_buffer[g_raw_data_index + 1])  - 32768.0) / 65535.0;
	signal_temp[2] = (float32_t)((float32_t)(g_adc3_4_buffer[g_raw_data_index])  - 32768.0) / 65535.0;
	signal_temp[3] = (float32_t)((float32_t)(g_adc3_4_buffer[g_raw_data_index + 1])  - 32768.0) / 65535.0;


	power[0] = signal_temp[0] * signal_temp[0];
	power[1] = signal_temp[1] * signal_temp[1];
	power[2] = signal_temp[2] * signal_temp[2];
	power[3] = signal_temp[3] * signal_temp[3];

    if ((power[0]>input.PowerThreshold ) && (power[1]>input.PowerThreshold ) && (power[2]>input.PowerThreshold ) && (power[3]>input.PowerThreshold )) {
			temp[0] = 0;
			temp[1] = 0;
			temp[2] = 0;
			temp[3] = 0;

			for (i = 2; i<=num_samples ; i+=2){
				k = g_raw_data_index - i;
				if (k < 0 ) {
					k = RAW_DATA_BUFFER_SIZE + k ;
				}

				signal_temp[0] = (float32_t)((float32_t)(g_adc1_2_buffer[k])  - 32768.0) / 65535.0;
				signal_temp[1] = (float32_t)((float32_t)(g_adc1_2_buffer[k + 1])  - 32768.0) / 65535.0;
				signal_temp[2] = (float32_t)((float32_t)(g_adc3_4_buffer[k])  - 32768.0) / 65535.0;
				signal_temp[3] = (float32_t)((float32_t)(g_adc3_4_buffer[k + 1])  - 32768.0) / 65535.0;

				temp[0] += signal_temp[0] * signal_temp[0];
				temp[1] += signal_temp[1] * signal_temp[1];
				temp[2] += signal_temp[2] * signal_temp[2];
				temp[3] += signal_temp[3] * signal_temp[3];
			}

			temp[0] = temp[0]/num_samples ;
			temp[1] = temp[1]/num_samples ;
			temp[2] = temp[2]/num_samples ;
			temp[3] = temp[3]/num_samples ;
			if (temp[0] < input.FrontThreshold * power[0]) {
				ch_status |= 1;
			}
			if (temp[1] < input.FrontThreshold * power[1]) {
				ch_status |= 1 << 1;
			}
			if (temp[2] < input.FrontThreshold * power[2]) {
				ch_status |= 1 << 2;
			}
			if (temp[3] < input.FrontThreshold * power[3]) {
				ch_status |= 1 << 3;
			}

			if(ch_status & 0xf){
				g_pulse_detect_index = g_raw_data_index;
				return 1;
			}
	}

	return 0;

}
