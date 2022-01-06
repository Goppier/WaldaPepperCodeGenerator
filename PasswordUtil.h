/*
 * PasswordUtil.h
 *
 *  Created on: Dec 18, 2021
 *      Author: Goppier
 */

#ifndef PASSWORDUTIL_H_
#define PASSWORDUTIL_H_

#include <stdint.h>

#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 


struct wallpaper_params_t
{
	uint8_t pattern;
	uint8_t icon;
	uint32_t background_colour;
	uint32_t foreground_colour;
	uint16_t trainer_id;
    uint8_t key;
};

enum english_letter_t
{
	B = 0,
	C, D, F, G, H, J, K, L, M,
	N, P, Q, R, S, T, V, W, Z,
	b, c, d, f, g, h, j, k, m,
	n, p, q, s
};

void prepareArray(uint8_t *array_code, wallpaper_params_t wallpaper);
void bitShiftRight(uint8_t *array_code, uint8_t range, uint8_t bits_to_shift);
void maskArray(uint8_t *array_code, uint8_t key);
void createEnglishCode(uint8_t *array_code, english_letter_t *english_code_buffer);

void printBinaryEnglishCode(uint8_t *array_code, english_letter_t *english_code);
void printSecretEnglishCode(english_letter_t *password);



#endif /* PASSWORDUTIL_H_ */
