#ifndef FONTS_H
#define FONTS_H

#include <stdint.h>

	// Characters definition
  	// -----------------------------------
	// number of columns for chars
	#define CHARS_COLS_LENGTH  5
	// number of rows for chars
	#define CHARS_ROWS_LENGTH  8

	#define H_LCD 240
	#define W_LCD 320
	#define SIZE_NUMBERS 59
	#define MARGE_NUMBERS_X 5
	#define INTERVALE_NUMBERS 4

	// @const Characters
	extern const uint8_t FONTS5_8[][CHARS_COLS_LENGTH]; // Tableau représentant une police 8x8 (chaque caractère a une largeur de 8px et une hauteur de 8px)

#endif /* FONTS_H */
