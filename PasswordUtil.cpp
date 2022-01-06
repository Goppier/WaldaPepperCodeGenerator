/*
 * PasswordUtil.cpp
 *
 *  Created on: Dec 18, 2021
 *      Author: Goppier
 */

#include <stdio.h>
#include <string.h>
#include "PasswordUtil.h"

/**
 * Prepares the given array with the correct order of values.
 *
 * @param	array_code				Pointer to array where the values can be placed in.
 * @param	wallpaper				Struct that contains all the wallpaper parameters.
 */
void prepareArray(uint8_t *array_code, wallpaper_params_t wallpaper)
{
	uint8_t red = ((wallpaper.background_colour >> 16) & 0xff) >> 3;
	uint8_t green = ((wallpaper.background_colour >> 8) & 0xff) >> 3;
	uint8_t blue = (wallpaper.background_colour & 0xff) >> 3;
	uint16_t bg_colour = (blue << 10) | (green << 5) | red;

	red = ((wallpaper.foreground_colour >> 16) & 0xff) >> 3;
	green = ((wallpaper.foreground_colour >> 8) & 0xff) >> 3;
	blue = (wallpaper.foreground_colour & 0xff) >> 3;
	uint16_t fg_colour = (blue << 10) | (green << 5) | red;

	uint8_t x = (bg_colour & 0xff) ^ (fg_colour & 0xff) ^ wallpaper.icon ^ ((wallpaper.trainer_id >> 8) & 0xff);
	uint8_t y = ((bg_colour >> 8) & 0xff) ^ ((fg_colour >> 8) & 0xff) ^ wallpaper.pattern ^ (wallpaper.trainer_id & 0xff);

	array_code[0] = (bg_colour & 0xff);
	array_code[1] = ((bg_colour >> 8) & 0xff);
	array_code[2] = (fg_colour & 0xff);
	array_code[3] = ((fg_colour >> 8) & 0xff);
	array_code[4] = wallpaper.icon;
	array_code[5] = wallpaper.pattern;
	array_code[6] = x;
	array_code[7] = y;
	array_code[8] = wallpaper.key;
}

/**
 * Bit shifts the entire array to the right a given amount of times. The array is seen as a circulair over the given range,
 * meaning that bits loop back around to the beginning.
 *
 * First bits are shifted in groups of 8 (a byte). Afterwards there is one last loop for the leftover bits.
 *
 * @param	array_code				Pointer to array that contains the secret code in its current state
 * @param	range					Range of bytes affected by the bitshift.
 * @param	bits_to_shift			Total number of bits to shift.
 */
void bitShiftRight(uint8_t *array_code, uint8_t range, uint8_t bits_to_shift)
{
	uint8_t byte_shift = bits_to_shift / 8;					
	uint8_t leftover_bit_shift = bits_to_shift - byte_shift * 8;

	for (uint8_t x = 0; x < byte_shift; x++)
	{
		uint8_t temp = array_code[range];									// Save last value in array
		memmove(&array_code[1], &array_code[0], range);						// Move all affected bytes to the right
		array_code[0] = temp;												// Paste the saved value to the first spot in the array
	}

	uint8_t temp_1 = 0;
	uint8_t temp_2 = 0;
	for (uint8_t x = 0; x <= range; x++)
	{
		temp_2 = ((array_code[x] << (8 - leftover_bit_shift)) & 0xff);		// Save bits and bit shift to the left so it fits in the next byte
		array_code[x] = array_code[x] >> leftover_bit_shift | temp_1;		// Shift bits to the right and paste bits from previous byte behind it
		temp_1 = temp_2;													// Save temp_2 to temp_1 now that it has been used
	}
	array_code[0] |= temp_1;												// Bits from the last byte need to loop back to the first byte
}

/**
 * Masks the entire array (except the key itself) with a mask created with the key.
 *
 * The most significant 4 bits of the key are masked over the entire array.
 *
 * @param	array_code				Pointer to array that contains the secret code in its current state
 * @param	range					Key to create the mask with
 */
void maskArray(uint8_t *array_code, uint8_t key)
{
	uint8_t mask = key & 0b11110000;
	mask = mask | (mask >> 4);
	for (uint8_t x = 0; x < 8; x++)
	{
		array_code[x] ^= mask;
	}
}

/**
 * Creates the English code that can be used to unlock the Wallpaper
 *
 * The code consists of 15 letters and each letter is 5 bits long. The array however only contains 72 bits (9 bytes). It is 3 bits too short.
 * This solution to this is combining the last 2 bits with the first 3 bits of the first index of the array.
 *
 * @param	array_code				Pointer to array that contains the secret code in its current state
 * @param	english_code_buffer		Pointer to the english code buffer to place the code into
 */
