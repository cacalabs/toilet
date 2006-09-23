/*
 *  TOIlet        The Other Implementation’s letters
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the Do What The Fuck You Want To
 *  Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif
#include <stdlib.h>
#include <cucul.h>

#include "render.h"

cucul_canvas_t *render_big(uint32_t const *string, unsigned int length)
{
    cucul_canvas_t *cv;
    cucul_font_t *f;
    char const * const * fonts;
    unsigned char *buf;
    unsigned int w, h, x, y, miny, maxy;

    cv = cucul_create_canvas(length, 1);
    for(x = 0; x < length; x++)
        cucul_putchar(cv, x, 0, string[x]);

    fonts = cucul_get_font_list();
    f = cucul_load_font(fonts[0], 0);

    /* Create our bitmap buffer (32-bit ARGB) */
    w = cucul_get_canvas_width(cv) * cucul_get_font_width(f);
    h = cucul_get_canvas_height(cv) * cucul_get_font_height(f);
    buf = malloc(4 * w * h);

    /* Render the canvas onto our image buffer */
    cucul_render_canvas(cv, f, buf, w, h, 4 * w);

    /* Free our canvas, and allocate a bigger one */
    cucul_free_font(f);
    cucul_free_canvas(cv);
    cv = cucul_create_canvas(w, h);

    /* Render the image buffer on the canvas */
    cucul_set_color(cv, CUCUL_COLOR_WHITE, CUCUL_COLOR_TRANSPARENT);
    cucul_clear_canvas(cv);

    miny = h; maxy = 0;

    for(y = 0; y < h; y++)
       for(x = 0; x < w; x++)
    {
        unsigned char c = buf[4 * (x + y * w) + 2];

        if(c >= 0xa0)
            cucul_putstr(cv, x, y, "█");
        else if(c >= 0x80)
            cucul_putstr(cv, x, y, "▓");
        else if(c >= 0x40)
            cucul_putstr(cv, x, y, "▒");
        else if(c >= 0x20)
            cucul_putstr(cv, x, y, "░");
    }

    free(buf);

    return cv;
}

cucul_canvas_t *render_tiny(uint32_t const *string, unsigned int length)
{
    unsigned int x;
    cucul_canvas_t *cv = cucul_create_canvas(length, 1);

    for(x = 0; x < length; x++)
        cucul_putchar(cv, x, 0, string[x]);

    return cv;
}

