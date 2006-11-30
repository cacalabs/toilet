
bin_PROGRAMS = toilet

toilet_SOURCES = main.c toilet.h \
	io.c io.h \
	render.c render.h \
	filter.c filter.h \
	export.c export.h \
	term.c figlet.c
toilet_CPPFLAGS = -DFONTDIR=\"$(datadir)/figlet\"
toilet_CFLAGS = @CUCUL_CFLAGS@
toilet_LDADD = @CUCUL_LIBS@ @GETOPT_LIBS@ @ZLIB_LIBS@

