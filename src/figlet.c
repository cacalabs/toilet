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
 * This file contains functions for handling FIGlet fonts.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cucul.h>

#include "toilet.h"
#include "render.h"
#include "io.h"

#define STD_GLYPHS (127 - 32)
#define EXT_GLYPHS (STD_GLYPHS + 7)

static int feed_figlet(context_t *, uint32_t, uint32_t);
static int flush_figlet(context_t *);
static int end_figlet(context_t *);

static int open_font(context_t *cx);
static uint32_t smush(uint32_t, uint32_t, unsigned int);

int init_figlet(context_t *cx)
{
    if(open_font(cx))
        return -1;

    /* FIXME: read the font's contents */
    if(cx->hlayout == H_DEFAULT)
        cx->hlayout = H_SMUSH;

    cx->charcv = cucul_create_canvas(cx->max_length - 2, cx->height);

    cx->left = malloc(cx->height * sizeof(int));
    cx->right = malloc(cx->height * sizeof(int));

    cx->feed = feed_figlet;
    cx->flush = flush_figlet;
    cx->end = end_figlet;

    return 0;
}

static int feed_figlet(context_t *cx, uint32_t ch, uint32_t attr)
{
    unsigned int c, w, h, x, y, overlap, extra, xleft, xright;

    switch(ch)
    {
        case (uint32_t)'\r':
            return 0;
        case (uint32_t)'\n':
            cx->x = 0;
            cx->y += cx->height;
            return 0;
        /* FIXME: handle '\t' */
    }

    /* Look whether our glyph is available */
    for(c = 0; c < cx->glyphs; c++)
        if(cx->lookup[c * 2] == ch)
            break;

    if(c == cx->glyphs)
        return 0;

    w = cx->lookup[c * 2 + 1];
    h = cx->height;

    cucul_set_canvas_handle(cx->fontcv, 0, c * cx->height);
    cucul_blit(cx->charcv, 0, 0, cx->fontcv, NULL);

    /* Check whether we reached the end of the screen */
    if(cx->x && cx->x + w > cx->term_width)
    {
        cx->x = 0;
        cx->y += h;
    }

    /* Compute how much the next character will overlap */
    switch(cx->hlayout)
    {
    case H_SMUSH:
    case H_KERN:
    case H_OVERLAP:
        extra = (cx->hlayout == H_OVERLAP);
        overlap = w;
        for(y = 0; y < h; y++)
        {
            /* Compute how much spaces we can eat from the new glyph */
            for(xright = 0; xright < overlap; xright++)
                if(cucul_get_char(cx->charcv, xright, y) != ' ')
                    break;

            /* Compute how much spaces we can eat from the previous glyph */
            for(xleft = 0; xright + xleft < overlap && xleft < cx->x; xleft++)
                if(cucul_get_char(cx->cv, cx->x - 1 - xleft, cx->y + y) != ' ')
                    break;

            /* Handle overlapping */
            if(cx->hlayout == H_OVERLAP && xleft < cx->x)
                xleft++;

            /* Handle smushing */
            if(cx->hlayout == H_SMUSH)
            {
                if(xleft < cx->x &&
                   smush(cucul_get_char(cx->charcv, xright, y),
                         cucul_get_char(cx->cv, cx->x - 1 - xleft, cx->y + y),
                         0))
                    xleft++;
            }
 
            if(xleft + xright < overlap)
                overlap = xleft + xright;
        }
        break;
    case H_NONE:
        overlap = 0;
        break;
    default:
        return -1;
    }

    /* Check whether the current canvas is large enough */
    if(cx->x + w - overlap > cx->w)
        cx->w = cx->x + w - overlap < cx->term_width
              ? cx->x + w - overlap : cx->term_width;

    if(cx->y + h > cx->h)
        cx->h = cx->y + h;

    if(attr)
        cucul_set_attr(cx->cv, attr);
    cucul_set_canvas_size(cx->cv, cx->w, cx->h);

    /* Render our char (FIXME: create a rect-aware cucul_blit_canvas?) */
    for(y = 0; y < h; y++)
        for(x = 0; x < w; x++)
    {
        uint32_t ch1, ch2;
        //uint32_t tmpat = cucul_get_attr(cx->fontcv, x, y + c * cx->height);
        ch2 = cucul_get_char(cx->charcv, x, y);
        if(ch2 == ' ')
            continue;
        ch1 = cucul_get_char(cx->cv, cx->x + x - overlap, cx->y + y);
        /* FIXME: this could be changed to cucul_put_attr() when the
         * function is fixed in libcucul */
        //cucul_set_attr(cx->cv, tmpat);
        if(ch1 == ' ' || cx->hlayout != H_SMUSH)
            cucul_put_char(cx->cv, cx->x + x - overlap, cx->y + y, ch2);
        else
            cucul_put_char(cx->cv, cx->x + x - overlap, cx->y + y,
                           smush(ch1, ch2, 0));
        //cucul_put_attr(cx->cv, cx->x + x, cx->y + y, tmpat);
    }

    /* Advance cursor */
    cx->x += w - overlap;

    return 0;
}

