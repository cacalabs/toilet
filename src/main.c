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
 * This is the main program entry point.
 */

#include "config.h"

#if defined HAVE_INTTYPES_H
#   include <inttypes.h>
#endif
#if defined HAVE_SYS_IOCTL_H && defined HAVE_TIOCGWINSZ
#   include <sys/ioctl.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <caca.h>

#include "toilet.h"
#include "render.h"
#include "filter.h"
#include "export.h"

static void version(void);
static void usage(void);

int main(int argc, char *argv[])
{
    context_t struct_cx;
    context_t *cx = &struct_cx;

    int infocode = -1;

    cx->export = "utf8";
    cx->font = "ascii9";
    cx->dir = FONTDIR;

    cx->term_width = 80;

    cx->hmode = "default";

    cx->filters = NULL;
    cx->nfilters = 0;

    for(;;)
    {
#define MOREINFO "Try `%s --help' for more information.\n"
        int option_index = 0;
        static struct caca_option long_options[] =
        {
            /* Long option, needs arg, flag, short option */
            { "font", 1, NULL, 'f' },
            { "directory", 1, NULL, 'd' },
            { "width", 1, NULL, 'w' },
            { "termwidth", 0, NULL, 't' },
            { "filter", 1, NULL, 'F' },
            { "gay", 0, NULL, 130 },
            { "metal", 0, NULL, 131 },
            { "export", 1, NULL, 'E' },
            { "irc", 0, NULL, 140 },
            { "html", 0, NULL, 141 },
            { "help", 0, NULL, 'h' },
            { "infocode", 1, NULL, 'I' },
            { "version", 0, NULL, 'v' },
            { NULL, 0, NULL, 0 }
        };

        int c = caca_getopt(argc, argv, "f:d:w:tsSkWoF:E:hI:v",
                            long_options, &option_index);
        if(c == -1)
            break;

        switch(c)
        {
        case 'h': /* --help */
            usage();
            return 0;
        case 'I': /* --infocode */
            infocode = atoi(caca_optarg);
            break;
        case 'v': /* --version */
            version();
            return 0;
        case 'f': /* --font */
            cx->font = caca_optarg;
            break;
        case 'd': /* --directory */
            cx->dir = caca_optarg;
            break;
        case 'F': /* --filter */
            if(!strcmp(caca_optarg, "list"))
                return filter_list();
            if(filter_add(cx, caca_optarg) < 0)
                return -1;
            break;
        case 130: /* --gay */
            filter_add(cx, "gay");
            break;
        case 131: /* --metal */
            filter_add(cx, "metal");
            break;
        case 'w': /* --width */
            cx->term_width = atoi(caca_optarg);
            break;
        case 't': /* --termwidth */
        {
#if defined HAVE_SYS_IOCTL_H && defined HAVE_TIOCGWINSZ
            struct winsize ws;

            if((ioctl(1, TIOCGWINSZ, &ws) != -1 ||
                ioctl(2, TIOCGWINSZ, &ws) != -1 ||
                ioctl(0, TIOCGWINSZ, &ws) != -1) && ws.ws_col != 0)
                cx->term_width = ws.ws_col;
#endif
            break;
        }
        case 's':
            cx->hmode = "default";
            break;
        case 'S':
            cx->hmode = "smush";
            break;
        case 'k':
            cx->hmode = "kern";
            break;
        case 'W':
            cx->hmode = "none";
            break;
        case 'o':
            cx->hmode = "overlap";
            break;
        case 'E': /* --export */
            if(!strcmp(caca_optarg, "list"))
                return export_list();
            if(export_set(cx, caca_optarg) < 0)
                return -1;
            break;
        case 140: /* --irc */
            export_set(cx, "irc");
            break;
        case 141: /* --html */
            export_set(cx, "html");
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

    switch(infocode)
    {
        case -1:
            break;
        case 0:
            version();
            return 0;
        case 1:
            printf("20201\n");
            return 0;
        case 2:
            printf("%s\n", cx->dir);
            return 0;
        case 3:
            printf("%s\n", cx->font);
            return 0;
        case 4:
            printf("%u\n", cx->term_width);
            return 0;
        default:
            return 0;
    }

    if(render_init(cx) < 0)
        return -1;

    if(caca_optind >= argc)
        render_stdin(cx);
    else
        render_list(cx, argc - caca_optind, argv + caca_optind);

    render_end(cx);
    filter_end(cx);

    return 0;
}

#define USAGE \
    "Usage: toilet [ -hkostvSW ] [ -d fontdirectory ]\n" \
    "              [ -f fontfile ] [ -F filter ] [ -w outputwidth ]\n" \
    "              [ -I infocode ] [ -E format ] [ message ]\n"

#define HELP \
    "  -f, --font <name>        select the font\n" \
    "  -d, --directory <dir>    specify font directory\n" \
    "  -s, -S, -k, -W, -o       render mode (default, force smushing,\n" \
    "                           kerning, full width, overlap)\n" \
    "  -w, --width <width>      set output width\n" \
    "  -t, --termwidth          adapt to terminal's width\n" \
    "  -F, --filter <filters>   apply one or several filters to the text\n" \
    "  -F, --filter list        list available filters\n" \
    "      --gay                rainbow filter (same as -F gay)\n" \
    "      --metal              metal filter (same as -F metal)\n" \
    "  -E, --export <format>    select export format\n" \
    "  -E, --export list        list available export formats\n" \
    "      --irc                output IRC colour codes (same as -E irc)\n" \
    "      --html               output an HTML document (same as -E html)\n" \
    "  -h, --help               display this help and exit\n" \
    "  -I, --infocode <code>    print FIGlet-compatible infocode\n" \
    "  -v, --version            output version information and exit\n"

static void version(void)
{
    printf(
    "TOIlet Copyright 2006 Sam Hocevar\n"
    "Internet: <sam@hocevar.net> Version: %s, date: %s\n"
    "\n"
    "TOIlet, along with the various TOIlet fonts and documentation, may be\n"
    "freely copied and distributed.\n"
    "\n"
    "If you use TOIlet, please send an e-mail message to <sam@hocevar.net>.\n"
    "\n"
    "The latest version of TOIlet is available from the web site,\n"
    "        http://libcaca.zoy.org/toilet.html\n"
    "\n"
    "%s", VERSION, DATE, USAGE);
}

static void usage(void)
{
    printf("%s%s", HELP, USAGE);
}

