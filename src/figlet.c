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
#include <cucul.h>

#include "figlet.h"

cucul_canvas_t *render_figlet(uint32_t const *string, unsigned int length,
                              char const *fontname)
{
    cucul_canvas_t *cv;



    cv = cucul_create_canvas(3, 1);
    cucul_putstr(cv, 0, 0, "LOL");
    return cv;
}

