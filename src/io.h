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
 * This header defines functions for compressed file I/O.
 */
typedef struct toifile
{
    FILE *f;
}
TOIFILE;

TOIFILE *toiopen(const char *, const char *);
int toiclose(TOIFILE *);
int toieof(TOIFILE *);
char *toigets(char *, int, TOIFILE *);

