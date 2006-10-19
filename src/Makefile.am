
bin_PROGRAMS = toilet

toilet_SOURCES = main.c \
	io.c io.h \
	render.c render.h \
	filters.c filter.h \
	figlet.c figlet.h
toilet_CFLAGS = `pkg-config --cflags cucul`
toilet_LDFLAGS = `pkg-config --libs cucul` @GETOPT_LIBS@ @ZLIB_LIBS@

