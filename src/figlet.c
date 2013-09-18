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
 * This file contains functions for handling FIGlet fonts.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <caca.h>

#include "toilet.h"
#include "render.h"

#define STD_GLYPHS (127 - 32)
#define EXT_GLYPHS (STD_GLYPHS + 7)

static int feed_figlet(context_t *, uint32_t, uint32_t);
static int flush_figlet(context_t *);
static int end_figlet(context_t *);

int init_figlet(context_t *cx)
{
    char path[2048];

    snprintf(path, 2047, "%s/%s", cx->dir, cx->font);
    if(caca_canvas_set_figfont(cx->cv, path))
    {
        snprintf(path, 2047, "./%s", cx->font);
        if(caca_canvas_set_figfont(cx->cv, path))
        {
            fprintf(stderr, "error: could not load font %s\n", cx->font);
            return -1;
        }
    }

    caca_set_figfont_smush(cx->cv, cx->hmode);
    caca_set_figfont_width(cx->cv, cx->term_width);

    cx->feed = feed_figlet;
    cx->flush = flush_figlet;
    cx->end = end_figlet;

    return 0;
}

static int feed_figlet(context_t *cx, uint32_t ch, uint32_t attr)
{
    return caca_put_figchar(cx->cv, ch);
}

static int flush_figlet(context_t *cx)
{
    /* We copy cx->cv into cx->torender instead of swapping pointers
     * because that would lose the figfont information. */
    /* FIXME: use caca_copy_canvas() or whatever when it's implemented. */
    int ret = caca_flush_figlet(cx->cv);
    cx->torender = caca_create_canvas(caca_get_canvas_width(cx->cv),
                                      caca_get_canvas_height(cx->cv));
    caca_blit(cx->torender, 0, 0, cx->cv, NULL);
    caca_set_canvas_size(cx->cv, 0, 0);
    return ret;
}

static int end_figlet(context_t *cx)
{
    return caca_canvas_set_figfont(cx->cv, NULL);
}

