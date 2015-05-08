/*
 *  TOIlet        The Other Implementation’s letters
 *  Copyright (c) 2006 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

/*
 * This file contains post-processing filter functions.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <caca.h>

#include "toilet.h"
#include "filter.h"

static void filter_crop(context_t *);
static void filter_rainbow(context_t *);
static void filter_metal(context_t *);
static void filter_flip(context_t *);
static void filter_flop(context_t *);
static void filter_180(context_t *);
static void filter_left(context_t *);
static void filter_right(context_t *);
static void filter_border(context_t *);

struct
{
    char const *name;
    void (*function)(context_t *);
    char const *description;
}
const lookup[] =
{
    { "crop", filter_crop, "crop unused blanks" },
    { "rainbow", filter_rainbow, "add a rainbow colour effect" },
    { "metal", filter_metal, "add a metallic colour effect" },
    { "flip", filter_flip, "flip horizontally" },
    { "flop", filter_flop, "flip vertically" },
    { "rotate", filter_180, NULL }, /* backwards compatibility */
    { "180", filter_180, "rotate 180 degrees" },
    { "left", filter_left, "rotate 90 degrees counterclockwise" },
    { "right", filter_right, "rotate 90 degrees clockwise" },
    { "border", filter_border, "surround text with a border" },
};

int filter_list(void)
{
    unsigned int i;

    printf("Available filters:\n");
    for(i = 0; i < sizeof(lookup) / sizeof(lookup[0]); i++)
        if(lookup[i].description)
            printf("\"%s\": %s\n", lookup[i].name, lookup[i].description);

    return 0;
}

int filter_add(context_t *cx, char const *filter)
{
    unsigned int n;
    int i;

    for(;;)
    {
        while(*filter == ':')
            filter++;

        if(*filter == '\0')
            break;

        for(i = sizeof(lookup) / sizeof(lookup[0]); i--; )
            if(!strncmp(filter, lookup[i].name, strlen(lookup[i].name)))
            {
                n = strlen(lookup[i].name);
                break;
            }

        if(i == -1 || (filter[n] != ':' && filter[n] != '\0'))
        {
            fprintf(stderr, "unknown filter near `%s'\n", filter);
            return -1;
        }

        if((cx->nfilters % 16) == 0)
            cx->filters = realloc(cx->filters, (cx->nfilters + 16)
                                                 * sizeof(lookup[0].function));
        cx->filters[cx->nfilters] = lookup[i].function;
        cx->nfilters++;

        filter += n;
    }

    return 0;
}

int filter_do(context_t *cx)
{
    unsigned int i;

    for(i = 0; i < cx->nfilters; i++)
        cx->filters[i](cx);

    return 0;
}

int filter_end(context_t *cx)
{
    free(cx->filters);

    return 0;
}

static void filter_crop(context_t *cx)
{
    unsigned int x, y, w, h;
    unsigned int xmin, xmax, ymin, ymax;

    xmin = w = caca_get_canvas_width(cx->torender);
    xmax = 0;
    ymin = h = caca_get_canvas_height(cx->torender);
    ymax = 0;

    for(y = 0; y < h; y++)
        for(x = 0; x < w; x++)
    {
        unsigned long int ch = caca_get_char(cx->torender, x, y);
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

    caca_set_canvas_boundaries(cx->torender, xmin, ymin,
                                xmax - xmin + 1, ymax - ymin + 1);
}

static void filter_metal(context_t *cx)
{
    static unsigned char const palette[] =
    {
        CACA_LIGHTBLUE, CACA_BLUE, CACA_LIGHTGRAY, CACA_DARKGRAY,
    };

    unsigned int x, y, w, h;

    w = caca_get_canvas_width(cx->torender);
    h = caca_get_canvas_height(cx->torender);

    for(y = 0; y < h; y++)
        for(x = 0; x < w; x++)
    {
        unsigned long int ch = caca_get_char(cx->torender, x, y);
        int i;

        if(ch == (unsigned char)' ')
            continue;

        i = ((cx->lines + y + x / 8) / 2) % 4;
        caca_set_color_ansi(cx->torender, palette[i], CACA_TRANSPARENT);
        caca_put_char(cx->torender, x, y, ch);
    }
}

static void filter_rainbow(context_t *cx)
{
    static unsigned char const rainbow[] =
    {
        CACA_LIGHTMAGENTA, CACA_LIGHTRED, CACA_YELLOW,
        CACA_LIGHTGREEN, CACA_LIGHTCYAN, CACA_LIGHTBLUE,
    };
    unsigned int x, y, w, h;

    w = caca_get_canvas_width(cx->torender);
    h = caca_get_canvas_height(cx->torender);

    for(y = 0; y < h; y++)
        for(x = 0; x < w; x++)
    {
        unsigned long int ch = caca_get_char(cx->torender, x, y);
        if(ch != (unsigned char)' ')
        {
            caca_set_color_ansi(cx->torender,
                                 rainbow[(x / 2 + y + cx->lines) % 6],
                                 CACA_TRANSPARENT);
            caca_put_char(cx->torender, x, y, ch);
        }
    }
}

static void filter_flip(context_t *cx)
{
    caca_flip(cx->torender);
}

static void filter_flop(context_t *cx)
{
    caca_flop(cx->torender);
}

static void filter_180(context_t *cx)
{
    caca_rotate_180(cx->torender);
}

static void filter_left(context_t *cx)
{
    caca_rotate_left(cx->torender);
}

static void filter_right(context_t *cx)
{
    caca_rotate_right(cx->torender);
}

static void filter_border(context_t *cx)
{
    int w, h;

    w = caca_get_canvas_width(cx->torender);
    h = caca_get_canvas_height(cx->torender);

    caca_set_canvas_boundaries(cx->torender, -1, -1, w + 2, h + 2);

    caca_draw_cp437_box(cx->torender, 0, 0, w + 2, h + 2);
}

