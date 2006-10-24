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

/*
 * This file contains post-processing filter functions.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif
#include <cucul.h>

#include "filter.h"

void filter_autocrop(cucul_canvas_t *cv)
{
    unsigned int x, y, w, h;
    unsigned int xmin, xmax, ymin, ymax;

    xmin = w = cucul_get_canvas_width(cv);
    xmax = 0;
    ymin = h = cucul_get_canvas_height(cv);
    ymax = 0;

    for(y = 0; y < h; y++)
        for(x = 0; x < w; x++)
    {
        unsigned long int ch = cucul_getchar(cv, x, y);
        if(ch != (unsigned char)' ')
        {
            if(x < xmin)
                xmin = x;
            if(x > xmax)
                xmax = x;
            if(y < ymin)
                ymin = y;
            if(y > ymax)
                ymax = y;
        }
    }

    if(xmax < xmin || ymax < ymin)
        return;

    cucul_set_canvas_boundaries(cv, xmin, ymin,
                                xmax - xmin + 1, ymax - ymin + 1);
}

void filter_metal(cucul_canvas_t *cv)
{
    static unsigned char const palette[] =
    {
        CUCUL_COLOR_LIGHTBLUE,
        CUCUL_COLOR_BLUE,
        CUCUL_COLOR_LIGHTGRAY,
        CUCUL_COLOR_DARKGRAY,
    };

    unsigned int x, y, w, h;

    w = cucul_get_canvas_width(cv);
    h = cucul_get_canvas_height(cv);

    for(y = 0; y < h; y++)
        for(x = 0; x < w; x++)
    {
        unsigned long int ch = cucul_getchar(cv, x, y);
        int i;

        if(ch == (unsigned char)' ')
            continue;

        i = y * 4 / h;
        cucul_set_color(cv, palette[i], CUCUL_COLOR_TRANSPARENT);
        cucul_putchar(cv, x, y, ch);
    }
}

void filter_gay(cucul_canvas_t *cv)
{
    static unsigned char const rainbow[] =
    {
        CUCUL_COLOR_LIGHTMAGENTA,
        CUCUL_COLOR_LIGHTRED,
        CUCUL_COLOR_YELLOW,
        CUCUL_COLOR_LIGHTGREEN,
        CUCUL_COLOR_LIGHTCYAN,
        CUCUL_COLOR_LIGHTBLUE,
    };
    unsigned int x, y, w, h;

    w = cucul_get_canvas_width(cv);
    h = cucul_get_canvas_height(cv);

    for(y = 0; y < h; y++)
        for(x = 0; x < w; x++)
    {
        unsigned long int ch = cucul_getchar(cv, x, y);
        if(ch != (unsigned char)' ')
        {
            cucul_set_color(cv, rainbow[(x / 2 + y) % 6],
                                CUCUL_COLOR_TRANSPARENT);
            cucul_putchar(cv, x, y, ch);
        }
    }
}

