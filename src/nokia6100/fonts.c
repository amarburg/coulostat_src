

#include "fonts.h"

// Unorthodox, but lets me use font files exactly as they arrive from 
// LCD Factory

#include "fonts/lynch_fonts.c"
#include "fonts/ss30pt.c"
#include "fonts/ss59pt.c"


const FONT_INFO *font_table[] = {
  (const FONT_INFO *)&FONT6x8_Info,
  (const FONT_INFO *)&FONT8x8_Info,
  (const FONT_INFO *)&FONT8x16_Info,
  (const FONT_INFO *)&microsoftSansSerif30pt_Info,
  (const FONT_INFO *)&microsoftSansSerif59pt_Info
};

