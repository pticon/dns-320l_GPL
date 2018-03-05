MAKEFLAGS=-s --no-print-directory
PACKAGE = amule
VERSION = 2.2.6
SHELL = /bin/sh
srcdir = .
top_srcdir = ..
prefix = /usr/local
exec_prefix = ${prefix}
datarootdir = @datarootdir@
datadir = ${prefix}/share
localedir = $(datadir)/locale
gettextsrcdir = $(datadir)/gettext/po
INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
MKINSTALLDIRS = ./mkinstalldirs
mkinstalldirs = $(SHELL) `case "$(MKINSTALLDIRS)" in /*) echo "$(MKINSTALLDIRS)" ;; *) echo "$(top_builddir)/$(MKINSTALLDIRS)" ;; esac`
GMSGFMT = :
MSGFMT = :
XGETTEXT = :
MSGMERGE = msgmerge
MSGMERGE_UPDATE = : --update
MSGINIT = msginit
MSGCONV = msgconv
MSGFILTER = msgfilter
POFILES =  ar.po ast.po bg.po ca.po cs.po da.po de.po el.po en_GB.po es.po et_EE.po eu.po fi.po fr.po gl.po he.po hr.po hu.po it.po it_CH.po ja.po ko_KR.po lt.po nl.po nn.po pl.po pt_BR.po pt_PT.po ru.po sl.po sq.po sv.po tr.po uk.po zh_CN.po zh_TW.po
GMOFILES =  ar.gmo ast.gmo bg.gmo ca.gmo cs.gmo da.gmo de.gmo el.gmo en_GB.gmo es.gmo et_EE.gmo eu.gmo fi.gmo fr.gmo gl.gmo he.gmo hr.gmo hu.gmo it.gmo it_CH.gmo ja.gmo ko_KR.gmo lt.gmo nl.gmo nn.gmo pl.gmo pt_BR.gmo pt_PT.gmo ru.gmo sl.gmo sq.gmo sv.gmo tr.gmo uk.gmo zh_CN.gmo zh_TW.gmo
UPDATEPOFILES =  ar.po-update ast.po-update bg.po-update ca.po-update cs.po-update da.po-update de.po-update el.po-update en_GB.po-update es.po-update et_EE.po-update eu.po-update fi.po-update fr.po-update gl.po-update he.po-update hr.po-update hu.po-update it.po-update it_CH.po-update ja.po-update ko_KR.po-update lt.po-update nl.po-update nn.po-update pl.po-update pt_BR.po-update pt_PT.po-update ru.po-update sl.po-update sq.po-update sv.po-update tr.po-update uk.po-update zh_CN.po-update zh_TW.po-update
DUMMYPOFILES =  ar.nop ast.nop bg.nop ca.nop cs.nop da.nop de.nop el.nop en_GB.nop es.nop et_EE.nop eu.nop fi.nop fr.nop gl.nop he.nop hr.nop hu.nop it.nop it_CH.nop ja.nop ko_KR.nop lt.nop nl.nop nn.nop pl.nop pt_BR.nop pt_PT.nop ru.nop sl.nop sq.nop sv.nop tr.nop uk.nop zh_CN.nop zh_TW.nop
DISTFILES.common = Makefile.in.in Makevars remove-potcdate.sin \
$(DISTFILES.common.extra1) $(DISTFILES.common.extra2) $(DISTFILES.common.extra3)
DISTFILES = $(DISTFILES.common) POTFILES.in $(DOMAIN).pot \
$(POFILES) $(GMOFILES) \
$(DISTFILES.extra1) $(DISTFILES.extra2) $(DISTFILES.extra3)
POTFILES = \
     ../src/Parser.cpp \
     ../src/libs/ec/cpp/RemoteConnect.cpp \
     ../src/libs/ec/cpp/ECSpecialTags.cpp \
     ../src/libs/ec/cpp/ECSocket.cpp \
     ../src/ThreadTasks.cpp \
     ../src/BaseClient.cpp \
     ../src/FriendListCtrl.cpp \
     ../src/UserEvents.h \
     ../src/SharedFilesCtrl.cpp \
     ../src/UploadQueue.cpp \
     ../src/TextClient.h \
     ../src/kademlia/kademlia/SearchManager.cpp \
     ../src/kademlia/routing/RoutingZone.cpp \
     ../src/ListenSocket.cpp \
     ../src/webserver/src/WebServer.cpp \
     ../src/webserver/src/WebInterface.cpp \
     ../src/StatTree.cpp \
     ../src/HTTPDownload.cpp \
     ../src/utils/wxCas/src/wxcasprefs.cpp \
     ../src/utils/wxCas/src/linuxmon.cpp \
     ../src/utils/wxCas/src/wxcas.cpp \
     ../src/utils/wxCas/src/wxcasframe.cpp \
     ../src/utils/wxCas/src/onlinesig.cpp \
     ../src/utils/aLinkCreator/src/md4.cpp \
     ../src/utils/aLinkCreator/src/ed2khash.cpp \
     ../src/utils/aLinkCreator/src/alcframe.cpp \
     ../src/utils/aLinkCreator/src/alcc.cpp \
     ../src/utils/aLinkCreator/src/alc.cpp \
     ../src/utils/aLinkCreator/src/bithelp.h \
     ../src/DataToText.cpp \
     ../src/Preferences.cpp \
     ../src/ServerListCtrl.cpp \
     ../src/OScopeCtrl.cpp \
     ../src/amuleDlg.cpp \
     ../src/PrefsUnifiedDlg.cpp \
     ../src/ExternalConn.cpp \
     ../src/UploadClient.cpp \
     ../src/TerminationProcess.cpp \
     ../src/ServerWnd.cpp \
     ../src/Statistics.cpp \
     ../src/MuleTextCtrl.cpp \
     ../src/SearchList.cpp \
     ../src/ClientTCPSocket.cpp \
     ../src/KadDlg.cpp \
     ../src/Server.cpp \
     ../src/Logger.cpp \
     ../src/AddFriend.cpp \
     ../src/SearchListCtrl.cpp \
     ../src/amule-remote-gui.cpp \
     ../src/DownloadListCtrl.cpp \
     ../src/PartFile.cpp \
     ../src/KnownFileList.cpp \
     ../src/SharedFileList.cpp \
     ../src/CommentDialogLst.cpp \
     ../src/ServerConnect.cpp \
     ../src/DownloadQueue.cpp \
     ../src/TextClient.cpp \
     ../src/ServerSocket.cpp \
     ../src/IP2Country.cpp \
     ../src/TransferWnd.cpp \
     ../src/FileDetailListCtrl.cpp \
     ../src/KnownFile.cpp \
     ../src/CommentDialog.cpp \
     ../src/PartFileConvert.cpp \
     ../src/PartFileConvertDlg.cpp \
     ../src/OtherFunctions.cpp \
     ../src/IPFilter.cpp \
     ../src/StatisticsDlg.cpp \
     ../src/FileDetailDialog.cpp \
     ../src/muuli_wdr.cpp \
     ../src/CatDialog.cpp \
     ../src/ExternalConnector.cpp \
     ../src/ClientCreditsList.cpp \
     ../src/amuled.cpp \
     ../src/OtherStructs.h \
     ../src/ClientListCtrl.cpp \
     ../src/ClientDetailDialog.cpp \
     ../src/SearchDlg.cpp \
     ../src/MuleTrayIcon.cpp \
     ../src/ChatSelector.cpp \
     ../src/MuleNotebook.cpp \
     ../src/ServerList.cpp \
     ../src/UserEvents.cpp \
     ../src/amule.cpp \
     ../src/FriendList.cpp
