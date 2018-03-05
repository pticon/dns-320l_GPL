MAKEFLAGS=-s --no-print-directory
srcdir = .
top_srcdir = ../../..
pkgdatadir = $(datadir)/amule
pkglibdir = $(libdir)/amule
pkgincludedir = $(includedir)/amule
top_builddir = ../../..
am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
INSTALL = /usr/bin/install -c
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_HEADER = $(INSTALL_DATA)
transform = $(program_transform_name)
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
build_triplet = i686-pc-linux-gnu
host_triplet = arm-mv5sft-linux-gnueabi
bin_PROGRAMS = amuleweb$(EXEEXT)
subdir = src/webserver/src
DIST_COMMON = $(noinst_HEADERS) $(srcdir)/Makefile.am \
	$(srcdir)/Makefile.in
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
am__aclocal_m4_deps = $(top_srcdir)/m4/GeoIP.m4 \
	$(top_srcdir)/m4/build-tools.m4 $(top_srcdir)/m4/codeset.m4 \
	$(top_srcdir)/m4/cryptopp.m4 $(top_srcdir)/m4/fallocate.m4 \
	$(top_srcdir)/m4/gdlib.m4 $(top_srcdir)/m4/gettext.m4 \
	$(top_srcdir)/m4/glibc21.m4 $(top_srcdir)/m4/iconv.m4 \
	$(top_srcdir)/m4/intdiv0.m4 $(top_srcdir)/m4/inttypes-pri.m4 \
	$(top_srcdir)/m4/inttypes.m4 $(top_srcdir)/m4/inttypes_h.m4 \
	$(top_srcdir)/m4/lcmessage.m4 $(top_srcdir)/m4/lib-ld.m4 \
	$(top_srcdir)/m4/lib-link.m4 $(top_srcdir)/m4/lib-prefix.m4 \
	$(top_srcdir)/m4/libpng.m4 $(top_srcdir)/m4/libupnp.m4 \
	$(top_srcdir)/m4/nls.m4 $(top_srcdir)/m4/pkg.m4 \
	$(top_srcdir)/m4/progtest.m4 $(top_srcdir)/m4/readline.m4 \
	$(top_srcdir)/m4/stdint_h.m4 $(top_srcdir)/m4/uintmax_t.m4 \
	$(top_srcdir)/m4/ulonglong.m4 $(top_srcdir)/m4/wxwin.m4 \
	$(top_srcdir)/m4/zlib.m4 $(top_srcdir)/acinclude.m4 \
	$(top_srcdir)/configure.in
am__configure_deps = $(am__aclocal_m4_deps) $(CONFIGURE_DEPENDENCIES) \
	$(ACLOCAL_M4)
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = $(top_builddir)/config.h
CONFIG_CLEAN_FILES =
am__installdirs = "$(DESTDIR)$(bindir)"
binPROGRAMS_INSTALL = $(INSTALL_PROGRAM)
PROGRAMS = $(bin_PROGRAMS)
am__amuleweb_SOURCES_DIST = WebServer.cpp WebInterface.cpp \
	WebSocket.cpp $(top_srcdir)/src/ExternalConnector.cpp \
	$(top_srcdir)/src/OtherFunctions.cpp $(top_srcdir)/src/RLE.cpp \
	$(top_srcdir)/src/NetworkFunctions.cpp php_parser.c \
	php_lexer.c php_syntree.cpp php_amule_lib.cpp php_core_lib.cpp \
	$(top_srcdir)/src/UPnPBase.cpp UPnPCompatibility.cpp
am_amuleweb_OBJECTS = amuleweb-WebServer.$(OBJEXT) \
	amuleweb-WebInterface.$(OBJEXT) amuleweb-WebSocket.$(OBJEXT) \
	amuleweb-ExternalConnector.$(OBJEXT) \
	amuleweb-OtherFunctions.$(OBJEXT) amuleweb-RLE.$(OBJEXT) \
	amuleweb-NetworkFunctions.$(OBJEXT) \
	amuleweb-php_parser.$(OBJEXT) amuleweb-php_lexer.$(OBJEXT) \
	amuleweb-php_syntree.$(OBJEXT) \
	amuleweb-php_amule_lib.$(OBJEXT) \
	amuleweb-php_core_lib.$(OBJEXT) $(am__objects_1)
amuleweb_OBJECTS = $(am_amuleweb_OBJECTS) $(nodist_amuleweb_OBJECTS)
am__DEPENDENCIES_1 =
DEFAULT_INCLUDES = -I. -I$(srcdir) -I$(top_builddir)
depcomp = $(SHELL) $(top_srcdir)/depcomp
am__depfiles_maybe = depfiles
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
CXXLD = $(CXX)
CXXLINK = $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) \
	-o $@
