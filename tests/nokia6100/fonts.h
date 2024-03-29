

#ifndef __FONTS_H__
#define __FONTS_H__

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

// ==========================================================================
// structure definition
// ==========================================================================

// This structure describes a single character's display information
typedef struct
{
  const uint8_t width;
  const uint8_t height;					// width, in bits (or pixels), of the character
  const uint16_t offset;					// offset of the character's bitmap, in bytes, into the the FONT_INFO's data array

} FONT_CHAR_INFO;	

// Describes a single font
typedef struct
{
  const uint8_t 			heightPages;	// height, in pages (8 pixels), of the font's characters
  const uint8_t 			startChar;		// the first character in the font (e.g. in charInfo and data)
  const uint8_t                    endChar;        // the last character in the font
  const uint8_t			spacePixels;	// number of pixels that a space character takes up
  const FONT_CHAR_INFO*	charInfo;		// pointer to array of char information
  const uint8_t*			data;			// pointer to generated array of character visual representation

} FONT_INFO;	

// Font sizes 
#define SMALL  0 
#define MEDIUM 1 
#define LARGE  2 
#define SS30PT   3
#define SS59PT   4
 
typedef enum {
  SMALL_font = 0,
  MEDIUM_font = 1,
  LARGE_font = 2,
  SS30PT_font  = 3,
  SS59PT_font  = 4
} font_size_t;


extern const FONT_INFO *font_table[];

#ifdef __cplusplus
}
#endif
#endif
