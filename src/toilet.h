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
 * This header defines global variables.
 */

struct toilet_context
{
    char const *export;
    char const *font;
    char const *dir;

    unsigned int term_width;

    cucul_canvas_t *cv;
    cucul_canvas_t *torender;
    unsigned int w, h, ew, eh, x, y, lines;

    /* Render methods */
    int (*feed)(struct toilet_context *, uint32_t);
    int (*flush)(struct toilet_context *);
    int (*end)(struct toilet_context *);

    /* Used by the big driver */
    cucul_font_t *f;
    cucul_canvas_t *onechar;
    unsigned char *buf;

    /* Used by the FIGlet driver */
    unsigned long int hardblank;
    unsigned int height, baseline, max_length;
    int old_layout;
    unsigned int print_direction, full_layout, codetag_count;
    unsigned int glyphs;
    cucul_canvas_t *image;
    unsigned int *lookup;

    /* Render filters */
    void (**filters)(cucul_canvas_t *);
    unsigned int nfilters;
};

typedef struct toilet_context context_t;

