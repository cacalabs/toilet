
bin_PROGRAMS = toilet

toilet_SOURCES = main.c toilet.h \
	io.c io.h \
	render.c render.h \
	filter.c filter.h \
	term.c mono9.c figlet.c
toilet_CFLAGS = `pkg-config --cflags cucul`
toilet_LDFLAGS = `pkg-config --libs cucul` @GETOPT_LIBS@ @ZLIB_LIBS@

