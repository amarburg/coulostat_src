

#include "fonts.h"

// Unorthodox, but lets me use font files exactly as they arrive from 
// LCD Factory

#include "fonts/huge.c"


const FONT_INFO *font_table[] = {
  (const FONT_INFO *)&BIGFONT_Info
};

