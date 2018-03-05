MAKEFLAGS=-s --no-print-directory
PACKAGE = amule
VERSION = 2.2.6
SHELL = /bin/sh
srcdir = .
top_srcdir = ..
top_builddir = ..
prefix = /usr/local
exec_prefix = ${prefix}
transform = s,x,x,
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include
datarootdir = @datarootdir@
datadir = ${prefix}/share
localedir = $(datadir)/locale
gettextsrcdir = $(datadir)/gettext/intl
aliaspath = $(localedir)
subdir = intl
INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
MKINSTALLDIRS = ./mkinstalldirs
mkinstalldirs = $(SHELL) `case "$(MKINSTALLDIRS)" in /*) echo "$(MKINSTALLDIRS)" ;; *) echo "$(top_builddir)/$(MKINSTALLDIRS)" ;; esac`
l = 
AR = ar
CC = arm-mv5sft-linux-gnueabi-gcc
LIBTOOL = @LIBTOOL@
RANLIB = arm-mv5sft-linux-gnueabi-ranlib
YACC = : -y -d
YFLAGS = --name-prefix=__gettext
DEFS = -DLOCALEDIR=\"$(localedir)\" -DLOCALE_ALIAS_PATH=\"$(aliaspath)\" \
-DLIBDIR=\"$(libdir)\" -DIN_LIBINTL -DHAVE_CONFIG_H
CPPFLAGS = -I/home/patrick/nas/DNS-345_GPL/aMule-2.2.6/../include
CFLAGS = 
LDFLAGS = -L/home/patrick/nas/DNS-345_GPL/aMule-2.2.6/../lib
COMPILE = $(CC) -c $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS)
HEADERS = $(COMHDRS) libgnuintl.h loadinfo.h
COMHDRS = gmo.h gettextP.h hash-string.h plural-exp.h eval-plural.h os2compat.h
SOURCES = $(COMSRCS) intl-compat.c
COMSRCS = bindtextdom.c dcgettext.c dgettext.c gettext.c \
finddomain.c loadmsgcat.c localealias.c textdomain.c l10nflist.c \
explodename.c dcigettext.c dcngettext.c dngettext.c ngettext.c plural.y \
plural-exp.c localcharset.c localename.c osdep.c os2compat.c
OBJECTS =  bindtextdom.$lo dcgettext.$lo dgettext.$lo gettext.$lo \
finddomain.$lo loadmsgcat.$lo localealias.$lo textdomain.$lo l10nflist.$lo \
explodename.$lo dcigettext.$lo dcngettext.$lo dngettext.$lo ngettext.$lo \
plural.$lo plural-exp.$lo localcharset.$lo localename.$lo osdep.$lo
GETTOBJS = intl-compat.$lo
DISTFILES.common = Makefile.in \
config.charset locale.alias ref-add.sin ref-del.sin $(HEADERS) $(SOURCES)
DISTFILES.generated = plural.c
DISTFILES.normal = VERSION
DISTFILES.gettext = COPYING.LIB-2.0 COPYING.LIB-2.1 libintl.glibc
DISTFILES.obsolete = xopen-msg.sed linux-msg.sed po2tbl.sed.in cat-compat.c \
COPYING.LIB-2 gettext.h libgettext.h plural-eval.c
LTV_CURRENT=4
LTV_REVISION=0
LTV_AGE=2
.SUFFIXES:
.SUFFIXES: .c .y .o .lo .sin .sed
.c.o:
	echo "Compiling $(<F)"
	$(COMPILE) $<
.c.lo:
	$(LIBTOOL) --mode=compile $(COMPILE) $<
.y.c:
	$(YACC) $(YFLAGS) --output $@ $<
	rm -f $*.h
.sin.sed:
	sed -e '/^#/d' -e 's/@''PACKAGE''@/amule/g' $< > t-$@
	mv t-$@ $@
INCLUDES = -I.. -I. -I$(top_srcdir)/intl
all: all-no
all-yes: libintl.$la libintl.h charset.alias ref-add.sed ref-del.sed
all-no: all-no-no
all-no-yes: libgnuintl.$la
all-no-no:
libintl.a libgnuintl.a: $(OBJECTS)
	echo "Building $(@F)"
	rm -f $@
	$(AR) cru $@ $(OBJECTS)
	$(RANLIB) $@
libintl.la libgnuintl.la: $(OBJECTS)
	$(LIBTOOL) --mode=link \
	  $(CC) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS) $(LDFLAGS) -o $@ \
	  $(OBJECTS) -L/home/patrick/nas/DNS-345_GPL/aMule-2.2.6/../lib -liconv -R/home/patrick/nas/DNS-345_GPL/aMule-2.2.6/../lib -lc \
	  -version-info $(LTV_CURRENT):$(LTV_REVISION):$(LTV_AGE) \
	  -rpath $(libdir) \
	  -no-undefined