CATALOGS =  ar.gmo ast.gmo bg.gmo ca.gmo cs.gmo da.gmo de.gmo el.gmo en_GB.gmo es.gmo et_EE.gmo eu.gmo fi.gmo fr.gmo gl.gmo he.gmo hr.gmo hu.gmo it.gmo it_CH.gmo ja.gmo ko_KR.gmo lt.gmo nl.gmo nn.gmo pl.gmo pt_BR.gmo pt_PT.gmo ru.gmo sl.gmo sq.gmo sv.gmo tr.gmo uk.gmo zh_CN.gmo zh_TW.gmo
DOMAIN = $(PACKAGE)
subdir = po
top_builddir = ..
XGETTEXT_OPTIONS = --keyword=_ --keyword=wxTRANSLATE --keyword=wxPLURAL:1,2
COPYRIGHT_HOLDER = Free Software Foundation, Inc.
EXTRA_LOCALE_CATEGORIES =
.SUFFIXES:
.SUFFIXES: .po .gmo .mo .sed .sin .nop .po-update
.po.mo:
	echo "Generating translations for $(<F:.po=)"
	@echo "$(MSGFMT) -c -o $@ $<">/dev/null; \
	$(MSGFMT) -c -o t-$@ $< && mv t-$@ $@
.po.gmo:
	echo "Generating translations for $(<F:.po=)"
	@lang=`echo $* | sed -e 's,.*/,,'`; \
	test "$(srcdir)" = . && cdcmd="" || cdcmd="cd $(srcdir) && "; \
	echo "$${cdcmd}rm -f $${lang}.gmo && $(GMSGFMT) -c  -o $${lang}.gmo $${lang}.po">/dev/null; \
	cd $(srcdir) && rm -f $${lang}.gmo && $(GMSGFMT) -c  -o t-$${lang}.gmo $${lang}.po && mv t-$${lang}.gmo $${lang}.gmo
