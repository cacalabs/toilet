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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cucul.h>

#include "toilet.h"
#include "filter.h"

static void filter_crop(context_t *);
static void filter_gay(context_t *);
static void filter_metal(context_t *);
static void filter_flip(context_t *);
static void filter_flop(context_t *);
static void filter_rotate(context_t *);

struct
{
    char const *name;
    void (*function)(context_t *);
}
const lookup[] =
{
    { "crop", filter_crop },
    { "gay", filter_gay },
    { "metal", filter_metal },
    { "flip", filter_flip },
    { "flop", filter_flop },
    { "rotate", filter_rotate },
};

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

    xmin = w = cucul_get_canvas_width(cx->torender);
    xmax = 0;
    ymin = h = cucul_get_canvas_height(cx->torender);
    ymax = 0;

    for(y = 0; y < h; y++)
        for(x = 0; x < w; x++)
    {
        unsigned long int ch = cucul_getchar(cx->torender, x, y);
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

    cucul_set_canvas_boundaries(cx->torender, xmin, ymin,
                                xmax - xmin + 1, ymax - ymin + 1);
}

static void filter_metal(context_t *cx)
{
    static unsigned char const palette[] =
    {
        CUCUL_COLOR_LIGHTBLUE,
        CUCUL_COLOR_BLUE,
        CUCUL_COLOR_LIGHTGRAY,
        CUCUL_COLOR_DARKGRAY,
    };

    unsigned int x, y, w, h;

    w = cucul_get_canvas_width(cx->torender);
    h = cucul_get_canvas_height(cx->torender);

    for(y = 0; y < h; y++)
        for(x = 0; x < w; x++)
    {
        unsigned long int ch = cucul_getchar(cx->torender, x, y);
        int i;

        if(ch == (unsigned char)' ')
            continue;

        i = ((cx->lines + y + x / 8) / 2) % 4;
        cucul_set_color(cx->torender, palette[i], CUCUL_COLOR_TRANSPARENT);
        cucul_putchar(cx->torender, x, y, ch);
    }
}

static void filter_gay(context_t *cx)
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

    w = cucul_get_canvas_width(cx->torender);
    h = cucul_get_canvas_height(cx->torender);

    for(y = 0; y < h; y++)
        for(x = 0; x < w; x++)
    {
        unsigned long int ch = cucul_getchar(cx->torender, x, y);
        if(ch != (unsigned char)' ')
        {
            cucul_set_color(cx->torender, rainbow[(x / 2 + y + cx->lines) % 6],
                                CUCUL_COLOR_TRANSPARENT);
            cucul_putchar(cx->torender, x, y, ch);
        }
    }
}

static void filter_flip(context_t *cx)
{
    cucul_flip(cx->torender);
}

static void filter_flop(context_t *cx)
{
    cucul_flop(cx->torender);
}

static void filter_rotate(context_t *cx)
{
    cucul_rotate(cx->torender);
}