SOURCES = $(amuleweb_SOURCES) $(nodist_amuleweb_SOURCES)
DIST_SOURCES = $(am__amuleweb_SOURCES_DIST)
HEADERS = $(noinst_HEADERS)
ETAGS = etags
CTAGS = ctags
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)
ACLOCAL = ${SHELL} /home/patrick/nas/DNS-345_GPL/aMule-2.2.6/missing --run aclocal-1.9
ALCC_FALSE = 
ALCC_MANPAGES = 
ALCC_TRUE = #
ALC_FALSE = 
ALC_MANPAGES = 
ALC_TRUE = #
ALLOCA = 
AMDEP_FALSE = #
AMDEP_TRUE = 
AMTAR = ${SHELL} /home/patrick/nas/DNS-345_GPL/aMule-2.2.6/missing --run tar
AMULECMD_MANPAGES = 
AMULEWEB_DEFS = -DWITH_LIBPNG
AMULE_DAEMON_FALSE = #
AMULE_DAEMON_MANPAGES = 
AMULE_DAEMON_TRUE = 
AMULE_GUI_FALSE = 
AMULE_GUI_MANPAGES = 
AMULE_GUI_TRUE = #
AR = arm-mv5sft-linux-gnueabi-ar
AUTOCONF = ${SHELL} /home/patrick/nas/DNS-345_GPL/aMule-2.2.6/missing --run autoconf
AUTOHEADER = ${SHELL} /home/patrick/nas/DNS-345_GPL/aMule-2.2.6/missing --run autoheader
AUTOMAKE = ${SHELL} /home/patrick/nas/DNS-345_GPL/aMule-2.2.6/missing --run automake-1.9
AWK = gawk
BFD_CPPFLAGS = 
BFD_LIBS = 
BUILD_CC = gcc
BUILD_CFLAGS = 
BUILD_CPPFLAGS = 
BUILD_EXEEXT = 
BUILD_INCLUDED_LIBINTL = no
BUILD_LDFLAGS = 
CAS_DEFS = 
CAS_FALSE = 
CAS_MANPAGES = 
CAS_TRUE = #
CATOBJEXT = .gmo
CC = arm-mv5sft-linux-gnueabi-gcc
CCDEPMODE = depmode=gcc3
CFLAGS = 
COMPILE_CMD_FALSE = 
COMPILE_CMD_TRUE = #
COMPILE_NLS_FALSE = 
COMPILE_NLS_TRUE = #
CPP = arm-mv5sft-linux-gnueabi-gcc -E
CPPFLAGS = -I/home/patrick/nas/DNS-345_GPL/aMule-2.2.6/../include
CRYPTOPP_CPPFLAGS =  -D__WEAK_CRYPTO__
CRYPTOPP_LDFLAGS = 
CRYPTOPP_LIBS = /home/patrick/nas/DNS-345_GPL/aMule-2.2.6/../cryptopp552/libcryptopp.a
CXX = arm-mv5sft-linux-gnueabi-g++
CXXCPP = arm-mv5sft-linux-gnueabi-g++ -E
CXXDEPMODE = depmode=gcc3
CXXFLAGS = 
CYGPATH_W = echo
DATADIRNAME = share
DEFS = -DHAVE_CONFIG_H
DEPDIR = .deps
ECHO_C = 
ECHO_N = -n
ECHO_T = 
ED2K_FALSE = #
ED2K_MANPAGES = 
ED2K_TRUE = 
EGREP = grep -E
ENABLE_IP2COUNTRY_FALSE = 
ENABLE_IP2COUNTRY_TRUE = #
ENABLE_UPNP_FALSE = 
ENABLE_UPNP_TRUE = #
EXEEXT = 
GDLIB_CFLAGS = 
GDLIB_CONFIG_PATH = 
GDLIB_LDFLAGS = 
GDLIB_LIBS = 
GENCAT = gencat
GENERATE_FLEX_HEADER_FALSE = 
GENERATE_FLEX_HEADER_TRUE = #
GEOIP_CPPFLAGS = 
GEOIP_LDFLAGS = 
GEOIP_LIBS = 
GLIBC21 = yes
GMSGFMT = :
INCINTL = 
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_SKINS_FALSE = 
INSTALL_SKINS_TRUE = #
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
INSTOBJEXT = .mo
INTLBISON = :
INTLLIBS = 
INTLOBJS = 
INTL_LIBTOOL_SUFFIX_PREFIX = 
LD = arm-mv5sft-linux-gnueabi-ld
LDFLAGS = -L/home/patrick/nas/DNS-345_GPL/aMule-2.2.6/../lib
LEX = :
LEXLIB = -lfl
LEX_OUTPUT_ROOT = 
LIBICONV = /home/patrick/nas/DNS-345_GPL/aMule-2.2.6/../lib/libiconv.so -Wl,-rpath -Wl,/home/patrick/nas/DNS-345_GPL/aMule-2.2.6/../lib
LIBINTL = 
LIBOBJS =  lstat$U.o memcmp$U.o stat$U.o strtod$U.o
LIBPNG_CFLAGS = -I/usr/local/include/libpng12
LIBPNG_CONFIG_PATH = ../libpng-1.2.39/libpng-config
LIBPNG_LDFLAGS = -L/usr/local/lib
LIBPNG_LIBS = -lpng12
LIBS = 
LIBUPNP_CFLAGS = 
LIBUPNP_CPPFLAGS = 
LIBUPNP_LDFLAGS = 
LIBUPNP_LIBS = 
LTLIBICONV = -L/home/patrick/nas/DNS-345_GPL/aMule-2.2.6/../lib -liconv -R/home/patrick/nas/DNS-345_GPL/aMule-2.2.6/../lib
LTLIBINTL = 
LTLIBOBJS =  lstat$U.lo memcmp$U.lo stat$U.lo strtod$U.lo
MAINT = #
MAINTAINER_MODE_FALSE = 
MAINTAINER_MODE_TRUE = #
MAKEINFO = ${SHELL} /home/patrick/nas/DNS-345_GPL/aMule-2.2.6/missing --run makeinfo
MKINSTALLDIRS = ./mkinstalldirs
MONOLITHIC_FALSE = 
MONOLITHIC_MANPAGES = 
MONOLITHIC_TRUE = #
MSGFMT = :
MSGMERGE = :
MULECFLAGS = -w  -g -ggdb -fno-inline -fmessage-length=0
MULECPPFLAGS =  -D__DEBUG__ -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -DUSE_WX_EXTENSIONS
MULECXXFLAGS = -w  -g -ggdb -fno-inline -fmessage-length=0
MULELDFLAGS =  -lpthread
MULERCFLAGS = 
NEED_CORESERVICES_FALSE = 
NEED_CORESERVICES_TRUE = #
NEED_RC_FALSE = 
NEED_RC_TRUE = #
OBJEXT = o
PACKAGE = amule
PACKAGE_BUGREPORT = admin@amule.org
PACKAGE_NAME = aMule
PACKAGE_STRING = aMule 2.2.6
PACKAGE_TARNAME = amule
PACKAGE_VERSION = 2.2.6
PATH_SEPARATOR = :
PKG_CONFIG = /usr/bin/pkg-config
POSUB = po
POW_LIB = -lm
RANLIB = arm-mv5sft-linux-gnueabi-ranlib
RC = 
RCFLAGS = 
READLINE_LIBS = 
REGEX_LIBS = 
RESOLV_LIB = 
SET_MAKE = 
SHELL = /bin/sh
STRIP = arm-mv5sft-linux-gnueabi-strip
SVNDATE = 
SYS_WIN32_FALSE = 
SYS_WIN32_TRUE = #
USE_INCLUDED_LIBINTL = no
USE_NLS = no
VERSION = 2.2.6
WEB_FALSE = #
WEB_MANPAGES = 
WEB_TRUE = 
WXBASE_CPPFLAGS = -I/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/lib/wx/include/arm-mv5sft-linux-gnueabi-base-unicode-release-2.8 -I/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/include -I/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/contrib/include -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -DwxUSE_GUI=0 -DwxUSE_GUI=0
WXBASE_LIBS = -L/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/lib -pthread   -Wl,-rpath,/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/lib -lwx_baseu_net-2.8-arm-mv5sft-linux-gnueabi -lwx_baseu-2.8-arm-mv5sft-linux-gnueabi 
WXCAS_FALSE = 
WXCAS_MANPAGES = 
WXCAS_TRUE = #
WX_CFLAGS = -I/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/lib/wx/include/arm-mv5sft-linux-gnueabi-base-unicode-release-2.8 -I/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/include -I/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/contrib/include -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -DwxUSE_GUI=0 -pthread
WX_CFLAGS_ONLY = -pthread
WX_CONFIG_PATH = ../wxWidgets-2.8.9/wx-config
WX_CPPFLAGS = -I/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/lib/wx/include/arm-mv5sft-linux-gnueabi-base-unicode-release-2.8 -I/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/include -I/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/contrib/include -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -DwxUSE_GUI=0
WX_CXXFLAGS = -I/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/lib/wx/include/arm-mv5sft-linux-gnueabi-base-unicode-release-2.8 -I/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/include -I/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/contrib/include -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -DwxUSE_GUI=0 -pthread
WX_CXXFLAGS_ONLY = 
WX_DEBUG = 0
WX_LIBS = -L/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/lib -pthread   -Wl,-rpath,/home/patrick/nas/DNS-345_GPL/wxWidgets-2.8.9/lib -lwx_baseu_adv-2.8-arm-mv5sft-linux-gnueabi -lwx_baseu_core-2.8-arm-mv5sft-linux-gnueabi -lwx_baseu_net-2.8-arm-mv5sft-linux-gnueabi -lwx_baseu-2.8-arm-mv5sft-linux-gnueabi 
WX_LIBS_STATIC = 
WX_PORT = base
WX_RESCOMP = 
WX_SHARED = 1
WX_UNICODE = 
WX_VERSION_FULL = 2.8.9
WX_VERSION_MAJOR = 2
WX_VERSION_MINOR = 8
X11LIBS = 
XGETTEXT = :
YACC = yacc
ZLIB_CPPFLAGS = -I/home/patrick/nas/DNS-345_GPL/aMule-2.2.6/..//include
ZLIB_LDFLAGS = -L/home/patrick/nas/DNS-345_GPL/aMule-2.2.6/..//lib
ZLIB_LIBS = -lz
ac_ct_AR = 
ac_ct_BUILD_CC = gcc
ac_ct_CC = 
ac_ct_CXX = 
ac_ct_LD = 
ac_ct_RANLIB = 
ac_ct_RC = 
ac_ct_STRIP = 
ac_pt_PKG_CONFIG = /usr/bin/pkg-config
am__fastdepCC_FALSE = #
am__fastdepCC_TRUE = 
am__fastdepCXX_FALSE = #
am__fastdepCXX_TRUE = 
am__include = include
am__leading_dot = .
am__quote = 
am__tar = ${AMTAR} chof - "$$tardir"
am__untar = ${AMTAR} xf -
bindir = ${exec_prefix}/bin
build = i686-pc-linux-gnu
build_alias = 
build_cpu = i686
build_os = linux-gnu
build_vendor = pc
datadir = ${prefix}/share
docdir = ${prefix}/share/doc/aMule-2.2.6
exec_prefix = ${prefix}
host = arm-mv5sft-linux-gnueabi
host_alias = arm-mv5sft-linux-gnueabi
host_cpu = arm
host_os = linux-gnueabi
host_vendor = mv5sft
includedir = ${prefix}/include
infodir = ${prefix}/info
install_sh = /home/patrick/nas/DNS-345_GPL/aMule-2.2.6/install-sh
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
localstatedir = ${prefix}/var
mandir = ${prefix}/man
mkdir_p = mkdir -p --
oldincludedir = /usr/include
prefix = /usr/local
program_transform_name = s,x,x,
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${prefix}/com
sysconfdir = ${prefix}/etc
target_alias = 
amuleweb_SOURCES = WebServer.cpp WebInterface.cpp WebSocket.cpp \
	$(top_srcdir)/src/ExternalConnector.cpp \
	$(top_srcdir)/src/OtherFunctions.cpp $(top_srcdir)/src/RLE.cpp \
	$(top_srcdir)/src/NetworkFunctions.cpp php_parser.c \
	php_lexer.c php_syntree.cpp php_amule_lib.cpp php_core_lib.cpp \
	$(am__append_1)