libintl.h: libgnuintl.h
	cp $(srcdir)/libgnuintl.h libintl.h
charset.alias: config.charset
	$(SHELL) $(srcdir)/config.charset 'arm-mv5sft-linux-gnueabi' > t-$@
	mv t-$@ $@
check: all
install: install-exec install-data
install-exec: all
	if test "$(PACKAGE)" = "gettext" \
	   && test '' = '$(GETTOBJS)'; then \
	  $(mkinstalldirs) $(DESTDIR)$(libdir) $(DESTDIR)$(includedir)>/dev/null; \
	echo "Installing $(includedir)/libintl.h"; \
	  $(INSTALL_DATA) libintl.h $(DESTDIR)$(includedir)/libintl.h; \
	  $(LIBTOOL) --mode=install \
	echo "Installing $(libdir)/libintl.$la"; \
	    $(INSTALL_DATA) libintl.$la $(DESTDIR)$(libdir)/libintl.$la; \
	else \
	  : ; \
	fi
	if test 'no' = yes; then \
	  test yes != no || $(mkinstalldirs) $(DESTDIR)$(libdir)>/dev/null; \
	  temp=$(DESTDIR)$(libdir)/t-charset.alias; \
	  dest=$(DESTDIR)$(libdir)/charset.alias; \
	  if test -f $(DESTDIR)$(libdir)/charset.alias; then \
	    orig=$(DESTDIR)$(libdir)/charset.alias; \
	    sed -f ref-add.sed $$orig > $$temp; \
	    $(INSTALL_DATA) $$temp $$dest; \
	    $(INSTALL_DATA) $$temp $$dest; \
	    rm -f $$temp; \
	  else \
	    if test yes = no; then \
	      orig=charset.alias; \
	      sed -f ref-add.sed $$orig > $$temp; \
	      $(INSTALL_DATA) $$temp $$dest; \
	      $(INSTALL_DATA) $$temp $$dest; \
	      rm -f $$temp; \
	    fi; \
	  fi; \
	  $(mkinstalldirs) $(DESTDIR)$(localedir)>/dev/null; \
	  test -f $(DESTDIR)$(localedir)/locale.alias \
	    && orig=$(DESTDIR)$(localedir)/locale.alias \
	    || orig=$(srcdir)/locale.alias; \
	  temp=$(DESTDIR)$(localedir)/t-locale.alias; \
	  dest=$(DESTDIR)$(localedir)/locale.alias; \
	  sed -f ref-add.sed $$orig > $$temp; \
	  $(INSTALL_DATA) $$temp $$dest; \
	  $(INSTALL_DATA) $$temp $$dest; \
	  rm -f $$temp; \
	else \
	  : ; \
	fi
install-data: all
	if test "$(PACKAGE)" = "gettext"; then \
	  $(mkinstalldirs) $(DESTDIR)$(gettextsrcdir)>/dev/null; \
	echo "Installing $(gettextsrcdir)/VERSION"; \
	  $(INSTALL_DATA) VERSION $(DESTDIR)$(gettextsrcdir)/VERSION; \
	echo "Installing $(gettextsrcdir)/ChangeLog"; \
	  $(INSTALL_DATA) ChangeLog.inst $(DESTDIR)$(gettextsrcdir)/ChangeLog; \
	  dists="COPYING.LIB-2.0 COPYING.LIB-2.1 $(DISTFILES.common)"; \
	  for file in $$dists; do \
	    $(INSTALL_DATA) $(srcdir)/$$file \
	    $(INSTALL_DATA) $(srcdir)/$$file \
			    $(DESTDIR)$(gettextsrcdir)/$$file; \
	  done; \
	  chmod a+x $(DESTDIR)$(gettextsrcdir)/config.charset; \
	  dists="$(DISTFILES.generated)"; \
	  for file in $$dists; do \
	    if test -f $$file; then dir=.; else dir=$(srcdir); fi; \
	    $(INSTALL_DATA) $$dir/$$file \
	    $(INSTALL_DATA) $$dir/$$file \
			    $(DESTDIR)$(gettextsrcdir)/$$file; \
	  done; \
	  dists="$(DISTFILES.obsolete)"; \
	  for file in $$dists; do \
	    rm -f $(DESTDIR)$(gettextsrcdir)/$$file; \
	  done; \
	else \
	  : ; \
	fi
install-strip: install
installdirs:
	if test "$(PACKAGE)" = "gettext" \
	   && test '' = '$(GETTOBJS)'; then \
	  $(mkinstalldirs) $(DESTDIR)$(libdir) $(DESTDIR)$(includedir)>/dev/null; \
	else \
	  : ; \
	fi
	if test 'no' = yes; then \
	  test yes != no || $(mkinstalldirs) $(DESTDIR)$(libdir)>/dev/null; \
	  $(mkinstalldirs) $(DESTDIR)$(localedir)>/dev/null; \
	else \
	  : ; \
	fi
	if test "$(PACKAGE)" = "gettext"; then \
	  $(mkinstalldirs) $(DESTDIR)$(gettextsrcdir)>/dev/null; \
	else \
	  : ; \
	fi
