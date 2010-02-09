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
 * This file contains export functions.
 */

#include "config.h"

#if defined(HAVE_INTTYPES_H)
#   include <inttypes.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <caca.h>

#include "toilet.h"
#include "export.h"

int export_list(void)
{
    char const * const * exports, * const * p;

    printf("Available export formats:\n");

    exports = caca_get_export_list();
    for(p = exports; *p; p += 2)
        printf("\"%s\": %s\n", *p, *(p + 1));

    return 0;
}

int export_set(context_t *cx, char const *format)
{
    char const * const * exports, * const * p;

    cx->export = format;

    exports = caca_get_export_list();
    for(p = exports; *p; p += 2)
        if(!strcmp(*p, format))
            return 0;

    fprintf(stderr, "unknown export format `%s'\n", format);
    return -1;
}