static int flush_figlet(context_t *cx)
{
    unsigned int x, y;

    cx->torender = cx->cv;
    cucul_set_canvas_size(cx->torender, cx->w, cx->h);

    /* FIXME: do this somewhere else, or record hardblank positions */
    for(y = 0; y < cx->h; y++)
        for(x = 0; x < cx->w; x++)
            if(cucul_get_char(cx->torender, x, y) == 0xa0)
            {
                uint32_t attr = cucul_get_attr(cx->torender, x, y);
                cucul_put_char(cx->torender, x, y, ' ');
                cucul_put_attr(cx->torender, x, y, attr);
            }

    cx->x = cx->y = 0;
    cx->w = cx->h = 0;
    cx->cv = cucul_create_canvas(1, 1);

    return 0;
}

static int end_figlet(context_t *cx)
{
    free(cx->left);
    free(cx->right);
    cucul_free_canvas(cx->charcv);
    cucul_free_canvas(cx->fontcv);
    free(cx->lookup);

    return 0;
}

static int open_font(context_t *cx)
{
    char *data = NULL;
    char path[2048];
    char buf[2048];
    char hardblank[10];
    TOIFILE *f;
    unsigned int i, j, size, comment_lines;

    /* Open font: try .tlf, then .flf */
    snprintf(path, 2047, "%s/%s.tlf", cx->dir, cx->font);
    path[2047] = '\0';
    f = toiopen(path, "r");
    if(!f)
    {
        snprintf(path, 2047, "%s/%s.flf", cx->dir, cx->font);
        path[2047] = '\0';
        f = toiopen(path, "r");
        if(!f)
        {
            fprintf(stderr, "font `%s' not found\n", path);
            return -1;
        }
    }

    /* Read header */
    cx->print_direction = 0;
    cx->full_layout = 0;
    cx->codetag_count = 0;
    toigets(buf, 2048, f);
    if(sscanf(buf, "%*[ft]lf2a%6s %u %u %u %i %u %u %u %u\n", hardblank,
              &cx->height, &cx->baseline, &cx->max_length,
              &cx->old_layout, &comment_lines, &cx->print_direction,
              &cx->full_layout, &cx->codetag_count) < 6)
    {
        fprintf(stderr, "font `%s' has invalid header: %s\n", path, buf);
        toiclose(f);
        return -1;
    }

    cx->hardblank = cucul_utf8_to_utf32(hardblank, NULL);

    /* Skip comment lines */
    for(i = 0; i < comment_lines; i++)
        toigets(buf, 2048, f);

    /* Read mandatory characters (32-127, 196, 214, 220, 228, 246, 252, 223)
     * then read additional characters. */
    cx->glyphs = 0;
    cx->lookup = NULL;

    for(i = 0, size = 0; !toieof(f); cx->glyphs++)
    {
        if((cx->glyphs % 2048) == 0)
            cx->lookup = realloc(cx->lookup,
                                   (cx->glyphs + 2048) * 2 * sizeof(int));

        if(cx->glyphs < STD_GLYPHS)
        {
            cx->lookup[cx->glyphs * 2] = 32 + cx->glyphs;
        }
        else if(cx->glyphs < EXT_GLYPHS)
        {
            static int const tab[7] = { 196, 214, 220, 228, 246, 252, 223 };
            cx->lookup[cx->glyphs * 2] = tab[cx->glyphs - STD_GLYPHS];
        }
        else
        {
            if(toigets(buf, 2048, f) == NULL)
                break;

            /* Ignore blank lines, as in jacky.flf */
            if(buf[0] == '\n' || buf[0] == '\r')
                continue;

            /* Ignore negative indices for now, as in ivrit.flf */
            if(buf[0] == '-')
            {
                for(j = 0; j < cx->height; j++)
                    toigets(buf, 2048, f);
                continue;
            }

            if(!buf[0] || buf[0] < '0' || buf[0] > '9')
            {
                free(data);
                free(cx->lookup);
                fprintf(stderr, "read error at glyph #%u in `%s'\n",
                                cx->glyphs, path);
                return -1;
            }

            if(buf[1] == 'x')
                sscanf(buf, "%x", &cx->lookup[cx->glyphs * 2]);
            else
                sscanf(buf, "%u", &cx->lookup[cx->glyphs * 2]);
        }

        cx->lookup[cx->glyphs * 2 + 1] = 0;

        for(j = 0; j < cx->height; j++)
        {
            if(i + 2048 >= size)
                data = realloc(data, size += 2048);

            toigets(data + i, 2048, f);
            i = (uintptr_t)strchr(data + i, 0) - (uintptr_t)data;
        }
    }

    toiclose(f);

    if(cx->glyphs < EXT_GLYPHS)
    {
        free(data);
        free(cx->lookup);
        fprintf(stderr, "only %u glyphs in `%s', expected at least %u\n",
                        cx->glyphs, path, EXT_GLYPHS);
        return -1;
    }

    /* Import buffer into canvas */
    cx->fontcv = cucul_create_canvas(0, 0);
    cucul_import_memory(cx->fontcv, data, i, "utf8");
    free(data);

    /* Remove EOL characters. For now we ignore hardblanks, don’t do any
     * smushing, nor any kind of error checking. */
    for(j = 0; j < cx->height * cx->glyphs; j++)
    {
        unsigned long int ch, oldch = 0;

        for(i = cx->max_length; i--;)
        {
            ch = cucul_get_char(cx->fontcv, i, j);

            /* Replace hardblanks with U+00A0 NO-BREAK SPACE */
            if(ch == cx->hardblank)
                cucul_put_char(cx->fontcv, i, j, ch = 0xa0);

            if(oldch && ch != oldch)
            {
                if(!cx->lookup[j / cx->height * 2 + 1])
                    cx->lookup[j / cx->height * 2 + 1] = i + 1;
            }
            else if(oldch && ch == oldch)
                cucul_put_char(cx->fontcv, i, j, ' ');
            else if(ch != ' ')
            {
                oldch = ch;
                cucul_put_char(cx->fontcv, i, j, ' ');
            }
        }
    }

    return 0;
}