.sin.sed:
	sed -e '/^#/d' $< > t-$@
	mv t-$@ $@
all: all-no
all-yes: $(CATALOGS)
all-no:
$(DOMAIN).pot-update: $(POTFILES) $(srcdir)/POTFILES.in remove-potcdate.sed
	echo "Updating translation catalog template"
	$(XGETTEXT) --default-domain=$(DOMAIN) --directory=$(top_srcdir) \
	  --add-comments=TRANSLATORS: $(XGETTEXT_OPTIONS) \
	  --files-from=$(srcdir)/POTFILES.in \
	  --copyright-holder='$(COPYRIGHT_HOLDER)'
	test ! -f $(DOMAIN).po || { \
	  if test -f $(srcdir)/$(DOMAIN).pot; then \
	    sed -f remove-potcdate.sed < $(srcdir)/$(DOMAIN).pot > $(DOMAIN).1po && \
	    sed -f remove-potcdate.sed < $(DOMAIN).po > $(DOMAIN).2po && \
	    if cmp $(DOMAIN).1po $(DOMAIN).2po >/dev/null 2>&1; then \
	      rm -f $(DOMAIN).1po $(DOMAIN).2po $(DOMAIN).po; \
	    else \
	      rm -f $(DOMAIN).1po $(DOMAIN).2po $(srcdir)/$(DOMAIN).pot && \
	      mv $(DOMAIN).po $(srcdir)/$(DOMAIN).pot; \
	    fi; \
	  else \
	    mv $(DOMAIN).po $(srcdir)/$(DOMAIN).pot; \
	  fi; \
	}
$(srcdir)/$(DOMAIN).pot:
	$(MAKE) $(DOMAIN).pot-update
$(POFILES): $(srcdir)/$(DOMAIN).pot
	echo "Updating translation catalog for $(@F:.po=)"
	@lang=`echo $@ | sed -e 's,.*/,,' -e 's/\.po$$//'`; \
	test "$(srcdir)" = . && cdcmd="" || cdcmd="cd $(srcdir) && "; \
	echo "$${cdcmd}$(MSGMERGE_UPDATE) --silent $${lang}.po $(DOMAIN).pot">/dev/null; \
	cd $(srcdir) && $(MSGMERGE_UPDATE) --silent $${lang}.po $(DOMAIN).pot
