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
#include <stdint.h>
#include <string.h>

#if defined HAVE_ZLIB_H
#   include <zlib.h>
#   define READSIZE  128 /* Read buffer size */
#   define WRITESIZE 128 /* Inflate buffer size */
#endif

#include "io.h"

#if defined HAVE_ZLIB_H
static int zipread(TOIFILE *, void *, unsigned int);
#endif

struct toifile
{
#if defined HAVE_ZLIB_H
    unsigned char read_buffer[READSIZE];
    z_stream stream;
    gzFile gz;
    int eof, zip;
#endif
    FILE *f;
};

TOIFILE *toiopen(char const *path, const char *mode)
{
    TOIFILE *toif = malloc(sizeof(*toif));

#if defined HAVE_ZLIB_H
    uint8_t buf[4];
    unsigned int skip_size = 0;

    toif->gz = gzopen(path, "rb");
    if(!toif->gz)
    {
        free(toif);
        return NULL;
    }

    toif->eof = 0;
    toif->zip = 0;

    /* Parse ZIP file and go to start of first file */
    gzread(toif->gz, buf, 4);
    if(memcmp(buf, "PK\3\4", 4))
    {
        gzseek(toif->gz, 0, SEEK_SET);
        return toif;
    }

    toif->zip = 1;

    gzseek(toif->gz, 22, SEEK_CUR);

    gzread(toif->gz, buf, 2); /* Filename size */
    skip_size += (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
    gzread(toif->gz, buf, 2); /* Extra field size */
    skip_size += (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);

    gzseek(toif->gz, skip_size, SEEK_CUR);

    /* Initialise inflate stream */
    toif->stream.total_out = 0;
    toif->stream.zalloc = NULL;
    toif->stream.zfree = NULL;
    toif->stream.opaque = NULL;
    toif->stream.next_in = NULL;
    toif->stream.avail_in = 0;

    if(inflateInit2(&toif->stream, -MAX_WBITS))
    {
        free(toif);
        gzclose(toif->gz);
        return NULL;
    }
#else
    toif->f = fopen(path, mode);

    if(!f)
    {
        free(toif);
        return NULL;
    }
#endif

    return toif;
}

int toiclose(TOIFILE *toif)
{
#if defined HAVE_ZLIB_H
    gzFile gz = toif->gz;
    if(toif->zip)
        inflateEnd(&toif->stream);
    free(toif);
    return gzclose(gz);
#else
    FILE *f = toif->f;
    free(toif);
    return fclose(f);
#endif
}

int toieof(TOIFILE *toif)
{
#if defined HAVE_ZLIB_H
    return toif->zip ? toif->eof : gzeof(toif->gz);
#else
    return feof(toif->f);
#endif
}

char *toigets(char *s, int size, TOIFILE *toif)
{
#if defined HAVE_ZLIB_H
    if(toif->zip)
    {
        int i;

        for(i = 0; i < size; i++)
        {
            int ret = zipread(toif, s + i, 1);

            if(ret < 0)
                return NULL;

            if(ret == 0 || s[i] == '\n')
            {
                if(i + 1 < size)
                    s[i + 1] = '\0';
                return s;
            }
        }

        return s;
    }

    return gzgets(toif->gz, s, size);
#else
    return fgets(s, size, toif->f);
#endif
}

#if defined HAVE_ZLIB_H
static int zipread(TOIFILE *toif, void *buf, unsigned int len)
{
    unsigned int total_read = 0;

    if(len == 0)
        return 0;

    toif->stream.next_out = buf;
    toif->stream.avail_out = len;

    while(toif->stream.avail_out > 0)
    {
        unsigned int tmp;
        int ret = 0;

        if(toif->stream.avail_in == 0 && !gzeof(toif->gz))
        {
            int bytes_read;

            bytes_read = gzread(toif->gz, toif->read_buffer, READSIZE);
            if(bytes_read < 0)
                return -1;

            toif->stream.next_in = toif->read_buffer;
            toif->stream.avail_in = bytes_read;
        }

        tmp = toif->stream.total_out;
        ret = inflate(&toif->stream, Z_SYNC_FLUSH);
        total_read += toif->stream.total_out - tmp;

        if(ret == Z_STREAM_END)
        {
            toif->eof = 1;
            return total_read;
        }

        if(ret != Z_OK)
            return ret;
    }

    return total_read;
}
#endif

