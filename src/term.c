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
 * This file contains text to canvas rendering functions.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif
#include <stdlib.h>
#include <caca.h>

#include "toilet.h"
#include "render.h"

static int feed_tiny(context_t *, uint32_t, uint32_t);
static int flush_tiny(context_t *);
static int end_tiny(context_t *);

int init_tiny(context_t *cx)
{
    cx->ew = 16;
    cx->eh = 2;

    cx->feed = feed_tiny;
    cx->flush = flush_tiny;
    cx->end = end_tiny;

    return 0;
}

static int feed_tiny(context_t *cx, uint32_t ch, uint32_t attr)
{
    switch(ch)
    {
        case (uint32_t)'\r':
            return 0;
        case (uint32_t)'\n':
            cx->x = 0;
            cx->y++;
            return 0;
        case (uint32_t)'\t':
            cx->x = (cx->x & ~7) + 8;
            return 0;
    }

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

    caca_set_attr(cx->cv, attr);
    caca_set_canvas_size(cx->cv, cx->ew, cx->eh);

    caca_put_char(cx->cv, cx->x, cx->y, ch);
    cx->x++;

    return 0;
}

static int flush_tiny(context_t *cx)
{
    cx->torender = cx->cv;
    caca_set_canvas_size(cx->torender, cx->w, cx->h);

    cx->ew = 16;
    cx->eh = 2;
    cx->x = cx->y = 0;
    cx->w = cx->h = 0;
    cx->cv = caca_create_canvas(cx->ew, cx->eh);

    return 0;
}

static int end_tiny(context_t *cx)
{
    return 0;
}

