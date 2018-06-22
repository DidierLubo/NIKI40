#ifndef _UTILS_H
#define _UTILS_H

#include "stm32f4xx_hal.h"

void init_cycle_counter();
uint32_t read_cycle_counter();
void delay_us(uint32_t time_us);
void delay_ms(uint32_t time_ms);

void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
int16_t compl2_to_int16(uint16_t wert);
int32_t compl2_to_int24(uint32_t wert);
int64_t compl2_to_int48(uint64_t wert);
int16_t bytes_to_int16(uint8_t byte_h, uint8_t byte_l);
void int16_to_bytes(int16_t wert, uint8_t *byte_h, uint8_t *byte_l);
int32_t get_int_param(char *s, uint8_t start_index);
uint32_t get_hex_param(char *s, uint8_t start_index);
double get_double_param(char *s, uint8_t start_index);
void get_string_param(char* src, char* dest, uint8_t start_index);
float convert_frac_int(uint32_t val, uint8_t frac_bits);
void uint32_to_byte_array(uint32_t val, uint8_t *ba);

#endif
