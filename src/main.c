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

#include "config.h"

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif
#if defined(HAVE_GETOPT_H)
#   include <getopt.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cucul.h>

#include "render.h"
#include "figlet.h"
#include "filters.h"

int main(int argc, char *argv[])
{
    cucul_canvas_t *cv;
    cucul_buffer_t *buffer;

    uint32_t *string = NULL;
    unsigned int length;

    int i;

    char const *export = "utf8";
    char const *font = "mono9";
    unsigned flag_gay = 0;
    unsigned flag_metal = 0;

#if defined(HAVE_GETOPT_H)
    for(;;)
    {
#   ifdef HAVE_GETOPT_LONG
#       define MOREINFO "Try `%s --help' for more information.\n"
        int option_index = 0;
        static struct option long_options[] =
        {
            /* Long option, needs arg, flag, short option */
            { "font", 1, NULL, 'f' },
            { "gay", 0, NULL, 'g' },
            { "metal", 0, NULL, 'm' },
            { "irc", 0, NULL, 'i' },
            { "help", 0, NULL, 'h' },
            { "version", 0, NULL, 'v' },
            { NULL, 0, NULL, 0 }
        };

        int c = getopt_long(argc, argv, "f:gmihv", long_options, &option_index);
#   else
#       define MOREINFO "Try `%s -h' for more information.\n"
        int c = getopt(argc, argv, "f:gmihv");
#   endif
        if(c == -1)
            break;

        switch(c)
        {
        case 'h': /* --help */
            printf("Usage: %s [ -f:gmihv ] [ message ]\n", argv[0]);
#   ifdef HAVE_GETOPT_LONG
            printf("  -f, --font <fontfile>\n");
            printf("                   select the font\n");
            printf("  -g, --gay        add a rainbow effect to the text\n");
            printf("  -m, --metal      add a metal effect to the text\n");
            printf("  -i, --irc        output IRC colour codes\n");
            printf("  -h, --help       display this help and exit\n");
            printf("  -v, --version    output version information and exit\n");
#   else
            printf("  -f <fontfile>\n");
            printf("        select the font\n");
            printf("  -g    add a rainbow effect to the text\n");
            printf("  -m    add a metal effect to the text\n");
            printf("  -i    output IRC colour codes\n");
            printf("  -h    display this help and exit\n");
            printf("  -v    output version information and exit\n");
#   endif
            return 0;
        case 'v': /* --version */
            printf("TOIlet Copyright 2006 Sam Hocevar %s\n", VERSION);
            printf("Internet: <sam@zoy.org> Version: 0, date: 21 Sep 2006\n");
            printf("\n");
            return 0;
        case 'f': /* --font */
            font = optarg;
            break;
        case 'g': /* --gay */
            flag_gay = 1;
            break;
        case 'm': /* --metal */
            flag_metal = 1;
            break;
        case 'i': /* --irc */
            export = "irc";
            break;
        case '?':
            printf(MOREINFO, argv[0]);
            return 1;
        default:
            printf("%s: invalid option -- %i\n", argv[0], c);
            printf(MOREINFO, argv[0]);
            return 1;
        }
    }
#else
#   define MOREINFO "Usage: %s message...\n"
    int optind = 1;
#endif

    if(optind >= argc)
    {
        printf("%s: too few arguments\n", argv[0]);
        printf(MOREINFO, argv[0]);
        return 1;
    }

    /* Load rest of commandline into a UTF-32 string */
    for(i = optind, length = 0; i < argc; i++)
    {
        unsigned int k, guessed_len, real_len;

        guessed_len = strlen(argv[i]);

        if(i > optind)
            string[length++] = (uint32_t)' ';

        string = realloc(string, (length + guessed_len + 1) * 4);

        for(k = 0, real_len = 0; k < guessed_len; real_len++)
        {
            unsigned int char_len;

            string[length + real_len] =
                cucul_utf8_to_utf32(argv[i] + k, &char_len);

            k += char_len;
        }

        length += real_len;
    }

    /* Render string to canvas */
    if(!strcasecmp(font, "mono9"))
        cv = render_big(string, length);
    else if(!strcasecmp(font, "term"))
        cv = render_tiny(string, length);
    else
        cv = render_figlet(string, length, font);

    /* Crop output */
    filter_autocrop(cv);

    /* Do gay stuff with our string (léopard) */
    if(flag_metal)
        filter_metal(cv);
    if(flag_gay)
        filter_gay(cv);

    /* Output char */
    buffer = cucul_export_canvas(cv, export);
    fwrite(cucul_get_buffer_data(buffer),
           cucul_get_buffer_size(buffer), 1, stdout);
    cucul_free_buffer(buffer);

    cucul_free_canvas(cv);

    return 0;
}