void createEnglishCode(uint8_t *array_code, english_letter_t *english_code_buffer)
{
	uint8_t u_index = 0;
	uint8_t temp = 0;
	uint8_t bits_to_shift = 0;

	for (uint8_t x = 0; x < 15; x++)
	{
		english_code_buffer[x] = (english_letter_t)((array_code[u_index] >> (bits_to_shift) | temp) >> (8 - 5));	// Create a new letter
		bits_to_shift = (u_index + 1) * 8 - (x + 1) * 5;															// Calculate the number of bits unused
		temp = array_code[u_index] << (8 - bits_to_shift);															// Save the unused bits

		if (bits_to_shift < 5)
		{
			u_index = (u_index < 8) ? u_index + 1 : 0;																// Increase array index or loop back to zero if max is reached
		}
	}
}

/**
 * Prints the secret code to the terminal
 *
 * @param	english_code_buffer		Pointer to the english code buffer that contains the secret code
 */
void printSecretEnglishCode(english_letter_t *english_code_buffer)
{
	char password_string[16] = { 0 };
	for (uint8_t x = 0; x <= 14; x++)
	{
		switch (english_code_buffer[x])
		{
		case B:	password_string[x] = 'B';	break;
		case C: password_string[x] = 'C';	break;
		case D: password_string[x] = 'D';	break;
		case F:	password_string[x] = 'F';	break;
		case G:	password_string[x] = 'G';	break;
		case H:	password_string[x] = 'H';	break;
		case J:	password_string[x] = 'J';	break;
		case K:	password_string[x] = 'K';	break;
		case L:	password_string[x] = 'L';	break;
		case M:	password_string[x] = 'M';	break;
		case N:	password_string[x] = 'N';	break;
		case P:	password_string[x] = 'P';	break;
		case Q:	password_string[x] = 'Q';	break;
		case R:	password_string[x] = 'R';	break;
		case S:	password_string[x] = 'S';	break;
		case T:	password_string[x] = 'T';	break;
		case V:	password_string[x] = 'V';	break;
		case W:	password_string[x] = 'W';	break;
		case Z:	password_string[x] = 'Z';	break;
		case b:	password_string[x] = 'b';	break;
		case c:	password_string[x] = 'c';	break;
		case d:	password_string[x] = 'd';	break;
		case f:	password_string[x] = 'f';	break;
		case g:	password_string[x] = 'g';	break;
		case h:	password_string[x] = 'h';	break;
		case j:	password_string[x] = 'j';	break;
		case k:	password_string[x] = 'k';	break;
		case m:	password_string[x] = 'm';	break;
		case n:	password_string[x] = 'n';	break;
		case p:	password_string[x] = 'p';	break;
		case q:	password_string[x] = 'q';	break;
		case s:	password_string[x] = 's';	break;
		}
	}
	printf("%s\n", password_string);
}

/**
 * Prints both the array and the created english code in binary form. This is only used for debugging
 *
 * @param	array_code				Pointer to array that contains the secret code in its current state
 * @param	english_code_buffer		Pointer to the english code buffer that contains the secret code
 */
void printBinaryEnglishCode(uint8_t *array_code, english_letter_t *english_code)
{
	printf("What it should be:\n");
	printf("%c%c%c%c%c\n%c%c%c", BYTE_TO_BINARY(array_code[0]));
	printf("%c%c\n%c%c%c%c%c\n%c", BYTE_TO_BINARY(array_code[1]));
	printf("%c%c%c%c\n%c%c%c%c", BYTE_TO_BINARY(array_code[2]));
	printf("%c\n%c%c%c%c%c\n%c%c", BYTE_TO_BINARY(array_code[3]));
	printf("%c%c%c\n%c%c%c%c%c\n", BYTE_TO_BINARY(array_code[4]));
	printf("%c%c%c%c%c\n%c%c%c", BYTE_TO_BINARY(array_code[5]));
	printf("%c%c\n%c%c%c%c%c\n%c", BYTE_TO_BINARY(array_code[6]));
	printf("%c%c%c%c\n%c%c%c%c", BYTE_TO_BINARY(array_code[7]));
	printf("%c\n%c%c%c%c%c\n%c%c", BYTE_TO_BINARY(array_code[8]));
	printf("%c%c%c\n\n", BYTE_TO_BINARY(array_code[0]));

	printf("What it is:\n");
	for (uint8_t x = 0; x <= 14; x++)
	{
		printf("%c%c%c%c%c\n", BYTE_TO_BINARY(english_code[x] << 3));
	}
	printf("\n");
}