installcheck:
uninstall:
	if test "$(PACKAGE)" = "gettext" \
	   && test '' = '$(GETTOBJS)'; then \
	echo "Uninstalling $(includedir)/libintl.h"; \
	  rm -f $(DESTDIR)$(includedir)/libintl.h; \
	  $(LIBTOOL) --mode=uninstall \
	echo "Uninstalling $(libdir)/libintl.$la"; \
	    rm -f $(DESTDIR)$(libdir)/libintl.$la; \
	else \
	  : ; \
	fi
	if test 'no' = yes; then \
	  if test -f $(DESTDIR)$(libdir)/charset.alias; then \
	    temp=$(DESTDIR)$(libdir)/t-charset.alias; \
	    dest=$(DESTDIR)$(libdir)/charset.alias; \
	    sed -f ref-del.sed $$dest > $$temp; \
	    if grep '^# Packages using this file: $$' $$temp > /dev/null; then \
	      rm -f $$dest; \
	    else \
	      $(INSTALL_DATA) $$temp $$dest; \
	      $(INSTALL_DATA) $$temp $$dest; \
	    fi; \
	    rm -f $$temp; \
	  fi; \
	  if test -f $(DESTDIR)$(localedir)/locale.alias; then \
	    temp=$(DESTDIR)$(localedir)/t-locale.alias; \
	    dest=$(DESTDIR)$(localedir)/locale.alias; \
	    sed -f ref-del.sed $$dest > $$temp; \
	    if grep '^# Packages using this file: $$' $$temp > /dev/null; then \
	      rm -f $$dest; \
	    else \
	      $(INSTALL_DATA) $$temp $$dest; \
	      $(INSTALL_DATA) $$temp $$dest; \
	    fi; \
	    rm -f $$temp; \
	  fi; \
	else \
	  : ; \
	fi
	if test "$(PACKAGE)" = "gettext"; then \
	  for file in VERSION ChangeLog COPYING.LIB-2.0 COPYING.LIB-2.1 $(DISTFILES.common) $(DISTFILES.generated); do \
	echo "Uninstalling $(gettextsrcdir)/$$file"; \
	    rm -f $(DESTDIR)$(gettextsrcdir)/$$file; \
	  done; \
	else \
	  : ; \
	fi
info dvi:
$(OBJECTS): ../config.h libgnuintl.h
bindtextdom.$lo dcgettext.$lo dcigettext.$lo dcngettext.$lo dgettext.$lo dngettext.$lo finddomain.$lo gettext.$lo intl-compat.$lo loadmsgcat.$lo localealias.$lo ngettext.$lo textdomain.$lo: gettextP.h gmo.h loadinfo.h
dcigettext.$lo: hash-string.h
explodename.$lo l10nflist.$lo: loadinfo.h
dcigettext.$lo loadmsgcat.$lo plural.$lo plural-exp.$lo: plural-exp.h
dcigettext.$lo: eval-plural.h
tags: TAGS
TAGS: $(HEADERS) $(SOURCES)
	here=`pwd`; cd $(srcdir) && etags -o $$here/TAGS $(HEADERS) $(SOURCES)
id: ID
ID: $(HEADERS) $(SOURCES)
	here=`pwd`; cd $(srcdir) && mkid -f$$here/ID $(HEADERS) $(SOURCES)
mostlyclean:
	rm -f *.a *.la *.o *.lo core core.*
	rm -f libintl.h charset.alias ref-add.sed ref-del.sed
	rm -f -r .libs _libs
clean: mostlyclean
distclean: clean
	rm -f Makefile ID TAGS
	if test "$(PACKAGE)" = gettext; then \
	  rm -f ChangeLog.inst $(DISTFILES.normal); \
	else \
	  : ; \
	fi
maintainer-clean: distclean
	@echo "This command is intended for maintainers to use>/dev/null;"
	@echo "it deletes files that may require special tools to rebuild.">/dev/null
distdir = ../$(PACKAGE)-$(VERSION)/$(subdir)
dist distdir: Makefile
	if test "$(PACKAGE)" = gettext; then \
	  additional="$(DISTFILES.gettext)"; \
	else \
	  additional="$(DISTFILES.normal)"; \
	fi; \
	$(MAKE) $(DISTFILES.common) $(DISTFILES.generated) $$additional; \
	for file in ChangeLog $(DISTFILES.common) $(DISTFILES.generated) $$additional; do \
	  if test -f $$file; then dir=.; else dir=$(srcdir); fi; \
	  cp -p $$dir/$$file $(distdir); \
	done
Makefile: Makefile.in ../config.status
	cd .. \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status
.NOEXPORT:
