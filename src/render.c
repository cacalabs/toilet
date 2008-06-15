/*
 *  TOIlet        The Other Implementation’s letters
 *  Copyright (c) 2006 Sam Hocevar <sam@zoy.org>
 *                All Rights Reserved
 *
 *  $Id$
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
    cucul_canvas_t *cv;
    char *line;
    int i, len;

    /* FIXME: we can't read longer lines */
    len = 1024;
    line = malloc(len);
    cv = cucul_create_canvas(0, 0);

    /* Read from stdin */
    while(!feof(stdin))
    {
        if(!fgets(line, len, stdin))
            break;

        cucul_set_canvas_size(cv, 0, 0);
        cucul_import_memory(cv, line, strlen(line), "utf8");
        for(i = 0; i < cucul_get_canvas_width(cv); i++)
        {
            uint32_t ch = cucul_get_char(cv, i, 0);
            uint32_t at = cucul_get_attr(cv, i, 0);
            cx->feed(cx, ch, at);
            if(cucul_utf32_is_fullwidth(ch)) i++;
        }

        render_flush(cx);
    }

    free(line);

    return 0;
}

int render_list(context_t *cx, int argc, char *argv[])
{
    cucul_canvas_t *cv;
    int i, j, len;
    char *parser = NULL;

    cv = cucul_create_canvas(0, 0);

    for(j = 0; j < argc; )
    {
        char *cr;

        if(!parser)
        {
            if(j)
                cx->feed(cx, ' ', 0);
            parser = argv[j];
        }

        cr = strchr(parser, '\n');
        if(cr)
            len = (cr - parser) + 1;
        else
            len = strlen(parser);

        cucul_set_canvas_size(cv, 0, 0);
        cucul_import_memory(cv, parser, len, "utf8");
        for(i = 0; i < cucul_get_canvas_width(cv); i++)
        {
            uint32_t ch = cucul_get_char(cv, i, 0);
            uint32_t at = cucul_get_attr(cv, i, 0);
            cx->feed(cx, ch, at);
            if(cucul_utf32_is_fullwidth(ch)) i++;
        }

        if(cr)
        {
            parser += len;
            render_flush(cx);
        }
        else
        {
            parser = NULL;
            j++;
        }
    }

    render_flush(cx);

    cucul_free_canvas(cv);

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
    unsigned long int len;
    void *buffer;

    /* Flush current line */
    cx->flush(cx);

    /* Apply optional effects to our string */
    filter_do(cx);

    /* Output line */
    buffer = cucul_export_memory(cx->torender, cx->export, &len);
    if(!buffer)
        return -1;
    fwrite(buffer, len, 1, stdout);
    free(buffer);
    cucul_free_canvas(cx->torender);

    return 0;
}

