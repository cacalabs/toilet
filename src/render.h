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
 * This header defines text to canvas rendering functions.
 */

extern int init_tiny(context_t *);
extern int init_figlet(context_t *);

extern int render_init(context_t *);
extern int render_stdin(context_t *);
extern int render_list(context_t *, int, char *[]);
extern int render_end(context_t *);

