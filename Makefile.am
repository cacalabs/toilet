
SUBDIRS = src tools fonts doc
DIST_SUBDIRS = $(SUBDIRS)

EXTRA_DIST = bootstrap
AUTOMAKE_OPTIONS = foreign dist-bzip2

fonts: tools/caca2tlf FORCE
	$(top_builddir)/tools/caca2tlf "Monospace 9" | zip \
	    > $(top_builddir)/fonts/bigmono9.tlf
	$(top_builddir)/tools/caca2tlf "Monospace Bold 12" | zip \
	    > $(top_builddir)/fonts/bigmono12.tlf
	$(top_builddir)/tools/caca2tlf --half "Monospace 9" | zip \
	    > $(top_builddir)/fonts/mono9.tlf
	$(top_builddir)/tools/caca2tlf --half "Monospace Bold 12" | zip \
	    > $(top_builddir)/fonts/mono12.tlf
	$(top_builddir)/tools/caca2tlf --quarter "Monospace 9" | zip \
	    > $(top_builddir)/fonts/smmono9.tlf
	$(top_builddir)/tools/caca2tlf --quarter "Monospace Bold 12" | zip \
	    > $(top_builddir)/fonts/smmono12.tlf

FORCE:
