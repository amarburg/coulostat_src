

#include "fonts.h"

// Unorthodox, but lets me use font files exactly as they arrive from 
// LCD Factory

#include "fonts/lynch_fonts.c"
#include "fonts/huge.c"


const FONT_INFO *font_table[] = {
  (const FONT_INFO *)&FONT6x8_Info,
  (const FONT_INFO *)&FONT8x8_Info,
  (const FONT_INFO *)&FONT8x16_Info,
  (const FONT_INFO *)&BIGFONT_Info
};

