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
 * This file contains functions for compressed file I/O.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include "io.h"

TOIFILE *toiopen(const char *path, const char *mode)
{
    TOIFILE *toif;
    FILE *f = fopen(path, mode);

    if(!f)
        return NULL;

    toif = malloc(sizeof(*toif));
    toif->f = f;

    return toif;
}

int toiclose(TOIFILE *toif)
{
    FILE *f = toif->f;
    free(toif);
    return fclose(f);
}

int toieof(TOIFILE *toif)
{
    return feof(toif->f);
}

char *toigets(char *s, int size, TOIFILE *toif)
{
    return fgets(s, size, toif->f);
}

