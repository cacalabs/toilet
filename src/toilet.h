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
 * This header defines global variables.
 */

struct toilet_context
{
    char const *export;
    char const *font;
    char const *dir;

    unsigned int term_width;

    caca_canvas_t *cv;
    caca_canvas_t *torender;
    unsigned int w, h, ew, eh, x, y, lines;

    /* Render methods */
    int (*feed)(struct toilet_context *, uint32_t, uint32_t);
    int (*flush)(struct toilet_context *);
    int (*end)(struct toilet_context *);

    /* Used by the FIGlet driver */
    char const *hmode;
    unsigned int *lookup;

    /* Render filters */
    void (**filters)(struct toilet_context *);
    unsigned int nfilters;
};

typedef struct toilet_context context_t;

