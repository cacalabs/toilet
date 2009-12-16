/*
 *  caca2tlf      Create a TOIlet font from a libcaca font
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
 * This is the main program entry point.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <caca.h>

enum mode { GRAY, HALFBLOCKS, QUARTERBLOCKS } mode;

static void list_fonts(void);
static void add_char(unsigned long int);

caca_font_t *f;
caca_canvas_t *out, *onechar;
uint32_t const *blocks;
uint8_t * image;
unsigned int w, h, gw, fgw, gh, iw, ih;

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

    f = caca_load_font(fontname, 0);

    if(!f)
    {
        fprintf(stderr, "Font \"%s\" not found.\n", argv[1]);
        list_fonts();
        return -2;
    }

    w = caca_get_font_width(f);
    h = caca_get_font_height(f);
    iw = w * 2 + 1;
    ih = h + 1;
    switch(mode)
    {
    case GRAY:
        gw = w;
        fgw = w * 2;
        gh = h;
        break;
    case HALFBLOCKS:
        gw = w;
        fgw = w * 2;
        gh = (h + 1) / 2;
        break;
    case QUARTERBLOCKS:
        gw = (w + 1) / 2;
        fgw = (w * 2 + 1) / 2;
        gh = (h + 1) / 2;
        break;
    }

    blocks = caca_get_font_blocks(f);
    onechar = caca_create_canvas(0, 0);
    caca_set_color_ansi(onechar, CACA_WHITE, CACA_BLACK);
    image = malloc(4 * iw * ih);

    out = caca_create_canvas(0, 0);
    printf("tlf2a$ %u %u %u -1 4 0 0 0\n", gh, gh - 1, fgw + 2);

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

            len = caca_utf32_to_utf8(buf, ch);
            buf[len] = '\0';
            printf("0x%.04lX %s\n", ch, buf);
            add_char(ch);
        }
    }

    caca_free_canvas(out);
    caca_free_canvas(onechar);
    free(image);
    caca_free_font(f);

    return 0;
}

static void list_fonts(void)
{
    char const * const * fonts;
    unsigned int i;

    fprintf(stderr, "Available fonts:\n");

    fonts = caca_get_font_list();
    for(i = 0; fonts[i]; i++)
        fprintf(stderr, "  \"%s\"\n", fonts[i]);
}

static void add_char(unsigned long int ch)
{
    void *buf;
    size_t len;
    unsigned int x, y, myw, mygw;
    int full = caca_utf32_is_fullwidth(ch);

    caca_set_canvas_size(onechar, full ? 2 : 1, 1);
    caca_put_char(onechar, 0, 0, ch);
    caca_render_canvas(onechar, f, image, iw, ih, 4 * iw);

    myw = full ? 2 * w : w;
    mygw = full ? fgw : gw;

    caca_set_canvas_size(out, (full ? fgw : gw) + 2, gh);

    switch(mode)
    {
    case GRAY:
        for(y = 0; y < h; y++)
            for(x = 0; x < myw; x++)
        {
            uint8_t c = image[4 * (x + y * iw) + 2];

            if(c >= 0xa0)
                caca_put_str(out, x, y, "█");
            else if(c >= 0x80)
                caca_put_str(out, x, y, "▓");
            else if(c >= 0x40)
                caca_put_str(out, x, y, "▒");
            else if(c >= 0x20)
                caca_put_str(out, x, y, "░");
            else
                caca_put_char(out, x, y, ' ');
        }
        break;
    case HALFBLOCKS:
        for(y = 0; y < gh; y++)
            for(x = 0; x < mygw; x++)
        {
            static char const *str[] = { " ", "▀", "▄", "█" };

            uint8_t p1 = image[4 * (x + y * 2 * iw) + 2];
            uint8_t p2 = image[4 * (x + (y * 2 + 1) * iw) + 2];

            caca_put_str(out, x, y, str[(p1 > 0x80) + 2 * (p2 > 0x80)]);
        }
        break;
    case QUARTERBLOCKS:
        for(y = 0; y < gh; y++)
            for(x = 0; x < mygw; x++)
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

            caca_put_str(out, x, y, str[(p1 > 0x80) + 2 * (p2 > 0x80) +
                                         4 * (p3 > 0x80) + 8 * (p4 > 0x80)]);
        }
        break;
    }

    if(ch == ' ' || ch == 0xa0)
    {
        caca_draw_line(out, mygw - 1, 0, mygw - 1, gh - 1, '$');
        caca_draw_line(out, mygw / 2, 0, mygw / 2, gh - 1, '$');
    }

    caca_draw_line(out, mygw, 0, mygw, gh - 1, '@');
    caca_put_char(out, mygw + 1, gh - 1, '@');

    buf = caca_export_memory(out, "utf8", &len);
    fwrite(buf, len, 1, stdout);
    free(buf);
}

