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
#include "figlet.h"

#define STD_GLYPHS (127 - 32)
#define EXT_GLYPHS (STD_GLYPHS + 7)

struct figfont
{
    /* From the font format */
    unsigned long int hardblank;
    unsigned int height, baseline, max_length;
    int old_layout;
    unsigned int print_direction, full_layout, codetag_count;

    unsigned int glyphs;
    cucul_canvas_t *image;
    unsigned int *lookup;
};

static struct figfont *open_font(void);
static void free_font(struct figfont *);

cucul_canvas_t *render_figlet(uint32_t const *string, unsigned int length)
{
    cucul_canvas_t *cv;
    struct figfont *font;
    unsigned int x, i, c;

    font = open_font();

    if(!font)
        return NULL;

    cv = cucul_create_canvas(length * font->max_length, font->height);

    for(x = 0, i = 0; i < length; i++)
    {
        for(c = 0; c < font->glyphs; c++)
            if(font->lookup[c * 2] == string[i])
                break;

        if(c == font->glyphs)
            continue;

        cucul_blit(cv, x, - (int)(c * font->height), font->image, NULL);

        x += font->lookup[c * 2 + 1];
    }

    cucul_set_canvas_boundaries(cv, 0, 0, x, font->height);

    free_font(font);

    return cv;
}

static struct figfont *open_font(void)
{
    char *data = NULL;
    char path[2048];
    char hardblank[10];
    struct figfont *font;
    cucul_buffer_t *b;
    FILE *f;
    unsigned int i, j, size, comment_lines;

    /* Open font: try .tlf, then .flf */
    snprintf(path, 2047, "%s/%s.tlf", toilet_dir, toilet_font);
    path[2047] = '\0';
    f = fopen(path, "r");
    if(!f)
    {
        snprintf(path, 2047, "%s/%s.flf", toilet_dir, toilet_font);
        path[2047] = '\0';
        f = fopen(path, "r");
        if(!f)
        {
            fprintf(stderr, "font `%s' not found\n", path);
            return NULL;
        }
    }

    font = malloc(sizeof(struct figfont));

    /* Read header */
    font->print_direction = 0;
    font->full_layout = 0;
    font->codetag_count = 0;
    if(fscanf(f, "%*[ft]lf2a%6s %u %u %u %i %u %u %u %u\n", hardblank,
              &font->height, &font->baseline, &font->max_length,
              &font->old_layout, &comment_lines, &font->print_direction,
              &font->full_layout, &font->codetag_count) < 6)
    {
        fprintf(stderr, "font `%s' has invalid header\n", path);
        free(font);
        fclose(f);
        return NULL;
    }

    font->hardblank = cucul_utf8_to_utf32(hardblank, NULL);

    /* Skip comment lines */
    for(i = 0; i < comment_lines; i++)
    {
        fscanf(f, "%*[^\n]");
        fscanf(f, "%*c");
    }

    /* Read mandatory characters (32-127, 196, 214, 220, 228, 246, 252, 223)
     * then read additional characters. */
    font->glyphs = 0;
    font->lookup = NULL;

    for(i = 0, size = 0; !feof(f); font->glyphs++)
    {
        if((font->glyphs % 2048) == 0)
            font->lookup = realloc(font->lookup,
                                   (font->glyphs + 2048) * 2 * sizeof(int));

        if(font->glyphs < STD_GLYPHS)
        {
            font->lookup[font->glyphs * 2] = 32 + font->glyphs;
        }
        else if(font->glyphs < EXT_GLYPHS)
        {
            static int const tab[7] = { 196, 214, 220, 228, 246, 252, 223 };
            font->lookup[font->glyphs * 2] = tab[font->glyphs - STD_GLYPHS];
        }
        else
        {
            char number[10];
            int ret = fscanf(f, "%s %*[^\n]", number);

            if(ret == EOF)
                break;

            if(!ret)
            {
                free(data);
                free(font->lookup);
                free(font);
                fprintf(stderr, "read error at glyph %u in `%s'\n",
                                font->glyphs, path);
                return NULL;
            }

            if(number[1] == 'x')
                sscanf(number, "%x", &font->lookup[font->glyphs * 2]);
            else
                sscanf(number, "%u", &font->lookup[font->glyphs * 2]);

            fscanf(f, "%*c");
        }

        font->lookup[font->glyphs * 2 + 1] = 0;

        for(j = 0; j < font->height; j++)
        {
            if(i + 2048 >= size)
                data = realloc(data, size += 2048);

            fgets(data + i, 2048, f);
            i = (uintptr_t)strchr(data + i, 0) - (uintptr_t)data;
        }
    }

    fclose(f);

    if(font->glyphs < EXT_GLYPHS)
    {
        free(data);
        free(font->lookup);
        free(font);
        fprintf(stderr, "only %u glyphs in `%s', expected at least %u\n",
                        font->glyphs, path, EXT_GLYPHS);
        return NULL;
    }

    /* Import buffer into canvas */
    b = cucul_load_memory(data, i);
    font->image = cucul_import_canvas(b, "utf8");
    cucul_free_buffer(b);
    free(data);

    if(!font->image)
    {
        free(font->lookup);
        free(font);
        fprintf(stderr, "libcucul could not load data in `%s'\n", path);
        return NULL;
    }

    /* Remove EOL characters. For now we ignore hardblanks, don’t do any
     * smushing, nor any kind of error checking. */
    for(j = 0; j < font->height * font->glyphs; j++)
    {
        unsigned long int ch, oldch = 0;

        for(i = font->max_length; i--;)
        {
            ch = cucul_getchar(font->image, i, j);

            if(ch == font->hardblank)
                cucul_putchar(font->image, i, j, ch = ' ');

            if(oldch && ch != oldch)
            {
                if(!font->lookup[j / font->height * 2 + 1])
                    font->lookup[j / font->height * 2 + 1] = i + 1;
            }
            else if(oldch && ch == oldch)
                cucul_putchar(font->image, i, j, ' ');
            else if(ch != ' ')
            {
                oldch = ch;
                cucul_putchar(font->image, i, j, ' ');
            }
        }
    }

    return font;
}

static void free_font(struct figfont *font)
{
    cucul_free_canvas(font->image);
    free(font->lookup);
    free(font);
}

