/*
 * main.cpp
 *
 *  Created on: Dec 18, 2021
 *      Author: Goppier
 */

#include "./PasswordUtil.h"
#include <stdio.h>

void printAllEnglishKeys(uint8_t *U, wallpaper_params_t wallpaper);

int main()
{
	wallpaper_params_t wallpaper;
	uint8_t U[9] = { 0 };
	english_letter_t code[15] = { B };

	wallpaper.pattern = 11;						// Pikachu
	wallpaper.icon = 1;							// Heart
	wallpaper.background_colour = 0xf850a1;		// Red = 248; Green = 80; Blue = 160
	wallpaper.foreground_colour = 0xf850f8;		// Red = 248; Green = 80; Blue = 248

	wallpaper.trainer_id = 38381;
	wallpaper.key = 0;


	printAllEnglishKeys(U, wallpaper);

	while (1);
}

void printAllEnglishKeys(uint8_t *U, wallpaper_params_t wallpaper)
{
	english_letter_t code[15] = { B };
	for (uint16_t x = 0; x <= 255; x++)
	{
		wallpaper.key = x;
		prepareArray(U, wallpaper);
		maskArray(U, wallpaper.key);
		bitShiftRight(U, 7, wallpaper.key % 16);
		bitShiftRight(U, 8, 21);
		createEnglishCode(U, code);
		//printBinaryEnglishCode(U, code);		//For debugging
		printf("Key %03d: ", wallpaper.key);
		printSecretEnglishCode(code);
	}
}
