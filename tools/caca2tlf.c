/*
 *  caca2tlf      Create a TOIlet font from a libcaca font
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
 * This is the main program entry point.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cucul.h>

enum mode { GRAY, HALFBLOCKS, QUARTERBLOCKS } mode;

static void list_fonts(void);
static void add_char(unsigned int);

cucul_font_t *f;
cucul_canvas_t *out, *onechar;
unsigned long int const *blocks;
uint8_t * image;
unsigned int w, h, gw, gh, iw, ih;

int main(int argc, char *argv[])
{
    char *fontname, *extraflag;
    unsigned int b, i;

    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s [--half|--quarter] <font>\n", argv[0]);
        list_fonts();
        return -1;
    }

    if(!strcmp(argv[1], "--half") && argc >= 3)
    {
        extraflag = "--half ";
        mode = HALFBLOCKS;
        fontname = argv[2];
    }
    else if(!strcmp(argv[1], "--quarter") && argc >= 3)
    {
        extraflag = "--quarter ";
        mode = QUARTERBLOCKS;
        fontname = argv[2];
    }
    else
    {
        extraflag = "";
        mode = GRAY;
        fontname = argv[1];
    }

    f = cucul_load_font(fontname, 0);

    if(!f)
    {
        fprintf(stderr, "Font \"%s\" not found.\n", argv[1]);
        list_fonts();
        return -2;
    }

    w = cucul_get_font_width(f);
    h = cucul_get_font_height(f);
    iw = w + 1;
    ih = h + 1;
    switch(mode)
    {
    case GRAY:
        gw = w;
        gh = h;
        break;
    case HALFBLOCKS:
        gw = w;
        gh = (h + 1) / 2;
        break;
    case QUARTERBLOCKS:
        gw = (w + 1) / 2;
        gh = (h + 1) / 2;
        break;
    }

    blocks = cucul_get_font_blocks(f);
    onechar = cucul_create_canvas(1, 1); /* FIXME: support double width */
    cucul_set_color_ansi(onechar, CUCUL_WHITE, CUCUL_BLACK);
    image = malloc(4 * iw * ih);

    out = cucul_create_canvas(gw + 2, gh);
    printf("tlf2a$ %u %u %u 0 4 0 0 0\n", gh, gh - 1, gw + 2);

    printf("=============================================="
                                       "==================================\n");
    printf("  This font was automatically generated using:\n");
    printf("   %% caca2tlf %s\"%s\"\n", extraflag, fontname);
    printf("=============================================="
                                       "==================================\n");

    for(i = 32; i < 127; i++)
        add_char(i);

    add_char(196);
    add_char(214);
    add_char(220);
    add_char(228);
    add_char(246);
    add_char(252);
    add_char(223);

    for(b = 0, i = 0; blocks[i + 1]; i += 2)
    {
        int j, n = (int)(blocks[i + 1] - blocks[i]);

        for(j = 0; j < n; j++)
        {
            char buf[7];
            unsigned int len;
            unsigned long int ch = blocks[i] + j;

            if(ch <= 127 || ch == 196 || ch == 214 || ch == 220
               || ch == 228 || ch == 246 || ch == 252 || ch == 223)
                continue;

            len = cucul_utf32_to_utf8(buf, ch);
            buf[len] = '\0';
            printf("0x%.04lX %s\n", ch, buf);
            add_char(ch);
        }
    }

    cucul_free_canvas(out);
    cucul_free_canvas(onechar);
    free(image);
    cucul_free_font(f);

    return 0;
}

static void list_fonts(void)
{
    char const * const * fonts;
    unsigned int i;

    fprintf(stderr, "Available fonts:\n");

    fonts = cucul_get_font_list();
    for(i = 0; fonts[i]; i++)
        fprintf(stderr, "  \"%s\"\n", fonts[i]);
}

static void add_char(unsigned int ch)
{
    cucul_buffer_t *buf;
    unsigned int x, y;

    cucul_putchar(onechar, 0, 0, ch);
    cucul_render_canvas(onechar, f, image, iw, ih, 4 * iw);

    switch(mode)
    {
    case GRAY:
        for(y = 0; y < h; y++)
           for(x = 0; x < w; x++)
        {
            uint8_t c = image[4 * (x + y * iw) + 2];

            if(c >= 0xa0)
                cucul_putstr(out, x, y, "█");
            else if(c >= 0x80)
                cucul_putstr(out, x, y, "▓");
            else if(c >= 0x40)
                cucul_putstr(out, x, y, "▒");
            else if(c >= 0x20)
                cucul_putstr(out, x, y, "░");
            else
                cucul_putchar(out, x, y, ' ');
        }
        break;
    case HALFBLOCKS:
        for(y = 0; y < gh; y++)
           for(x = 0; x < gw; x++)
        {
            static char const *str[] = { " ", "▀", "▄", "█" };

            uint8_t p1 = image[4 * (x + y * 2 * iw) + 2];
            uint8_t p2 = image[4 * (x + (y * 2 + 1) * iw) + 2];

            cucul_putstr(out, x, y,
                         str[(p1 < 0x80 ? 0 : 1) + (p2 < 0x80 ? 0 : 2)]);
        }
        break;
    case QUARTERBLOCKS:
        for(y = 0; y < gh; y++)
           for(x = 0; x < gw; x++)
        {
            static char const *str[] =
            {
                " ", "▘", "▝", "▀", "▖", "▌", "▞", "▛",
                "▗", "▚", "▐", "▜", "▄", "▙", "▟", "█"
            };

            uint8_t p1 = image[4 * (x * 2 + y * 2 * iw) + 2];
            uint8_t p2 = image[4 * (x * 2 + 1 + y * 2 * iw) + 2];
            uint8_t p3 = image[4 * (x * 2 + (y * 2 + 1) * iw) + 2];
            uint8_t p4 = image[4 * (x * 2 + 1 + (y * 2 + 1) * iw) + 2];

            cucul_putstr(out, x, y,
                         str[(p1 < 0x80 ? 0 : 1) + (p2 < 0x80 ? 0 : 2) +
                             (p3 < 0x80 ? 0 : 4) + (p4 < 0x80 ? 0 : 8)]);
        }
        break;
    }

    cucul_draw_line(out, gw, 0, gw, gh - 1, "@");
    cucul_putchar(out, gw + 1, gh - 1, '@');

    buf = cucul_export_canvas(out, "utf8");
    fwrite(cucul_get_buffer_data(buf), cucul_get_buffer_size(buf), 1, stdout);
    cucul_free_buffer(buf);
}