install: install-exec install-data
install-exec:
install-data: install-data-no
	if test "$(PACKAGE)" = "gettext"; then \
	  $(mkinstalldirs) $(DESTDIR)$(gettextsrcdir)>/dev/null; \
	  for file in $(DISTFILES.common); do \
	    $(INSTALL_DATA) $(srcdir)/$$file \
	    $(INSTALL_DATA) $(srcdir)/$$file \
			    $(DESTDIR)$(gettextsrcdir)/$$file; \
	  done; \
	else \
	  : ; \
	fi
install-data-no: all
install-data-yes: all
	$(mkinstalldirs) $(DESTDIR)$(datadir)>/dev/null
	@catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed -e 's/\.gmo$$//'`; \
	  dir=$(localedir)/$$lang/LC_MESSAGES; \
	  $(mkinstalldirs) $(DESTDIR)$$dir>/dev/null; \
	  if test -r $$cat; then realcat=$$cat; else realcat=$(srcdir)/$$cat; fi; \
	echo "Installing $$dir/$(DOMAIN).mo"; \
	  $(INSTALL_DATA) $$realcat $(DESTDIR)$$dir/$(DOMAIN).mo; \
	  echo "installing $$realcat as $(DESTDIR)$$dir/$(DOMAIN).mo">/dev/null; \
	  for lc in '' $(EXTRA_LOCALE_CATEGORIES); do \
	    if test -n "$$lc"; then \
	      if (cd $(DESTDIR)$(localedir)/$$lang && LC_ALL=C ls -l -d $$lc 2>/dev/null) | grep ' -> ' >/dev/null; then \
	        link=`cd $(DESTDIR)$(localedir)/$$lang && LC_ALL=C ls -l -d $$lc | sed -e 's/^.* -> //'`; \
	        mv $(DESTDIR)$(localedir)/$$lang/$$lc $(DESTDIR)$(localedir)/$$lang/$$lc.old; \
	        mkdir $(DESTDIR)$(localedir)/$$lang/$$lc; \
	        (cd $(DESTDIR)$(localedir)/$$lang/$$lc.old && \
	         for file in *; do \
	           if test -f $$file; then \
	             ln -s ../$$link/$$file $(DESTDIR)$(localedir)/$$lang/$$lc/$$file; \
	           fi; \
	         done); \
	        rm -f $(DESTDIR)$(localedir)/$$lang/$$lc.old; \
	      else \
	        if test -d $(DESTDIR)$(localedir)/$$lang/$$lc; then \
	          :; \
	        else \
	          rm -f $(DESTDIR)$(localedir)/$$lang/$$lc; \
	          mkdir $(DESTDIR)$(localedir)/$$lang/$$lc; \
	        fi; \
	      fi; \
	      rm -f $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo; \
	      ln -s ../LC_MESSAGES/$(DOMAIN).mo $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo 2>/dev/null || \
	      ln $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(DOMAIN).mo $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo 2>/dev/null || \
	      cp -p $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(DOMAIN).mo $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo; \
	      echo "installing $$realcat link as $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo">/dev/null; \
	    fi; \
	  done; \
	done
install-strip: install
installdirs: installdirs-exec installdirs-data
installdirs-exec:
installdirs-data: installdirs-data-no
	if test "$(PACKAGE)" = "gettext"; then \
	  $(mkinstalldirs) $(DESTDIR)$(gettextsrcdir)>/dev/null; \
	else \
	  : ; \
	fi
installdirs-data-no:
installdirs-data-yes:
	$(mkinstalldirs) $(DESTDIR)$(datadir)>/dev/null
	@catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed -e 's/\.gmo$$//'`; \
	  dir=$(localedir)/$$lang/LC_MESSAGES; \
	  $(mkinstalldirs) $(DESTDIR)$$dir>/dev/null; \
	  for lc in '' $(EXTRA_LOCALE_CATEGORIES); do \
	    if test -n "$$lc"; then \
	      if (cd $(DESTDIR)$(localedir)/$$lang && LC_ALL=C ls -l -d $$lc 2>/dev/null) | grep ' -> ' >/dev/null; then \
	        link=`cd $(DESTDIR)$(localedir)/$$lang && LC_ALL=C ls -l -d $$lc | sed -e 's/^.* -> //'`; \
	        mv $(DESTDIR)$(localedir)/$$lang/$$lc $(DESTDIR)$(localedir)/$$lang/$$lc.old; \
	        mkdir $(DESTDIR)$(localedir)/$$lang/$$lc; \
	        (cd $(DESTDIR)$(localedir)/$$lang/$$lc.old && \
	         for file in *; do \
	           if test -f $$file; then \
	             ln -s ../$$link/$$file $(DESTDIR)$(localedir)/$$lang/$$lc/$$file; \
	           fi; \
	         done); \
	        rm -f $(DESTDIR)$(localedir)/$$lang/$$lc.old; \
	      else \
	        if test -d $(DESTDIR)$(localedir)/$$lang/$$lc; then \
	          :; \
	        else \
	          rm -f $(DESTDIR)$(localedir)/$$lang/$$lc; \
	          mkdir $(DESTDIR)$(localedir)/$$lang/$$lc; \
	        fi; \
	      fi; \
	    fi; \
	  done; \
	done
installcheck:
uninstall: uninstall-exec uninstall-data
uninstall-exec:
uninstall-data: uninstall-data-no
	if test "$(PACKAGE)" = "gettext"; then \
	  for file in $(DISTFILES.common); do \
	echo "Uninstalling $(gettextsrcdir)/$$file"; \
	    rm -f $(DESTDIR)$(gettextsrcdir)/$$file; \
	  done; \
	else \
	  : ; \
	fi
uninstall-data-no:
uninstall-data-yes:
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed -e 's/\.gmo$$//'`; \
	  for lc in LC_MESSAGES $(EXTRA_LOCALE_CATEGORIES); do \
	echo "Uninstalling $(localedir)/$$lang/$$lc/$(DOMAIN).mo"; \
	    rm -f $(DESTDIR)$(localedir)/$$lang/$$lc/$(DOMAIN).mo; \
	  done; \
	done
check: all
dvi info tags TAGS ID:
mostlyclean:
	rm -f remove-potcdate.sed
	rm -f core core.* $(DOMAIN).po $(DOMAIN).1po $(DOMAIN).2po *.new.po
	rm -fr *.o
clean: mostlyclean
distclean: clean
	rm -f Makefile Makefile.in POTFILES *.mo
maintainer-clean: distclean
	@echo "This command is intended for maintainers to use>/dev/null;"
	@echo "it deletes files that may require special tools to rebuild.">/dev/null
	rm -f $(GMOFILES)
distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)
dist distdir:
	$(MAKE) update-po
	@$(MAKE) dist2
dist2: $(DISTFILES)
	dists="$(DISTFILES)"; \
	if test -f $(srcdir)/ChangeLog; then dists="$$dists ChangeLog"; fi; \
	if test -f $(srcdir)/LINGUAS; then dists="$$dists LINGUAS"; fi; \
	for file in $$dists; do \
	  if test -f $$file; then \
	    cp -p $$file $(distdir); \
	  else \
	    cp -p $(srcdir)/$$file $(distdir); \
	  fi; \
	done
update-po: Makefile
	$(MAKE) $(DOMAIN).pot-update
	$(MAKE) $(UPDATEPOFILES)
	$(MAKE) update-gmo
.nop.po-update:
	echo "Updating translation catalog for $(<F:.nop=)"
	@lang=`echo $@ | sed -e 's/\.po-update$$//'`; \
	if test "$(PACKAGE)" = "gettext"; then PATH=`pwd`/../src:$$PATH; fi; \
	tmpdir=`pwd`; \
	echo "$$lang:">/dev/null; \
	test "$(srcdir)" = . && cdcmd="" || cdcmd="cd $(srcdir) && "; \
	echo "$${cdcmd}$(MSGMERGE) --silent $$lang.po $(DOMAIN).pot -o $$lang.new.po">/dev/null; \
	cd $(srcdir); \
	if $(MSGMERGE) --silent $$lang.po $(DOMAIN).pot -o $$tmpdir/$$lang.new.po; then \
	  if cmp $$lang.po $$tmpdir/$$lang.new.po >/dev/null 2>&1; then \
	    rm -f $$tmpdir/$$lang.new.po; \
	  else \
	    if mv -f $$tmpdir/$$lang.new.po $$lang.po; then \
	      :; \
	    else \
	      echo "msgmerge for $$lang.po failed: cannot move $$tmpdir/$$lang.new.po to $$lang.po" 1>&2; \
	      exit 1; \
	    fi; \
	  fi; \
	else \
	  echo "msgmerge for $$lang.po failed!" 1>&2; \
	  rm -f $$tmpdir/$$lang.new.po; \
	fi
$(DUMMYPOFILES):
update-gmo: Makefile $(GMOFILES)
	@:
Makefile: Makefile.in.in $(top_builddir)/config.status POTFILES.in
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@.in CONFIG_HEADERS= \
	       $(SHELL) ./config.status
force:
.NOEXPORT:
DISTFILES.common.extra1 = quot.sed boldquot.sed en@quot.header en@boldquot.header insert-header.sin Rules-quot
.SUFFIXES: .insert-header .po-update-en
en@quot.po-update: en@quot.po-update-en
en@boldquot.po-update: en@boldquot.po-update-en
.insert-header.po-update-en:
	@lang=`echo $@ | sed -e 's/\.po-update-en$$//'`; \
	if test "$(PACKAGE)" = "gettext"; then PATH=`pwd`/../src:$$PATH; GETTEXTLIBDIR=`cd $(top_srcdir)/src && pwd`; export GETTEXTLIBDIR; fi; \
	tmpdir=`pwd`; \
	echo "$$lang:">/dev/null; \
	ll=`echo $$lang | sed -e 's/@.*//'`; \
	LC_ALL=C; export LC_ALL; \
	cd $(srcdir); \
	if $(MSGINIT) -i $(DOMAIN).pot --no-translator -l $$ll -o - 2>/dev/null | sed -f $$tmpdir/$$lang.insert-header | $(MSGCONV) -t UTF-8 | $(MSGFILTER) sed -f `echo $$lang | sed -e 's/.*@//'`.sed 2>/dev/null > $$tmpdir/$$lang.new.po; then \
	  if cmp $$lang.po $$tmpdir/$$lang.new.po >/dev/null 2>&1; then \
	    rm -f $$tmpdir/$$lang.new.po; \
	  else \
	    if mv -f $$tmpdir/$$lang.new.po $$lang.po; then \
	      :; \
	    else \
	      echo "creation of $$lang.po failed: cannot move $$tmpdir/$$lang.new.po to $$lang.po" 1>&2; \
	      exit 1; \
	    fi; \
	  fi; \
	else \
	  echo "creation of $$lang.po failed!" 1>&2; \
	  rm -f $$tmpdir/$$lang.new.po; \
	fi
en@quot.insert-header: insert-header.sin
	sed -e '/^#/d' -e 's/HEADER/en@quot.header/g' $(srcdir)/insert-header.sin > en@quot.insert-header
en@boldquot.insert-header: insert-header.sin
	sed -e '/^#/d' -e 's/HEADER/en@boldquot.header/g' $(srcdir)/insert-header.sin > en@boldquot.insert-header
mostlyclean: mostlyclean-quot
mostlyclean-quot:
	rm -f *.insert-header
