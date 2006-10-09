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
 * This file contains text to canvas rendering functions.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif
#include <stdlib.h>
#include <cucul.h>

#include "toilet.h"
#include "render.h"

static int feed_tiny(context_t *, uint32_t);
static int end_tiny(context_t *);

static int feed_big(context_t *, uint32_t);
static int end_big(context_t *);

int init_tiny(context_t *cx)
{
    cx->ew = 16;
    cx->eh = 2;
    cx->x = cx->y = 0;
    cx->w = cx->h = 0;
    cx->cv = cucul_create_canvas(cx->ew, cx->eh);

    cx->feed = feed_tiny;
    cx->end = end_tiny;

    return 0;
}

static int feed_tiny(context_t *cx, uint32_t ch)
{
    /* Check whether we reached the end of the screen */
    if(cx->x && cx->x + 1 > cx->term_width)
    {
        cx->x = 0;
        cx->y++;
    }

    /* Check whether the current canvas is large enough */
    if(cx->x + 1 > cx->w)
    {
        cx->w = cx->x + 1 < cx->term_width ? cx->x + 1 : cx->term_width;
        if(cx->w > cx->ew)
            cx->ew = cx->ew + cx->ew / 2;
    }

    if(cx->y + 1 > cx->h)
    {
        cx->h = cx->y + 1;
        if(cx->h > cx->eh)
            cx->eh = cx->eh + cx->eh / 2;
    }

    cucul_set_canvas_size(cx->cv, cx->ew, cx->eh);

    switch(ch)
    {
        case (uint32_t)'\r':
            return 0;
        case (uint32_t)'\n':
            cx->x = 0;
            cx->y++;
            break;
        case (uint32_t)'\t':
            cx->x = (cx->x & ~7) + 8;
            break;
        default:
            cucul_putchar(cx->cv, cx->x, cx->y, ch);
            cx->x++;
            break;
    }

    return 0;
}

static int end_tiny(context_t *cx)
{
    cucul_set_canvas_size(cx->cv, cx->w, cx->h);

    return 0;
}

int init_big(context_t *cx)
{
    char const * const * fonts;

    fonts = cucul_get_font_list();
    cx->f = cucul_load_font(fonts[0], 0);
    cx->buf = malloc(4 * cucul_get_font_width(cx->f)
                       * cucul_get_font_height(cx->f));
    cx->onechar = cucul_create_canvas(1, 1);
    cucul_set_color(cx->onechar, CUCUL_COLOR_WHITE, CUCUL_COLOR_BLACK);

    cx->x = cx->y = 0;
    cx->w = cx->h = 0;
    cx->cv = cucul_create_canvas(1, 1);

    cx->feed = feed_big;
    cx->end = end_big;

    return 0;
}

static int feed_big(context_t *cx, uint32_t ch)
{
    unsigned int w = cucul_get_font_width(cx->f);
    unsigned int h = cucul_get_font_height(cx->f);
    unsigned int x, y;

    /* Check whether we reached the end of the screen */
    if(cx->x && cx->x + w > cx->term_width)
    {
        cx->x = 0;
        cx->y += h;
    }

    /* Check whether the current canvas is large enough */
    if(cx->x + w > cx->w)
        cx->w = cx->x + w < cx->term_width ? cx->x + w : cx->term_width;

    if(cx->y + h > cx->h)
        cx->h = cx->y + h;

    cucul_set_canvas_size(cx->cv, cx->w, cx->h);

    /* Render our char */
    cucul_putchar(cx->onechar, 0, 0, ch);
    cucul_render_canvas(cx->onechar, cx->f, cx->buf, w, h, 4 * w);

    for(y = 0; y < h; y++)
       for(x = 0; x < w; x++)
    {
        unsigned char c = cx->buf[4 * (x + y * w) + 2];

        if(c >= 0xa0)
            cucul_putstr(cx->cv, cx->x + x, cx->y + y, "█");
        else if(c >= 0x80)
            cucul_putstr(cx->cv, cx->x + x, cx->y + y, "▓");
        else if(c >= 0x40)
            cucul_putstr(cx->cv, cx->x + x, cx->y + y, "▒");
        else if(c >= 0x20)
            cucul_putstr(cx->cv, cx->x + x, cx->y + y, "░");
    }

    /* Advance cursor */
    cx->x += w;

    return 0;
}

static int end_big(context_t *cx)
{
    cucul_free_canvas(cx->onechar);
    free(cx->buf);
    cucul_free_font(cx->f);

    return 0;
}