amuleweb_DEPENDENCIES = ../../libs/common/libmulecommon.a ../../libs/ec/cpp/libec.a
amuleweb_CPPFLAGS = -I../.. -I$(top_srcdir)/src -I$(top_srcdir)/src/libs -I$(top_srcdir)/src/include \
	$(MULECPPFLAGS) $(AMULEWEB_DEFS) $(BFD_CPPFLAGS) $(WXBASE_CPPFLAGS) \
	-DEC_REMOTE -DWEBSERVERDIR="\"$(pkgdatadir)/webserver\"" -DECSOCKET_USE_EVENTS=0 \
	$(INCINTL) $(ZLIB_CPPFLAGS) $(LIBUPNP_CPPFLAGS)
amuleweb_CFLAGS = $(MULECFLAGS) $(WX_CFLAGS_ONLY) $(LIBPNG_CFLAGS) $(LIBUPNP_CFLAGS)
amuleweb_CXXFLAGS = $(MULECXXFLAGS) $(WX_CFLAGS_ONLY) $(WX_CXXFLAGS_ONLY) $(LIBPNG_CFLAGS) $(LIBUPNP_CFLAGS)
amuleweb_LDFLAGS = $(MULELDFLAGS) -L../../libs/common -L../../libs/ec/cpp $(LIBPNG_LDFLAGS) $(ZLIB_LDFLAGS) $(LIBUPNP_LDFLAGS)
amuleweb_LDADD = -lec -lmulecommon $(WXBASE_LIBS) $(BFD_LIBS) $(ZLIB_LIBS) \
	$(READLINE_LIBS) $(LIBPNG_LIBS) $(RESOLV_LIB) $(REGEX_LIBS) $(LIBINTL) $(LIBUPNP_LIBS)
