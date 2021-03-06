/*
 * Copyright (c) 2015-2016 Jean Guyomarc'h
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _SPRITE_H_
#define _SPRITE_H_

typedef enum
{
   SPRITE_INFO_NORTH            = 0,
   SPRITE_INFO_NORTH_EAST       = 1,
   SPRITE_INFO_EAST             = 2,
   SPRITE_INFO_SOUTH_EAST       = 3,
   SPRITE_INFO_SOUTH            = 4,
   SPRITE_INFO_SOUTH_WEST,
   SPRITE_INFO_WEST,
   SPRITE_INFO_NORTH_WEST,
   SPRITE_INFO_ICON , /* special value for icons */
} Sprite_Info;

typedef struct
{
   unsigned char *data;
   unsigned int w;
   unsigned int h;
   Pud_Player   color;
} Sprite_Descriptor;


Sprite_Descriptor *sprite_get(Pud_Unit unit, Pud_Era era, Sprite_Info info,
                              Eina_Bool *flip_me);
Eet_File *sprite_buildings_open(Pud_Era era);
Eet_File *sprite_units_open(void);
Sprite_Info sprite_info_random_get(void);

Eina_Bool sprite_init(void);
void sprite_shutdown(void);
void sprite_tile_size_get(Pud_Unit unit, unsigned int *w, unsigned int *h);
cairo_surface_t *sprite_selection_get(unsigned int edge);

#endif /* ! _SPRITE_H_ */