static uint32_t smush(uint32_t ch1, uint32_t ch2, unsigned int mode)
{

    /* Rule 1 */
    if(ch1 == ch2 && ch1 != 0xa0)
        return ch2;

    if(ch1 < 0x80 && ch2 < 0x80)
    {
        char const rule2[] = "|/\\[]{}()<>";
        char const rule3[] = "||/\\[]{}()<>";
        char *tmp1, *tmp2;
        uint16_t s, p;

        /* Rule 2 */
        if(ch1 == '_' && strchr(rule2, ch2))
            return ch2;

        /* Rule 3 */
        if((tmp1 = strchr(rule3, ch1)) && (tmp2 = strchr(rule3, ch2)))
        {
            int cl1 = (tmp1 - rule3) / 2;
            int cl2 = (tmp2 - rule3) / 2;

            if(cl1 < cl2)
                return ch2;
            if(cl1 > cl2)
                return ch1;
        }

        /* Rule 4 */
        s = ch1 + ch2;
        p = ch1 * ch2;

        if(p == 15375 /* '{' * '}' */
            || p == 8463 /* '[' * ']' */
            || (p == 1640 && s == 81)) /* '(' *|+ ')' */
            return '|';

        /* Rule 5 */
        switch((ch1 << 8) | ch2)
        {
            case 0x2f5c: return '|'; /* /\ */
            case 0x5c2f: return 'Y'; /* \/ */
            case 0x3e3c: return 'X'; /* >< */
        }

        /* Rule 6 */
        if(ch1 == ch2 && ch1 == 0xa0)
            return 0xa0;
    }

    return 0;
}

