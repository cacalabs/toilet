
EXTRA_DIST = toilet.1.in
CLEANFILES = toilet.1

man_MANS = toilet.1

toilet.1: toilet.1.in
	sed -e "s,[@]datarootdir@,$(datarootdir),g" $(srcdir)/$^ > $@