noinst_HEADERS = \
		 WebInterface.h \
		 WebServer.h \
		 WebSocket.h \
		 php_core_lib.h \
		 php_parser.h \
		 php_syntree.h
MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST = \
	missing-template.html \
	php_lexer.l \
	php_parser.y
all: all-am
.SUFFIXES:
.SUFFIXES: .rc .$(OBJEXT) .c .cpp .o .obj
$(srcdir)/Makefile.in: # $(srcdir)/Makefile.am  $(am__configure_deps)
	@for dep in $?; do \
	  case '$(am__configure_deps)' in \
	    *$$dep*) \
	      cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh \
		&& exit 0; \
	      exit 1;; \
	  esac; \
	done; \
	echo ' cd $(top_srcdir) && $(AUTOMAKE) --foreign  src/webserver/src/Makefile'>/dev/null; \
	cd $(top_srcdir) && \
	  $(AUTOMAKE) --foreign  src/webserver/src/Makefile
.PRECIOUS: Makefile
Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	@case '$?' in \
	  *config.status*) \
	    cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh;; \
	  *) \
	    echo ' cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe)'>/dev/null; \
	    cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__depfiles_maybe) denoiser;; \
	esac;
$(top_builddir)/config.status: $(top_srcdir)/configure $(CONFIG_STATUS_DEPENDENCIES)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(top_srcdir)/configure: # $(am__configure_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(ACLOCAL_M4): # $(am__aclocal_m4_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	test -z "$(bindir)" || $(mkdir_p) "$(DESTDIR)$(bindir)"
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  p1=`echo $$p|sed 's/$(EXEEXT)$$//'`; \
	  if test -f $$p \
	  ; then \
	    f=`echo "$$p1" | sed 's,^.*/,,;$(transform);s/$$/$(EXEEXT)/'`; \
	   echo " $(INSTALL_PROGRAM_ENV) $(binPROGRAMS_INSTALL) '$$p' '$(DESTDIR)$(bindir)/$$f'">/dev/null; \
	echo "Installing $(bindir)/$$f"; \
	   $(INSTALL_PROGRAM_ENV) $(binPROGRAMS_INSTALL) "$$p" "$(DESTDIR)$(bindir)/$$f" || exit 1; \
	  else :; fi; \
	done
uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  f=`echo "$$p" | sed 's,^.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  echo " rm -f '$(DESTDIR)$(bindir)/$$f'">/dev/null; \
	echo "Uninstalling $(bindir)/$$f"; \
	  rm -f "$(DESTDIR)$(bindir)/$$f"; \
	done
clean-binPROGRAMS:
	-test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)
amuleweb$(EXEEXT): $(amuleweb_OBJECTS) $(amuleweb_DEPENDENCIES) 
	echo "Linking $(@F)"
	@rm -f amuleweb$(EXEEXT)
	$(CXXLINK) $(amuleweb_LDFLAGS) $(amuleweb_OBJECTS) $(amuleweb_LDADD) $(LIBS)
mostlyclean-compile:
	-rm -f *.$(OBJEXT)
distclean-compile:
	-rm -f *.tab.c
include ./$(DEPDIR)/amuleweb-ExternalConnector.Po
include ./$(DEPDIR)/amuleweb-NetworkFunctions.Po
include ./$(DEPDIR)/amuleweb-OtherFunctions.Po
include ./$(DEPDIR)/amuleweb-RLE.Po
include ./$(DEPDIR)/amuleweb-UPnPBase.Po
include ./$(DEPDIR)/amuleweb-UPnPCompatibility.Po
include ./$(DEPDIR)/amuleweb-WebInterface.Po
include ./$(DEPDIR)/amuleweb-WebServer.Po
include ./$(DEPDIR)/amuleweb-WebSocket.Po
include ./$(DEPDIR)/amuleweb-php_amule_lib.Po
include ./$(DEPDIR)/amuleweb-php_core_lib.Po
include ./$(DEPDIR)/amuleweb-php_lexer.Po
include ./$(DEPDIR)/amuleweb-php_parser.Po
include ./$(DEPDIR)/amuleweb-php_syntree.Po
.c.o:
	echo "Compiling $(<F)"
	if $(COMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" -c -o $@ $<; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; fi
.c.obj:
	echo "Compiling $(<F)"
	if $(COMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" -c -o $@ `$(CYGPATH_W) '$<'`; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; fi
amuleweb-php_parser.o: php_parser.c
	echo "Compiling $(<F)"
	if $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CFLAGS) $(CFLAGS) -MT amuleweb-php_parser.o -MD -MP -MF "$(DEPDIR)/amuleweb-php_parser.Tpo" -c -o amuleweb-php_parser.o `test -f 'php_parser.c' || echo '$(srcdir)/'`php_parser.c; \
	then mv -f "$(DEPDIR)/amuleweb-php_parser.Tpo" "$(DEPDIR)/amuleweb-php_parser.Po"; else rm -f "$(DEPDIR)/amuleweb-php_parser.Tpo"; exit 1; fi
amuleweb-php_parser.obj: php_parser.c
	echo "Compiling $(<F)"
	if $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CFLAGS) $(CFLAGS) -MT amuleweb-php_parser.obj -MD -MP -MF "$(DEPDIR)/amuleweb-php_parser.Tpo" -c -o amuleweb-php_parser.obj `if test -f 'php_parser.c'; then $(CYGPATH_W) 'php_parser.c'; else $(CYGPATH_W) '$(srcdir)/php_parser.c'; fi`; \
	then mv -f "$(DEPDIR)/amuleweb-php_parser.Tpo" "$(DEPDIR)/amuleweb-php_parser.Po"; else rm -f "$(DEPDIR)/amuleweb-php_parser.Tpo"; exit 1; fi
amuleweb-php_lexer.o: php_lexer.c
	echo "Compiling $(<F)"
	if $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CFLAGS) $(CFLAGS) -MT amuleweb-php_lexer.o -MD -MP -MF "$(DEPDIR)/amuleweb-php_lexer.Tpo" -c -o amuleweb-php_lexer.o `test -f 'php_lexer.c' || echo '$(srcdir)/'`php_lexer.c; \
	then mv -f "$(DEPDIR)/amuleweb-php_lexer.Tpo" "$(DEPDIR)/amuleweb-php_lexer.Po"; else rm -f "$(DEPDIR)/amuleweb-php_lexer.Tpo"; exit 1; fi
amuleweb-php_lexer.obj: php_lexer.c
	echo "Compiling $(<F)"
	if $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CFLAGS) $(CFLAGS) -MT amuleweb-php_lexer.obj -MD -MP -MF "$(DEPDIR)/amuleweb-php_lexer.Tpo" -c -o amuleweb-php_lexer.obj `if test -f 'php_lexer.c'; then $(CYGPATH_W) 'php_lexer.c'; else $(CYGPATH_W) '$(srcdir)/php_lexer.c'; fi`; \
	then mv -f "$(DEPDIR)/amuleweb-php_lexer.Tpo" "$(DEPDIR)/amuleweb-php_lexer.Po"; else rm -f "$(DEPDIR)/amuleweb-php_lexer.Tpo"; exit 1; fi
.cpp.o:
	echo "Compiling $(<F)"
	if $(CXXCOMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" -c -o $@ $<; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; fi
.cpp.obj:
	echo "Compiling $(<F)"
	if $(CXXCOMPILE) -MT $@ -MD -MP -MF "$(DEPDIR)/$*.Tpo" -c -o $@ `$(CYGPATH_W) '$<'`; \
	then mv -f "$(DEPDIR)/$*.Tpo" "$(DEPDIR)/$*.Po"; else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; fi
amuleweb-WebServer.o: WebServer.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-WebServer.o -MD -MP -MF "$(DEPDIR)/amuleweb-WebServer.Tpo" -c -o amuleweb-WebServer.o `test -f 'WebServer.cpp' || echo '$(srcdir)/'`WebServer.cpp; \
	then mv -f "$(DEPDIR)/amuleweb-WebServer.Tpo" "$(DEPDIR)/amuleweb-WebServer.Po"; else rm -f "$(DEPDIR)/amuleweb-WebServer.Tpo"; exit 1; fi
amuleweb-WebServer.obj: WebServer.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-WebServer.obj -MD -MP -MF "$(DEPDIR)/amuleweb-WebServer.Tpo" -c -o amuleweb-WebServer.obj `if test -f 'WebServer.cpp'; then $(CYGPATH_W) 'WebServer.cpp'; else $(CYGPATH_W) '$(srcdir)/WebServer.cpp'; fi`; \
	then mv -f "$(DEPDIR)/amuleweb-WebServer.Tpo" "$(DEPDIR)/amuleweb-WebServer.Po"; else rm -f "$(DEPDIR)/amuleweb-WebServer.Tpo"; exit 1; fi
amuleweb-WebInterface.o: WebInterface.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-WebInterface.o -MD -MP -MF "$(DEPDIR)/amuleweb-WebInterface.Tpo" -c -o amuleweb-WebInterface.o `test -f 'WebInterface.cpp' || echo '$(srcdir)/'`WebInterface.cpp; \
	then mv -f "$(DEPDIR)/amuleweb-WebInterface.Tpo" "$(DEPDIR)/amuleweb-WebInterface.Po"; else rm -f "$(DEPDIR)/amuleweb-WebInterface.Tpo"; exit 1; fi
amuleweb-WebInterface.obj: WebInterface.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-WebInterface.obj -MD -MP -MF "$(DEPDIR)/amuleweb-WebInterface.Tpo" -c -o amuleweb-WebInterface.obj `if test -f 'WebInterface.cpp'; then $(CYGPATH_W) 'WebInterface.cpp'; else $(CYGPATH_W) '$(srcdir)/WebInterface.cpp'; fi`; \
	then mv -f "$(DEPDIR)/amuleweb-WebInterface.Tpo" "$(DEPDIR)/amuleweb-WebInterface.Po"; else rm -f "$(DEPDIR)/amuleweb-WebInterface.Tpo"; exit 1; fi
amuleweb-WebSocket.o: WebSocket.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-WebSocket.o -MD -MP -MF "$(DEPDIR)/amuleweb-WebSocket.Tpo" -c -o amuleweb-WebSocket.o `test -f 'WebSocket.cpp' || echo '$(srcdir)/'`WebSocket.cpp; \
	then mv -f "$(DEPDIR)/amuleweb-WebSocket.Tpo" "$(DEPDIR)/amuleweb-WebSocket.Po"; else rm -f "$(DEPDIR)/amuleweb-WebSocket.Tpo"; exit 1; fi
amuleweb-WebSocket.obj: WebSocket.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-WebSocket.obj -MD -MP -MF "$(DEPDIR)/amuleweb-WebSocket.Tpo" -c -o amuleweb-WebSocket.obj `if test -f 'WebSocket.cpp'; then $(CYGPATH_W) 'WebSocket.cpp'; else $(CYGPATH_W) '$(srcdir)/WebSocket.cpp'; fi`; \
	then mv -f "$(DEPDIR)/amuleweb-WebSocket.Tpo" "$(DEPDIR)/amuleweb-WebSocket.Po"; else rm -f "$(DEPDIR)/amuleweb-WebSocket.Tpo"; exit 1; fi
amuleweb-ExternalConnector.o: $(top_srcdir)/src/ExternalConnector.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-ExternalConnector.o -MD -MP -MF "$(DEPDIR)/amuleweb-ExternalConnector.Tpo" -c -o amuleweb-ExternalConnector.o `test -f '$(top_srcdir)/src/ExternalConnector.cpp' || echo '$(srcdir)/'`$(top_srcdir)/src/ExternalConnector.cpp; \
	then mv -f "$(DEPDIR)/amuleweb-ExternalConnector.Tpo" "$(DEPDIR)/amuleweb-ExternalConnector.Po"; else rm -f "$(DEPDIR)/amuleweb-ExternalConnector.Tpo"; exit 1; fi
amuleweb-ExternalConnector.obj: $(top_srcdir)/src/ExternalConnector.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-ExternalConnector.obj -MD -MP -MF "$(DEPDIR)/amuleweb-ExternalConnector.Tpo" -c -o amuleweb-ExternalConnector.obj `if test -f '$(top_srcdir)/src/ExternalConnector.cpp'; then $(CYGPATH_W) '$(top_srcdir)/src/ExternalConnector.cpp'; else $(CYGPATH_W) '$(srcdir)/$(top_srcdir)/src/ExternalConnector.cpp'; fi`; \
	then mv -f "$(DEPDIR)/amuleweb-ExternalConnector.Tpo" "$(DEPDIR)/amuleweb-ExternalConnector.Po"; else rm -f "$(DEPDIR)/amuleweb-ExternalConnector.Tpo"; exit 1; fi
amuleweb-OtherFunctions.o: $(top_srcdir)/src/OtherFunctions.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-OtherFunctions.o -MD -MP -MF "$(DEPDIR)/amuleweb-OtherFunctions.Tpo" -c -o amuleweb-OtherFunctions.o `test -f '$(top_srcdir)/src/OtherFunctions.cpp' || echo '$(srcdir)/'`$(top_srcdir)/src/OtherFunctions.cpp; \
	then mv -f "$(DEPDIR)/amuleweb-OtherFunctions.Tpo" "$(DEPDIR)/amuleweb-OtherFunctions.Po"; else rm -f "$(DEPDIR)/amuleweb-OtherFunctions.Tpo"; exit 1; fi
amuleweb-OtherFunctions.obj: $(top_srcdir)/src/OtherFunctions.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-OtherFunctions.obj -MD -MP -MF "$(DEPDIR)/amuleweb-OtherFunctions.Tpo" -c -o amuleweb-OtherFunctions.obj `if test -f '$(top_srcdir)/src/OtherFunctions.cpp'; then $(CYGPATH_W) '$(top_srcdir)/src/OtherFunctions.cpp'; else $(CYGPATH_W) '$(srcdir)/$(top_srcdir)/src/OtherFunctions.cpp'; fi`; \
	then mv -f "$(DEPDIR)/amuleweb-OtherFunctions.Tpo" "$(DEPDIR)/amuleweb-OtherFunctions.Po"; else rm -f "$(DEPDIR)/amuleweb-OtherFunctions.Tpo"; exit 1; fi
amuleweb-RLE.o: $(top_srcdir)/src/RLE.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-RLE.o -MD -MP -MF "$(DEPDIR)/amuleweb-RLE.Tpo" -c -o amuleweb-RLE.o `test -f '$(top_srcdir)/src/RLE.cpp' || echo '$(srcdir)/'`$(top_srcdir)/src/RLE.cpp; \
	then mv -f "$(DEPDIR)/amuleweb-RLE.Tpo" "$(DEPDIR)/amuleweb-RLE.Po"; else rm -f "$(DEPDIR)/amuleweb-RLE.Tpo"; exit 1; fi
amuleweb-RLE.obj: $(top_srcdir)/src/RLE.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-RLE.obj -MD -MP -MF "$(DEPDIR)/amuleweb-RLE.Tpo" -c -o amuleweb-RLE.obj `if test -f '$(top_srcdir)/src/RLE.cpp'; then $(CYGPATH_W) '$(top_srcdir)/src/RLE.cpp'; else $(CYGPATH_W) '$(srcdir)/$(top_srcdir)/src/RLE.cpp'; fi`; \
	then mv -f "$(DEPDIR)/amuleweb-RLE.Tpo" "$(DEPDIR)/amuleweb-RLE.Po"; else rm -f "$(DEPDIR)/amuleweb-RLE.Tpo"; exit 1; fi
amuleweb-NetworkFunctions.o: $(top_srcdir)/src/NetworkFunctions.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-NetworkFunctions.o -MD -MP -MF "$(DEPDIR)/amuleweb-NetworkFunctions.Tpo" -c -o amuleweb-NetworkFunctions.o `test -f '$(top_srcdir)/src/NetworkFunctions.cpp' || echo '$(srcdir)/'`$(top_srcdir)/src/NetworkFunctions.cpp; \
	then mv -f "$(DEPDIR)/amuleweb-NetworkFunctions.Tpo" "$(DEPDIR)/amuleweb-NetworkFunctions.Po"; else rm -f "$(DEPDIR)/amuleweb-NetworkFunctions.Tpo"; exit 1; fi
amuleweb-NetworkFunctions.obj: $(top_srcdir)/src/NetworkFunctions.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-NetworkFunctions.obj -MD -MP -MF "$(DEPDIR)/amuleweb-NetworkFunctions.Tpo" -c -o amuleweb-NetworkFunctions.obj `if test -f '$(top_srcdir)/src/NetworkFunctions.cpp'; then $(CYGPATH_W) '$(top_srcdir)/src/NetworkFunctions.cpp'; else $(CYGPATH_W) '$(srcdir)/$(top_srcdir)/src/NetworkFunctions.cpp'; fi`; \
	then mv -f "$(DEPDIR)/amuleweb-NetworkFunctions.Tpo" "$(DEPDIR)/amuleweb-NetworkFunctions.Po"; else rm -f "$(DEPDIR)/amuleweb-NetworkFunctions.Tpo"; exit 1; fi
amuleweb-php_syntree.o: php_syntree.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-php_syntree.o -MD -MP -MF "$(DEPDIR)/amuleweb-php_syntree.Tpo" -c -o amuleweb-php_syntree.o `test -f 'php_syntree.cpp' || echo '$(srcdir)/'`php_syntree.cpp; \
	then mv -f "$(DEPDIR)/amuleweb-php_syntree.Tpo" "$(DEPDIR)/amuleweb-php_syntree.Po"; else rm -f "$(DEPDIR)/amuleweb-php_syntree.Tpo"; exit 1; fi
amuleweb-php_syntree.obj: php_syntree.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-php_syntree.obj -MD -MP -MF "$(DEPDIR)/amuleweb-php_syntree.Tpo" -c -o amuleweb-php_syntree.obj `if test -f 'php_syntree.cpp'; then $(CYGPATH_W) 'php_syntree.cpp'; else $(CYGPATH_W) '$(srcdir)/php_syntree.cpp'; fi`; \
	then mv -f "$(DEPDIR)/amuleweb-php_syntree.Tpo" "$(DEPDIR)/amuleweb-php_syntree.Po"; else rm -f "$(DEPDIR)/amuleweb-php_syntree.Tpo"; exit 1; fi
amuleweb-php_amule_lib.o: php_amule_lib.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-php_amule_lib.o -MD -MP -MF "$(DEPDIR)/amuleweb-php_amule_lib.Tpo" -c -o amuleweb-php_amule_lib.o `test -f 'php_amule_lib.cpp' || echo '$(srcdir)/'`php_amule_lib.cpp; \
	then mv -f "$(DEPDIR)/amuleweb-php_amule_lib.Tpo" "$(DEPDIR)/amuleweb-php_amule_lib.Po"; else rm -f "$(DEPDIR)/amuleweb-php_amule_lib.Tpo"; exit 1; fi
amuleweb-php_amule_lib.obj: php_amule_lib.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-php_amule_lib.obj -MD -MP -MF "$(DEPDIR)/amuleweb-php_amule_lib.Tpo" -c -o amuleweb-php_amule_lib.obj `if test -f 'php_amule_lib.cpp'; then $(CYGPATH_W) 'php_amule_lib.cpp'; else $(CYGPATH_W) '$(srcdir)/php_amule_lib.cpp'; fi`; \
	then mv -f "$(DEPDIR)/amuleweb-php_amule_lib.Tpo" "$(DEPDIR)/amuleweb-php_amule_lib.Po"; else rm -f "$(DEPDIR)/amuleweb-php_amule_lib.Tpo"; exit 1; fi
amuleweb-php_core_lib.o: php_core_lib.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-php_core_lib.o -MD -MP -MF "$(DEPDIR)/amuleweb-php_core_lib.Tpo" -c -o amuleweb-php_core_lib.o `test -f 'php_core_lib.cpp' || echo '$(srcdir)/'`php_core_lib.cpp; \
	then mv -f "$(DEPDIR)/amuleweb-php_core_lib.Tpo" "$(DEPDIR)/amuleweb-php_core_lib.Po"; else rm -f "$(DEPDIR)/amuleweb-php_core_lib.Tpo"; exit 1; fi
amuleweb-php_core_lib.obj: php_core_lib.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-php_core_lib.obj -MD -MP -MF "$(DEPDIR)/amuleweb-php_core_lib.Tpo" -c -o amuleweb-php_core_lib.obj `if test -f 'php_core_lib.cpp'; then $(CYGPATH_W) 'php_core_lib.cpp'; else $(CYGPATH_W) '$(srcdir)/php_core_lib.cpp'; fi`; \
	then mv -f "$(DEPDIR)/amuleweb-php_core_lib.Tpo" "$(DEPDIR)/amuleweb-php_core_lib.Po"; else rm -f "$(DEPDIR)/amuleweb-php_core_lib.Tpo"; exit 1; fi
amuleweb-UPnPBase.o: $(top_srcdir)/src/UPnPBase.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-UPnPBase.o -MD -MP -MF "$(DEPDIR)/amuleweb-UPnPBase.Tpo" -c -o amuleweb-UPnPBase.o `test -f '$(top_srcdir)/src/UPnPBase.cpp' || echo '$(srcdir)/'`$(top_srcdir)/src/UPnPBase.cpp; \
	then mv -f "$(DEPDIR)/amuleweb-UPnPBase.Tpo" "$(DEPDIR)/amuleweb-UPnPBase.Po"; else rm -f "$(DEPDIR)/amuleweb-UPnPBase.Tpo"; exit 1; fi
amuleweb-UPnPBase.obj: $(top_srcdir)/src/UPnPBase.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-UPnPBase.obj -MD -MP -MF "$(DEPDIR)/amuleweb-UPnPBase.Tpo" -c -o amuleweb-UPnPBase.obj `if test -f '$(top_srcdir)/src/UPnPBase.cpp'; then $(CYGPATH_W) '$(top_srcdir)/src/UPnPBase.cpp'; else $(CYGPATH_W) '$(srcdir)/$(top_srcdir)/src/UPnPBase.cpp'; fi`; \
	then mv -f "$(DEPDIR)/amuleweb-UPnPBase.Tpo" "$(DEPDIR)/amuleweb-UPnPBase.Po"; else rm -f "$(DEPDIR)/amuleweb-UPnPBase.Tpo"; exit 1; fi
amuleweb-UPnPCompatibility.o: UPnPCompatibility.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-UPnPCompatibility.o -MD -MP -MF "$(DEPDIR)/amuleweb-UPnPCompatibility.Tpo" -c -o amuleweb-UPnPCompatibility.o `test -f 'UPnPCompatibility.cpp' || echo '$(srcdir)/'`UPnPCompatibility.cpp; \
	then mv -f "$(DEPDIR)/amuleweb-UPnPCompatibility.Tpo" "$(DEPDIR)/amuleweb-UPnPCompatibility.Po"; else rm -f "$(DEPDIR)/amuleweb-UPnPCompatibility.Tpo"; exit 1; fi
amuleweb-UPnPCompatibility.obj: UPnPCompatibility.cpp
	echo "Compiling $(<F)"
	if $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(amuleweb_CPPFLAGS) $(CPPFLAGS) $(amuleweb_CXXFLAGS) $(CXXFLAGS) -MT amuleweb-UPnPCompatibility.obj -MD -MP -MF "$(DEPDIR)/amuleweb-UPnPCompatibility.Tpo" -c -o amuleweb-UPnPCompatibility.obj `if test -f 'UPnPCompatibility.cpp'; then $(CYGPATH_W) 'UPnPCompatibility.cpp'; else $(CYGPATH_W) '$(srcdir)/UPnPCompatibility.cpp'; fi`; \
	then mv -f "$(DEPDIR)/amuleweb-UPnPCompatibility.Tpo" "$(DEPDIR)/amuleweb-UPnPCompatibility.Po"; else rm -f "$(DEPDIR)/amuleweb-UPnPCompatibility.Tpo"; exit 1; fi
uninstall-info-am:
ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique
tags: TAGS
TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)  $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	if test -z "$(ETAGS_ARGS)$$tags$$unique"; then :; else \
	  test -n "$$unique" || unique=$$empty_fix; \
	  $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	    $$tags $$unique; \
	fi
ctags: CTAGS
CTAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)  $(LISP) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(CTAGS_ARGS)$$tags$$unique" \
	  || $(CTAGS) $(CTAGSFLAGS) $(AM_CTAGSFLAGS) $(CTAGS_ARGS) \
	     $$tags $$unique
GTAGS:
	here=`$(am__cd) $(top_builddir) && pwd` \
	  && cd $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) $$here
distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH tags
distdir: $(DISTFILES)
	@srcdirstrip=`echo "$(srcdir)" | sed 's|.|.|g'`; \
	topsrcdirstrip=`echo "$(top_srcdir)" | sed 's|.|.|g'`; \
	list='$(DISTFILES)'; for file in $$list; do \
	  case $$file in \
	    $(srcdir)/*) file=`echo "$$file" | sed "s|^$$srcdirstrip/||"`;; \
	    $(top_srcdir)/*) file=`echo "$$file" | sed "s|^$$topsrcdirstrip/|$(top_builddir)/|"`;; \
	  esac; \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	  if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	    dir="/$$dir"; \
	    $(mkdir_p) "$(distdir)$$dir"; \
	  else \
	    dir=''; \
	  fi; \
	  if test -d $$d/$$file; then \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -pR $(srcdir)/$$file $(distdir)$$dir || exit 1; \
	    fi; \
	    cp -pR $$d/$$file $(distdir)$$dir || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
check-am: all-am
check: check-am
all-am: Makefile $(PROGRAMS) $(HEADERS)
installdirs:
	for dir in "$(DESTDIR)$(bindir)"; do \
	  test -z "$$dir" || $(mkdir_p) "$$dir"; \
	done
install: install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am
install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
installcheck: installcheck-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	  install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	  `test -z '$(STRIP)' || \
	    echo "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'"` install
mostlyclean-generic:
clean-generic:
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)
distclean-generic:
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)
maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
	-test -z "$(MAINTAINERCLEANFILES)" || rm -f $(MAINTAINERCLEANFILES)
clean: clean-am
clean-am: clean-binPROGRAMS clean-generic mostlyclean-am
distclean: distclean-am
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
distclean-am: clean-am distclean-compile distclean-generic \
	distclean-tags
dvi: dvi-am
dvi-am:
html: html-am
info: info-am
info-am:
install-data-am:
install-exec-am: install-binPROGRAMS
install-info: install-info-am
install-man:
installcheck-am:
maintainer-clean: maintainer-clean-am
	-rm -rf ./$(DEPDIR)
	-rm -f Makefile
maintainer-clean-am: distclean-am maintainer-clean-generic
mostlyclean: mostlyclean-am
mostlyclean-am: mostlyclean-compile mostlyclean-generic
pdf: pdf-am
pdf-am:
ps: ps-am
ps-am:
uninstall-am: uninstall-binPROGRAMS uninstall-info-am
.PHONY: CTAGS GTAGS all all-am check check-am clean clean-binPROGRAMS \
	clean-generic ctags distclean distclean-compile \
	distclean-generic distclean-tags distdir dvi dvi-am html \
	html-am info info-am install install-am install-binPROGRAMS \
	install-data install-data-am install-exec install-exec-am \
	install-info install-info-am install-man install-strip \
	installcheck installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-compile \
	mostlyclean-generic pdf pdf-am ps ps-am tags uninstall \
	uninstall-am uninstall-binPROGRAMS uninstall-info-am
php_parser.c: php_parser.y
	bison --debug -t -d -v -o $@ $<
php_lexer.c: php_lexer.l
	$(LEX) -o$@ $<
.NOEXPORT:
