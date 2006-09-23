
bin_PROGRAMS = toilet

toilet_SOURCES = main.c render.c render.h filters.c filter.h
toilet_CFLAGS = `pkg-config --cflags cucul`
toilet_LDFLAGS = `pkg-config --libs cucul` @GETOPT_LIBS@

