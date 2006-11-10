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
#include <string.h>
#include <stdio.h>
#include <cucul.h>

#include "toilet.h"
#include "render.h"
#include "filter.h"

static int render_flush(context_t *);

int render_init(context_t *cx)
{
    cx->x = cx->y = 0;
    cx->w = cx->h = 0;
    cx->lines = 0;
    cx->cv = cucul_create_canvas(0, 0);

    if(!strcasecmp(cx->font, "term"))
        return init_tiny(cx);

    return init_figlet(cx);
}

int render_stdin(context_t *cx)
{
    char buf[10];
    unsigned int i = 0, len;
    uint32_t ch;

    /* Read from stdin */
    while(!feof(stdin))
    {
        buf[i++] = getchar();
        buf[i] = '\0';

        ch = cucul_utf8_to_utf32(buf, &len);

        if(!len)
            continue;

        cx->feed(cx, ch);
        i = 0;

        if(ch == '\n')
            render_flush(cx);
    }

    return 0;
}

int render_list(context_t *cx, unsigned int argc, char *argv[])
{
    unsigned int i, j;

    for(i = 0; i < argc; i++)
    {
        /* Read from commandline */
        unsigned int len;

        if(i)
            cx->feed(cx, ' ');

        for(j = 0; argv[i][j];)
        {
            cx->feed(cx, cucul_utf8_to_utf32(argv[i] + j, &len));
            j += len;
        }
    }

    render_flush(cx);

    return 0;
}

int render_end(context_t *cx)
{
    cx->end(cx);
    cucul_free_canvas(cx->cv);

    return 0;
}

/* XXX: Following functions are local */

static int render_flush(context_t *cx)
{
    cucul_buffer_t *buffer;

    /* Flush current line */
    cx->flush(cx);

    /* Apply optional effects to our string */
    filter_do(cx);

    cx->lines += cucul_get_canvas_height(cx->torender);

    /* Output line */
    buffer = cucul_export_canvas(cx->torender, cx->export);
    if(!buffer)
        return -1;
    fwrite(cucul_get_buffer_data(buffer),
           cucul_get_buffer_size(buffer), 1, stdout);
    cucul_free_buffer(buffer);
    cucul_free_canvas(cx->torender);

    return 0;
}

