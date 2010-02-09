
SUBDIRS = src tools fonts doc test
DIST_SUBDIRS = $(SUBDIRS)

EXTRA_DIST = bootstrap
AUTOMAKE_OPTIONS = dist-bzip2

CACA2TLF = $(top_builddir)/tools/caca2tlf
FONTDIR = $(top_builddir)/fonts

fonts: tools/caca2tlf FORCE
	$(CACA2TLF) -u "Monospace 9" | zip > $(FONTDIR)/bigmono9.tlf
	$(CACA2TLF) -u "Monospace Bold 12" | zip > $(FONTDIR)/bigmono12.tlf
	$(CACA2TLF) -h -u "Monospace 9" | zip > $(FONTDIR)/mono9.tlf
	$(CACA2TLF) -h -u "Monospace Bold 12" | zip > $(FONTDIR)/mono12.tlf
	$(CACA2TLF) -q -u "Monospace 9" | zip > $(FONTDIR)/smmono9.tlf
	$(CACA2TLF) -q -u "Monospace Bold 12" | zip > $(FONTDIR)/smmono12.tlf
	$(CACA2TLF) -a "Monospace 9" | zip > $(FONTDIR)/bigascii9.tlf
	$(CACA2TLF) -a "Monospace Bold 12" | zip > $(FONTDIR)/bigascii12.tlf
	$(CACA2TLF) -h -a "Monospace 9" | zip > $(FONTDIR)/ascii9.tlf
	$(CACA2TLF) -h -a "Monospace Bold 12" | zip > $(FONTDIR)/ascii12.tlf
	$(CACA2TLF) -q -a "Monospace 9" | zip > $(FONTDIR)/smascii9.tlf
	$(CACA2TLF) -q -a "Monospace Bold 12" | zip > $(FONTDIR)/smascii12.tlf
	$(CACA2TLF) -s "Monospace 9" | zip > $(FONTDIR)/biggray9.tlf
	$(CACA2TLF) -s "Monospace Bold 12" | zip > $(FONTDIR)/biggray12.tlf

webhost = poulet.zoy.org
webdir = /srv/libcaca.zoy.org/var/www/

upload: FORCE
	scp $(distdir).tar.gz $(webhost):$(webdir)/files/

update-changelog: FORCE
	git --help >/dev/null 2>&1 \
	  && test -d .git \
	  && git log --stat | awk 'function flush() { if (m != "") { print "Commit: " v "\nAuthor: " a m } m=""; v="UNSUBMITTED" } { if ($$1 == "commit") flush(); else if ($$1 == "Author:") { a = $$2 } else if ($$1 == "git-svn-id:") { split($$2,tmp,"@"); v=tmp[2] } else if ($$_ != "    ") { m=m "\n" $$_ } } END { flush() }' > ChangeLog

echo-dirs: ; echo src tools

FORCE:
