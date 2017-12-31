#ifndef _FONT_H_
#define _FONT_H_

int font_get_height(const char* str);
int font_get_width(const char* str);

void draw_string(const char* str, int y_pos, int x_offset, int color);

#endif
