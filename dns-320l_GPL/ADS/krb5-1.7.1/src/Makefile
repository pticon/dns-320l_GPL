############################################################
## config/pre.in
## common prefix for all Makefile.in in the Kerberos V5 tree.
##

# These are set per-directory by autoconf 2.52 and 2.53:
#  srcdir=.
#  top_srcdir=.
# but these are only set by autoconf 2.53, and thus not useful to us on
# Mac OS X yet (as of 10.2):
#  abs_srcdir=/home/roy/DNS-345_GPL/packages/krb5-1.7.1/src
#  abs_top_srcdir=/home/roy/DNS-345_GPL/packages/krb5-1.7.1/src
#  builddir=.
#  abs_builddir=/home/roy/DNS-345_GPL/packages/krb5-1.7.1/src
#  top_builddir=
#  abs_top_builddir=/home/roy/DNS-345_GPL/packages/krb5-1.7.1/src
# The "top" variables refer to the directory with the configure (or
# config.status) script.

WHAT = unix
SHELL=/bin/sh

all:: all-$(WHAT)

clean:: clean-$(WHAT)

distclean:: distclean-$(WHAT)

install:: install-$(WHAT)

check:: check-$(WHAT)

install-headers:: install-headers-$(WHAT)

##############################
# Recursion rule support
#

# The commands for the recursion targets live in config/post.in.
#
# General form of recursion rules:
#
# Each recursive target foo-unix has related targets: foo-prerecurse,
# foo-recurse, and foo-postrecurse
#
# The foo-recurse rule is in post.in.  It is what actually recursively
# calls make.
#
# foo-recurse depends on foo-prerecurse, so any targets that must be
# built before descending into subdirectories must be dependencies of
# foo-prerecurse.
#
# foo-postrecurse depends on foo-recurse, but targets that must be
# built after descending into subdirectories should be have
# foo-recurse as dependencies in addition to being listed under
# foo-postrecurse, to avoid ordering issues.
#
# The foo-prerecurse, foo-recurse, and foo-postrecurse rules are all
# single-colon rules, to avoid nasty ordering problems with
# double-colon rules.
#
# e.g.
# all:: includes foo
# foo:
#	echo foo
# includes::
#	echo bar
# includes::
#	echo baz
#
# will result in "bar", "foo", "baz" on AIX, and possibly others.
all-unix:: all-postrecurse
all-postrecurse: all-recurse
all-recurse: all-prerecurse

all-prerecurse:
all-postrecurse:

clean-unix:: clean-postrecurse
clean-postrecurse: clean-recurse
clean-recurse: clean-prerecurse

clean-prerecurse:
clean-postrecurse:

distclean-unix: distclean-postrecurse
distclean-postrecurse: distclean-recurse
distclean-recurse: distclean-prerecurse

distclean-prerecurse:
distclean-postrecurse:

install-unix:: install-postrecurse
install-postrecurse: install-recurse
install-recurse: install-prerecurse

install-prerecurse:
install-postrecurse:

install-headers-unix:: install-headers-postrecurse
install-headers-postrecurse: install-headers-recurse
install-headers-recurse: install-headers-prerecurse

install-headers-prerecurse:
install-headers-postrecurse:

check-unix:: check-postrecurse
check-postrecurse: check-recurse
check-recurse: check-prerecurse

check-prerecurse:
check-postrecurse:

Makefiles: Makefiles-postrecurse
Makefiles-postrecurse: Makefiles-recurse
Makefiles-recurse: Makefiles-prerecurse

Makefiles-prerecurse:
Makefiles-postrecurse:

generate-files-mac: generate-files-mac-postrecurse
generate-files-mac-postrecurse: generate-files-mac-recurse
generate-files-mac-recurse: generate-files-mac-prerecurse
generate-files-mac-prerecurse:

#
# end recursion rule support
##############################

# Directory syntax:
#
# begin relative path
REL=
# this is magic... should only be used for preceding a program invocation
C=./
# "/" for UNIX, "\" for Windows; *sigh*
S=/

#
srcdir = .
SRCTOP = ./$(BUILDTOP)
top_srcdir = .

CONFIG_RELTOPDIR = .

FAKEDEST=$(BUILDTOP)/util/fakedest
FAKEPREFIX=$(FAKEDEST)/$(prefix)
FAKELIBDIR=$(FAKEPREFIX)/lib

# DEFS		set by configure
# DEFINES	set by local Makefile.in
# LOCALINCLUDES	set by local Makefile.in
# CPPFLAGS	user override
# CFLAGS	user override but starts off set by configure
# WARN_CFLAGS	user override but starts off set by configure
# PTHREAD_CFLAGS set by configure, not included in CFLAGS so that we
#		don't pull the pthreads library into shared libraries
ALL_CFLAGS = $(DEFS) $(DEFINES) $(KRB_INCLUDES) $(LOCALINCLUDES) \
	-DKRB5_DEPRECATED=1 \
	$(CPPFLAGS) $(CFLAGS) $(WARN_CFLAGS) $(PTHREAD_CFLAGS)
ALL_CXXFLAGS = $(DEFS) $(DEFINES) $(KRB_INCLUDES) $(LOCALINCLUDES) \
	-DKRB5_DEPRECATED=1 \
	$(CPPFLAGS) $(CXXFLAGS) $(WARN_CXXFLAGS) $(PTHREAD_CFLAGS)

CFLAGS = -DUSE_LINKER_FINI_OPTION
CXXFLAGS = -g -O2
WARN_CFLAGS =  -Wall -Wcast-qual -Wcast-align -Wshadow -Wmissing-prototypes -Wno-format-zero-length -Woverflow -Wstrict-overflow -Wmissing-format-attribute -Wmissing-prototypes -Wreturn-type -Wmissing-braces -Wparentheses -Wswitch -Wunused-function -Wunused-label -Wunused-variable -Wunused-value -Wunknown-pragmas -Wsign-compare -Werror=declaration-after-statement -Werror=variadic-macros
WARN_CXXFLAGS =  -Wall -Wcast-qual -Wcast-align -Wshadow
PTHREAD_CFLAGS = -pthread
PTHREAD_LIBS = 
THREAD_LINKOPTS = $(PTHREAD_CFLAGS) $(PTHREAD_LIBS)
CPPFLAGS = 
DEFS = -DHAVE_CONFIG_H
CC = arm-mv5sft-linux-gnueabi-gcc
CXX = arm-mv5sft-linux-gnueabi-g++
LD = $(PURE) arm-mv5sft-linux-gnueabi-ld
DEPLIBS = @DEPLIBS@
KRB_INCLUDES = -I$(BUILDTOP)/include -I$(SRCTOP)/include
LDFLAGS = 
LD_UNRESOLVED_PREFIX = @LD_UNRESOLVED_PREFIX@
LD_SHLIBDIR_PREFIX = @LD_SHLIBDIR_PREFIX@
LDARGS = @LDARGS@
LIBS = -lresolv 

INSTALL=/usr/bin/install -c
INSTALL_STRIP=
INSTALL_PROGRAM=${INSTALL} $(INSTALL_STRIP)
INSTALL_SCRIPT=${INSTALL}
INSTALL_DATA=${INSTALL} -m 644
INSTALL_SHLIB=$(INSTALL_DATA)
INSTALL_SETUID=$(INSTALL) $(INSTALL_STRIP) -m 4755 -o root
## This is needed because autoconf will sometimes define ${prefix} to be
## ${prefix}.
prefix=/home/roy/DNS-345_GPL/packages/krb5-1.7.1/src/_install
INSTALL_PREFIX=$(prefix)
INSTALL_EXEC_PREFIX=${prefix}
exec_prefix=${prefix}
datarootdir=@datarootdir@
SHLIB_TAIL_COMP=@SHLIB_TAIL_COMP@

datadir = ${prefix}/share
EXAMPLEDIR = $(datadir)/examples/krb5

KRB5MANROOT = ${prefix}/man
ADMIN_BINDIR = ${exec_prefix}/sbin
SERVER_BINDIR = ${exec_prefix}/sbin
CLIENT_BINDIR =${exec_prefix}/bin
ADMIN_MANDIR = $(KRB5MANROOT)/man8
SERVER_MANDIR = $(KRB5MANROOT)/man8
CLIENT_MANDIR = $(KRB5MANROOT)/man1
FILE_MANDIR = $(KRB5MANROOT)/man5
KRB5_LIBDIR = ${exec_prefix}/lib
KRB5_SHLIBDIR = ${exec_prefix}/lib$(SHLIB_TAIL_COMP)
KRB5_INCDIR = ${prefix}/include
MODULE_DIR = ${exec_prefix}/lib/krb5/plugins
KRB5_DB_MODULE_DIR = $(MODULE_DIR)/kdb
KRB5_PA_MODULE_DIR = $(MODULE_DIR)/preauth
KRB5_AD_MODULE_DIR = $(MODULE_DIR)/authdata
KRB5_LIBKRB5_MODULE_DIR = $(MODULE_DIR)/libkrb5
GSS_MODULE_DIR = ${exec_prefix}/lib/gss
KRB5_INCSUBDIRS = \
	$(KRB5_INCDIR)/kadm5 \
	$(KRB5_INCDIR)/krb5 \
	$(KRB5_INCDIR)/gssapi \
	$(KRB5_INCDIR)/gssrpc

#
# Macros used by the KADM5 (OV-based) unit test system.
# XXX check which of these are actually used!
#
TESTDIR		= $(BUILDTOP)/kadmin/testing
STESTDIR	= $(SRCTOP)/kadmin/testing
COMPARE_DUMP	= $(TESTDIR)/scripts/compare_dump.pl
FIX_CONF_FILES	= $(TESTDIR)/scripts/fixup-conf-files.pl
INITDB		= $(STESTDIR)/scripts/init_db
MAKE_KEYTAB	= $(TESTDIR)/scripts/make-host-keytab.pl
LOCAL_MAKE_KEYTAB= $(TESTDIR)/scripts/make-host-keytab.pl
RESTORE_FILES	= $(STESTDIR)/scripts/restore_files.sh
SAVE_FILES	= $(STESTDIR)/scripts/save_files.sh
ENV_SETUP	= $(TESTDIR)/scripts/env-setup.sh
CLNTTCL		= $(TESTDIR)/util/ovsec_kadm_clnt_tcl
SRVTCL		= $(TESTDIR)/util/ovsec_kadm_srv_tcl
# Dejagnu variables.
# We have to set the host with --host so that setup_xfail will work.
# If we don't set it, then the host type used is "native", which
# doesn't match "*-*-*".
host=arm-mv5sft-linux-gnueabi
DEJAFLAGS	= $(DEJALFLAGS) $(CLFLAGS) --debug --srcdir $(srcdir) --host \
		   $(host)
RUNTEST		= runtest $(DEJAFLAGS)

START_SERVERS	= $(STESTDIR)/scripts/start_servers $(TEST_SERVER) $(TEST_PATH)
START_SERVERS_LOCAL = $(STESTDIR)/scripts/start_servers_local

STOP_SERVERS	= $(STESTDIR)/scripts/stop_servers $(TEST_SERVER) $(TEST_PATH)
STOP_SERVERS_LOCAL = $(STESTDIR)/scripts/stop_servers_local
#
# End of macros for the KADM5 unit test system.
#

transform = s,x,x,

RM = rm -f
CP = cp
MV = mv -f
CHMOD=chmod
RANLIB = arm-mv5sft-linux-gnueabi-ranlib
ARCHIVE = ar cqv
ARADD = ar cruv
LN = ln -s
AWK = gawk
LEX = flex
LEXLIB = -lfl
YACC = bison -y
PERL = perl
AUTOCONF = autoconf
AUTOCONFFLAGS =
AUTOHEADER = autoheader
AUTOHEADERFLAGS =
MOVEIFCHANGED = $(SRCTOP)/config/move-if-changed

SHEXT = @SHEXT@
STEXT=@STEXT@
VEXT=@VEXT@

TOPLIBD = $(BUILDTOP)/lib

OBJEXT = o
LIBEXT = a
EXEEXT =

#
# variables for libraries, for use in linking programs
# -- this may want to get broken out into a separate frag later
#
#
# Note: the following variables must be set in any Makefile.in that
# uses KRB5_BUILD_PROGRAM
#
# PROG_LIBPATH	list of dirs, in -Ldir form, to search for libraries at link
# PROG_RPATH	list of dirs, in dir1:dir2 form, for rpath purposes
#
# invocation is like:
# prog: foo.o bar.o $(KRB5_BASE_DEPLIBS)
# 	$(CC_LINK) -o $@ foo.o bar.o $(KRB5_BASE_LIBS)


CC_LINK=$(CC) $(PROG_LIBPATH) $(PROG_RPATH_FLAGS) $(CFLAGS) $(LDFLAGS)
CXX_LINK=$(CXX) $(PROG_LIBPATH) $(PROG_RPATH_FLAGS) $(CXXFLAGS) $(LDFLAGS)

# prefix (with no spaces after) for rpath flag to cc
RPATH_FLAG=-Wl,-rpath -Wl,

# link flags to add PROG_RPATH to the rpath
PROG_RPATH_FLAGS=$(RPATH_FLAG)$(PROG_RPATH)

# this gets set by configure to either $(STLIBEXT) or $(SHLIBEXT),
# depending on whether we're building with shared libraries.
DEPLIBEXT=.so

KADMCLNT_DEPLIB	= $(TOPLIBD)/libkadm5clnt$(DEPLIBEXT)
KADMSRV_DEPLIB	= $(TOPLIBD)/libkadm5srv$(DEPLIBEXT)
KDB5_DEPLIB	= $(TOPLIBD)/libkdb5$(DEPLIBEXT)
GSSRPC_DEPLIB	= $(TOPLIBD)/libgssrpc$(DEPLIBEXT)
GSS_DEPLIB	= $(TOPLIBD)/libgssapi_krb5$(DEPLIBEXT)
KRB5_DEPLIB	= $(TOPLIBD)/libkrb5$(DEPLIBEXT)
CRYPTO_DEPLIB	= $(TOPLIBD)/libk5crypto$(DEPLIBEXT)
COM_ERR_DEPLIB	= $(COM_ERR_DEPLIB-k5)
COM_ERR_DEPLIB-sys = # empty
COM_ERR_DEPLIB-k5 = $(TOPLIBD)/libcom_err$(DEPLIBEXT)
SUPPORT_LIBNAME=krb5support
SUPPORT_DEPLIB	= $(TOPLIBD)/lib$(SUPPORT_LIBNAME)$(DEPLIBEXT)

# These are forced to use ".a" as an extension because they're never
# built shared.
SS_DEPLIB	= $(SS_DEPLIB-k5)
SS_DEPLIB-k5	= $(TOPLIBD)/libss.a
SS_DEPLIB-sys	=
PTY_DEPLIB	= $(TOPLIBD)/libpty.a
APPUTILS_DEPLIB	= $(TOPLIBD)/libapputils.a

KRB5_BASE_DEPLIBS	= $(KRB5_DEPLIB) $(CRYPTO_DEPLIB) $(COM_ERR_DEPLIB) $(SUPPORT_DEPLIB)
KDB5_DEPLIBS		= $(KDB5_DEPLIB)
GSS_DEPLIBS		= $(GSS_DEPLIB)
GSSRPC_DEPLIBS		= $(GSSRPC_DEPLIB) $(GSS_DEPLIBS)
KADM_COMM_DEPLIBS	= $(GSSRPC_DEPLIBS) $(KDB5_DEPLIBS) $(GSSRPC_DEPLIBS)
KADMSRV_DEPLIBS		= $(KADMSRV_DEPLIB) $(KDB5_DEPLIBS) $(KADM_COMM_DEPLIBS)
KADMCLNT_DEPLIBS	= $(KADMCLNT_DEPLIB) $(KADM_COMM_DEPLIBS)

# Header file dependencies we might override.
# See util/depfix.sed.
# Also see depend-verify-* in post.in, which wants to confirm that we're using
# the in-tree versions.
COM_ERR_VERSION = k5
COM_ERR_DEPS	= $(COM_ERR_DEPS-k5)
COM_ERR_DEPS-sys =
COM_ERR_DEPS-k5	= $(BUILDTOP)/include/com_err.h
SS_VERSION	= k5
SS_DEPS		= $(SS_DEPS-k5)
SS_DEPS-sys	=
SS_DEPS-k5	= $(BUILDTOP)/include/ss/ss.h $(BUILDTOP)/include/ss/ss_err.h

# LIBS gets substituted in... e.g. -lnsl -lsocket

# GEN_LIB is -lgen if needed for regexp
GEN_LIB		= 

SS_LIB		= $(SS_LIB-k5)
SS_LIB-sys	= 
SS_LIB-k5	= $(TOPLIBD)/libss.a
KDB5_LIB	= -lkdb5

DL_LIB		= -ldl

KRB5_LIB			= -lkrb5
K5CRYPTO_LIB			= -lk5crypto
COM_ERR_LIB			= -lcom_err
GSS_KRB5_LIB			= -lgssapi_krb5
SUPPORT_LIB			= -l$(SUPPORT_LIBNAME)

# HESIOD_LIBS is -lhesiod...
HESIOD_LIBS	= 

KRB5_BASE_LIBS	= $(KRB5_LIB) $(K5CRYPTO_LIB) $(COM_ERR_LIB) $(SUPPORT_LIB) $(GEN_LIB) $(LIBS) $(DL_LIB)
KDB5_LIBS	= $(KDB5_LIB) $(GSSRPC_LIBS)
GSS_LIBS	= $(GSS_KRB5_LIB)
# needs fixing if ever used on Mac OS X!
GSSRPC_LIBS	= -lgssrpc $(GSS_LIBS)
KADM_COMM_LIBS	= $(GSSRPC_LIBS)
# need fixing if ever used on Mac OS X!
KADMSRV_LIBS	= -lkadm5srv $(HESIOD_LIBS) $(KDB5_LIBS) $(KADM_COMM_LIBS)
KADMCLNT_LIBS	= -lkadm5clnt $(KADM_COMM_LIBS)

# need fixing if ever used on Mac OS X!
PTY_LIB		= -lpty

# libutil for NetBSD, et al. for openpty(), etc.
UTIL_LIB	= @UTIL_LIB@

# Misc stuff for linking server programs (and maybe some others,
# eventually) but which we don't want to install.
APPUTILS_LIB	= -lapputils

#
# variables for --with-tcl=
TCL_LIBS	= 
TCL_LIBPATH	= 
TCL_RPATH	= 
TCL_MAYBE_RPATH = 
TCL_INCLUDES	= 

# error table rules
#
### /* these are invoked as $(...) foo.et, which works, but could be better */
COMPILE_ET= $(COMPILE_ET-k5)
COMPILE_ET-sys= compile_et
COMPILE_ET-k5= $(BUILDTOP)/util/et/compile_et -d $(SRCTOP)/util/et

.SUFFIXES:  .h .c .et .ct

# These versions cause both .c and .h files to be generated at once.
# But GNU make doesn't understand this, and parallel builds can trigger
# both of them at once, causing them to stomp on each other.  The versions
# below only update one of the files, so compile_et has to get run twice,
# but it won't break parallel builds.
#.et.h: ; $(COMPILE_ET) $<
#.et.c: ; $(COMPILE_ET) $<

.et.h:
	$(RM) et-h-$*.et et-h-$*.c et-h-$*.h
	$(CP) $< et-h-$*.et
	$(COMPILE_ET) et-h-$*.et
	$(MV) et-h-$*.h $*.h
	$(RM) et-h-$*.et et-h-$*.c
.et.c:
	$(RM) et-c-$*.et et-c-$*.c et-c-$*.h
	$(CP) $< et-c-$*.et
	$(COMPILE_ET) et-c-$*.et
	$(MV) et-c-$*.c $*.c
	$(RM) et-c-$*.et et-c-$*.h

# rule to make object files
#
.SUFFIXES: .cpp .c .o
.c.o:
	$(CC) $(ALL_CFLAGS) -c $<
# Use .cpp because that's what autoconf uses in its test.
# If the compiler doesn't accept a .cpp suffix here, it wouldn't
# have accepted it when autoconf tested it.
.cpp.o:
	$(CXX) $(ALL_CXXFLAGS) -c $<

# ss command table rules
#
MAKE_COMMANDS= $(MAKE_COMMANDS-k5)
MAKE_COMMANDS-sys= mk_cmds
MAKE_COMMANDS-k5= $(BUILDTOP)/util/ss/mk_cmds

.ct.c:
	$(MAKE_COMMANDS) $<

## Parameters to be set by configure for use in lib.in:
##
#
# These settings are for building shared libraries only.  Including
# libpriv.in will override with values appropriate for static
# libraries that we don't install.  Some values will depend on whether
# the platform supports major and minor version number extensions on
# shared libraries, hence the FOO_@@ settings.

LN_S=ln -s
AR=ar

# Set to "lib$(LIBBASE)$(STEXT) lib$(LIBBASE)$(SHEXT) lib$(LIBBASE)$(PFEXT)" or
# some subset thereof by configure; determines which types of libs get
# built.
LIBLIST=$(LIBLIST_yes)
LIBLIST_yes=lib$(LIBBASE)$(SHLIBEXT) lib$(LIBBASE)$(SHLIBSEXT)
LIBLIST_no=lib$(LIBBASE)$(SHLIBEXT)

# Set by configure; list of library symlinks to make to $(TOPLIBD)
LIBLINKS=$(LIBLINKS_yes)
LIBLINKS_yes=$(TOPLIBD)/lib$(LIBBASE)$(SHLIBEXT) $(TOPLIBD)/lib$(LIBBASE)$(SHLIBVEXT) $(TOPLIBD)/lib$(LIBBASE)$(SHLIBSEXT)
LIBLINKS_no=$(TOPLIBD)/lib$(LIBBASE)$(SHLIBEXT) $(TOPLIBD)/lib$(LIBBASE)$(SHLIBVEXT)

# Set by configure; list of install targets
LIBINSTLIST=$(LIBINSTLIST_yes)
LIBINSTLIST_yes=install-shlib-soname
LIBINSTLIST_no=install-shared

# Some of these should really move to pre.in, since programs will need
# it too. (e.g. stuff that has dependencies on the libraries)

# usually .a
STLIBEXT_if_static=.a
STLIBEXT=.a-nobuild

# usually .so.$(LIBMAJOR).$(LIBMINOR)
SHLIBVEXT=.so.$(LIBMAJOR).$(LIBMINOR)

# usually .so.$(LIBMAJOR) (to allow for major-version compat)
SHLIBSEXT=.so.$(LIBMAJOR)

# usually .so
SHLIBEXT=.so

# usually _p.a
PFLIBEXT=_p.a

#
DYNOBJEXT=$(SHLIBEXT)
MAKE_DYNOBJ_COMMAND=$(MAKE_SHLIB_COMMAND)
DYNOBJ_EXPDEPS=$(SHLIB_EXPDEPS)
DYNOBJ_EXPFLAGS=$(SHLIB_EXPFLAGS)

# File with symbol names to be exported, both functions and data,
# currently not distinguished.
SHLIB_EXPORT_FILE=$(srcdir)/$(LIBPREFIX)$(LIBBASE).exports

# File that needs to be current for building the shared library,
# usually SHLIB_EXPORT_FILE, but not always, if we have to convert
# it to another, intermediate form for the linker.
SHLIB_EXPORT_FILE_DEP=binutils.versions

# Command to run to build a shared library.
# In systems that require multiple commands, like AIX, it may need
# to change to rearrange where the various parameters fit in.
MAKE_SHLIB_COMMAND=$(CC) -shared -fPIC -Wl,-h,$(LIBPREFIX)$(LIBBASE)$(SHLIBSEXT),--no-undefined $(LDFLAGS) -o $@ $$objlist $(SHLIB_EXPFLAGS) -Wl,--version-script binutils.versions && $(PERL) -w $(SRCTOP)/util/export-check.pl $(SHLIB_EXPORT_FILE) $@

# run path flags for explicit libraries depending on this one,
# e.g. "-R$(SHLIB_RPATH)"
SHLIB_RPATH_FLAGS=-Wl,-R$(SHLIB_RDIRS)

# flags for explicit libraries depending on this one,
# e.g. "$(SHLIB_RPATH_FLAGS) $(SHLIB_SHLIB_DIRFLAGS) $(SHLIB_EXPLIBS)"
SHLIB_EXPFLAGS=$(SHLIB_RPATH_FLAGS) $(SHLIB_DIRS) $(SHLIB_EXPLIBS)

## Parameters to be set by configure for use in libobj.in:

# Set to "OBJS.ST OBJS.SH OBJS.PF" or some subset thereof by
# configure; determines which types of object files get built.
OBJLISTS=OBJS.SH

# Note that $(LIBSRCS) *cannot* contain any variable references, or
# the suffix substitution will break on some platforms!
SHLIBOBJS=$(STLIBOBJS:.o=.so)
PFLIBOBJS=$(STLIBOBJS:.o=.po)

# "$(CC) -G", "$(LD) -Bshareable", etc.
LDCOMBINE=@LDCOMBINE@

#
# rules to make various types of object files
#
PICFLAGS=-fPIC
PROFFLAGS=-pg

# platform-dependent temporary files that should get cleaned up
EXTRA_FILES=

VALGRIND=
# Need absolute paths here because under kshd or ftpd we may run programs
# while in other directories.
VALGRIND_LOGDIR = `cd $(BUILDTOP)&&pwd`
VALGRIND1 = valgrind --tool=memcheck --log-file=$(VALGRIND_LOGDIR)/vg --trace-children=yes -v --leak-check=yes --suppressions=`cd $(SRCTOP)&&pwd`/util/valgrind-suppressions

##
## end of pre.in
############################################################
datadir=${prefix}/share

thisconfigdir=.
myfulldir=.
mydir=.
# Don't build sample by default, and definitely don't install them
# for production use:
#	plugins/locate/python
#	plugins/preauth/wpse
#	plugins/preauth/cksum_body
#	plugins/authdata/greet
SUBDIRS=util include lib kdc kadmin  slave clients \
	plugins/kdb/db2 \
	plugins/preauth/pkinit \
	plugins/preauth/encrypted_challenge \
	appl tests \
	config-files gen-manpages
BUILDTOP=$(REL)$(C)
LOCALINCLUDES = -I$(srcdir) 

SRCS =  
HDRS = 

# Why aren't these flags showing up in Windows builds?
##DOS##CPPFLAGS=$(CPPFLAGS) -D_X86_=1  -DWIN32 -D_WIN32 -W3 -D_WINNT

DISTFILES = $(SRCS) $(HDRS) COPYING COPYING.LIB ChangeLog Makefile.in

all-unix:: krb5-config

# Lots of things will start to depend on the thread support, which
# needs autoconf.h, but building "all" in include requires that util/et
# have been built first.  Until we can untangle this, let's just check
# that autoconf.h is up to date before going into any of the subdirectories.
all-prerecurse generate-files-mac-prerecurse: update-autoconf-h
update-autoconf-h:
	(cd include && $(MAKE) autoconf.h osconf.h)

##DOS##!if 0
# This makefile doesn't use lib.in, but we still need shlib.conf here.
config.status: $(SRCTOP)/config/shlib.conf
##DOS##!endif

all-windows:: maybe-awk Makefile-windows
	@echo Making autoconf.h in include
	cd include
	$(MAKE) -$(MFLAGS) autoconf.h

	@echo Making in ccapi
	cd ..\ccapi
	cd
	$(MAKE) -$(MFLAGS)

	@echo Making in util
	cd ..\util
	$(MAKE) -$(MFLAGS)
	@echo Making in include
	cd ..\include
	$(MAKE) -$(MFLAGS) 
	@echo Making in lib
	cd ..\lib
	$(MAKE) -$(MFLAGS) 
	@echo Making in windows
	cd ..\windows
	$(MAKE) -$(MFLAGS) 
	@echo Making in clients
	cd ..\clients
	$(MAKE) -$(MFLAGS)
	@echo Making in appl\gss-sample
	cd ..\appl\gss-sample
	$(MAKE) -$(MFLAGS)
	@echo Making in appl\gssftp\ftp
	cd ..\..\appl\gssftp\ftp
	$(MAKE) -$(MFLAGS)
	cd ..\..\..

world::
	date
	make $(MFLAGS) all
	date

INSTALLMKDIRS = $(KRB5ROOT) $(KRB5MANROOT) $(KRB5OTHERMKDIRS) \
		$(ADMIN_BINDIR) $(SERVER_BINDIR) $(CLIENT_BINDIR) \
		$(ADMIN_MANDIR) $(SERVER_MANDIR) $(CLIENT_MANDIR) \
		$(FILE_MANDIR) $(KRB5_LIBDIR) $(KRB5_INCDIR) \
		$(KRB5_DB_MODULE_DIR) $(KRB5_PA_MODULE_DIR) \
		$(KRB5_AD_MODULE_DIR) \
		$(KRB5_LIBKRB5_MODULE_DIR) \
		${prefix}/var ${prefix}/var/krb5kdc \
		$(KRB5_INCSUBDIRS) $(datadir) $(EXAMPLEDIR)

install-strip:
	$(MAKE) install INSTALL_STRIP=-s

install-recurse: install-mkdirs

install-mkdirs:
	@for i in $(INSTALLMKDIRS); do \
		$(srcdir)/config/mkinstalldirs $(DESTDIR)$$i; \
	done

install-headers-mkdirs:
	$(srcdir)/config/mkinstalldirs $(DESTDIR)$(KRB5_INCDIR)
	$(srcdir)/config/mkinstalldirs $(DESTDIR)$(KRB5_INCDIR)/gssapi
	$(srcdir)/config/mkinstalldirs $(DESTDIR)$(KRB5_INCDIR)/gssrpc
install-headers-prerecurse: install-headers-mkdirs

# install::
#	$(MAKE) $(MFLAGS) install.man

fake-install:
	$(RM) -r $(FAKEPREFIX)
	@for i in $(INSTALLMKDIRS); do \
		$(srcdir)/config/mkinstalldirs util/fakedest$$i; \
	done
	(w=`pwd`; cd util && $(MAKE) install DESTDIR="$$w/util/fakedest")
	(w=`pwd`; cd lib && $(MAKE) install DESTDIR="$$w/util/fakedest")
	(w=`pwd`; cd plugins/kdb/db2 && $(MAKE) install DESTDIR="$$w/util/fakedest")
	if test -r plugins/preauth/pkinit/Makefile; then \
	  (w=`pwd`; cd plugins/preauth/pkinit && $(MAKE) install DESTDIR="$$w/util/fakedest"); \
	fi

#	(w=`pwd`; cd plugins/locate/python && $(MAKE) install DESTDIR="$$w/util/fakedest")

TAGS: $(SRCS)
	etags $(SRCS)

clean-:: clean-windows
clean-unix::
	$(RM) *.o core krb5-config
	$(RM) -r util/fakedest

mostlyclean: clean

# This doesn't work; if you think you need it, you should use a
# separate build directory.
# 
# distclean: clean
# 	rm -f Makefile config.status
# 
# realclean: distclean
# 	rm -f TAGS

dist: $(DISTFILES)
	echo cpio-`sed -e '/version_string/!d' \
	-e 's/[^0-9.]*\([0-9.]*\).*/\1/' -e q version.c` > .fname
	rm -rf `cat .fname`
	mkdir `cat .fname`
	-ln $(DISTFILES) `cat .fname`
	for file in $(DISTFILES); do \
	  test -r `cat .fname`/$$file || cp -p $$file `cat .fname`; \
	done
	tar chzf `cat .fname`.tar.gz `cat .fname`
	rm -rf `cat .fname` .fname

GZIPPROG= gzip -9v
PKGDIR=`pwd`/pkgdir
pkgdir:
	if test ! -d $(PKGDIR); then mkdir $(PKGDIR); else true; fi
tgz-bin: pkgdir
	rm -rf $(PKGDIR)/install cns5-bin.tgz
	mkdir $(PKGDIR)/install
	$(MAKE) install DESTDIR=$(PKGDIR)/install
	(cd $(PKGDIR)/install && tar cf - usr/cygnus) | $(GZIPPROG) > cns5-bin.tgz
	rm -rf $(PKGDIR)/install

# Microsoft Windows build process...
#

config-windows:: Makefile-windows
#	@echo Making in include
#	cd include
#	$(MAKE) -$(MFLAGS)
#	cd ..

#
# We need outpre-dir explicitly in here because we may
# try to build wconfig on a config-windows.
#
##DOS##$(WCONFIG_EXE): outpre-dir wconfig.c
##DOS##	$(CC) -Fe$@ -Fo$*.obj wconfig.c $(CCLINKOPTION)
##DOS## $(_VC_MANIFEST_EMBED_EXE)

##DOS##MKFDEP=$(WCONFIG_EXE) config\win-pre.in config\win-post.in

WINMAKEFILES=Makefile \
	appl\gss-sample\Makefile \
	appl\gssftp\ftp\Makefile \
	ccapi\Makefile \
	ccapi\lib\win\Makefile \
	ccapi\server\win\Makefile \
    ccapi\test\Makefile \
	clients\Makefile clients\kdestroy\Makefile \
	clients\kinit\Makefile clients\klist\Makefile \
	clients\kpasswd\Makefile clients\kvno\Makefile \
	clients\kcpytkt\Makefile clients\kdeltkt\Makefile \
	include\Makefile \
	lib\Makefile lib\crypto\Makefile \
	lib\crypto\crc32\Makefile lib\crypto\des\Makefile \
	lib\crypto\dk\Makefile lib\crypto\enc_provider\Makefile \
	lib\crypto\hash_provider\Makefile \
	lib\crypto\keyhash_provider\Makefile \
	lib\crypto\raw\Makefile lib\crypto\old\Makefile \
	lib\crypto\sha1\Makefile lib\crypto\arcfour\Makefile \
	lib\crypto\md4\Makefile lib\crypto\md5\Makefile \
	lib\crypto\yarrow\Makefile lib\crypto\aes\Makefile \
	lib\gssapi\Makefile lib\gssapi\generic\Makefile \
	lib\gssapi\krb5\Makefile lib\gssapi\mechglue\Makefile \
	lib\gssapi\spnego\Makefile \
	lib\krb5\Makefile \
	lib\krb5\asn.1\Makefile lib\krb5\ccache\Makefile \
	lib\krb5\ccache\ccapi\Makefile \
	lib\krb5\error_tables\Makefile \
	lib\krb5\keytab\Makefile \
	lib\krb5\krb\Makefile \
	lib\krb5\os\Makefile lib\krb5\posix\Makefile \
	lib\krb5\rcache\Makefile \
	util\Makefile \
	util\et\Makefile util\profile\Makefile \
	util\support\Makefile \
	util\windows\Makefile \
	windows\Makefile windows\lib\Makefile \
	windows\cns\Makefile windows\gina\Makefile \
	windows\gss\Makefile windows\ms2mit\Makefile \
	windows\wintel\Makefile windows\kfwlogon\Makefile

##DOS##Makefile-windows:: $(MKFDEP) $(WINMAKEFILES)

##DOS##Makefile: Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##appl\gss-sample\Makefile: appl\gss-sample\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##appl\gssftp\ftp\Makefile: appl\gssftp\ftp\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##ccapi\Makefile: ccapi\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##ccapi\lib\win\Makefile: ccapi\lib\win\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##ccapi\server\win\Makefile: ccapi\server\win\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##ccapi\test\Makefile: ccapi\test\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##clients\Makefile: clients\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##clients\kdestroy\Makefile: clients\kdestroy\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##clients\kinit\Makefile: clients\kinit\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##clients\klist\Makefile: clients\klist\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##clients\kpasswd\Makefile: clients\kpasswd\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##clients\kvno\Makefile: clients\kvno\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##clients\kcpytkt\Makefile: clients\kcpytkt\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##clients\kdeltkt\Makefile: clients\kdeltkt\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##include\Makefile: include\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\Makefile: lib\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\Makefile: lib\crypto\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\crc32\Makefile: lib\crypto\crc32\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\des\Makefile: lib\crypto\des\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\dk\Makefile: lib\crypto\dk\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\enc_provider\Makefile: lib\crypto\enc_provider\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\hash_provider\Makefile: lib\crypto\hash_provider\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\keyhash_provider\Makefile: lib\crypto\keyhash_provider\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\sha1\Makefile: lib\crypto\sha1\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\arcfour\Makefile: lib\crypto\arcfour\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\md4\Makefile: lib\crypto\md4\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\md5\Makefile: lib\crypto\md5\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\yarrow\Makefile: lib\crypto\yarrow\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\aes\Makefile: lib\crypto\aes\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\old\Makefile: lib\crypto\old\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\crypto\raw\Makefile: lib\crypto\raw\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\gssapi\Makefile: lib\gssapi\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\gssapi\generic\Makefile: lib\gssapi\generic\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\gssapi\mechglue\Makefile: lib\gssapi\mechglue\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\gssapi\krb5\Makefile: lib\gssapi\krb5\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\krb5\Makefile: lib\krb5\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\krb5\asn.1\Makefile: lib\krb5\asn.1\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\krb5\ccache\Makefile: lib\krb5\ccache\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\krb5\ccache\ccapi\Makefile: lib\krb5\ccache\ccapi\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\krb5\error_tables\Makefile: lib\krb5\error_tables\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\krb5\keytab\Makefile: $$@.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\krb5\krb\Makefile: lib\krb5\krb\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\krb5\os\Makefile: lib\krb5\os\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\krb5\posix\Makefile: lib\krb5\posix\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##lib\krb5\rcache\Makefile: lib\krb5\rcache\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##util\Makefile: util\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##util\et\Makefile: util\et\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##util\profile\Makefile: util\profile\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##util\support\Makefile: util\support\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##util\windows\Makefile: util\windows\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##windows\Makefile: windows\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##windows\lib\Makefile: windows\lib\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##windows\cns\Makefile: windows\cns\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##windows\gina\Makefile: windows\gina\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##windows\gss\Makefile: windows\gss\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##windows\ms2mit\Makefile: windows\ms2mit\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##windows\wintel\Makefile: windows\wintel\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@
##DOS##windows\kfwlogon\Makefile: windows\kfwlogon\Makefile.in $(MKFDEP)
##DOS##	$(WCONFIG) config < $@.in > $@

clean-windows:: Makefile-windows
	@echo Making clean in util
	cd util
	$(MAKE) -$(MFLAGS) clean
	@echo Making clean in include
	cd ..\include
	$(MAKE) -$(MFLAGS) clean
	@echo Making clean in lib
	cd ..\lib
	$(MAKE) -$(MFLAGS) clean
	@echo Making clean in windows
	cd ..\windows
	$(MAKE) -$(MFLAGS) clean
	@echo Making clean in clients
	cd ..\clients
	$(MAKE) -$(MFLAGS) clean
	@echo Making clean in appl\gss-sample
	cd ..\appl\gss-sample
	$(MAKE) -$(MFLAGS) clean
	@echo Making clean in appl\gssftp\ftp
	cd ..\..\appl\gssftp\ftp
	$(MAKE) -$(MFLAGS) clean
	cd ..\..\..
	@echo Making clean in root

#
# Renames DOS 8.3 filenames back to their proper, longer names.
#
ren2long:
	-sh config/ren2long

#
# Builds the file that distributes Kerberos sources for DOS and 
# Macintosh sites from the source tree on Unix.
#
ZIP=zip
FILES= ./* \
    clients/* clients/kdestroy/* clients/kinit/* clients/klist/* \
	clients/kpasswd/* clients/kcpytkt/* clients/kdeltkt/* \
	config/* include/* include/kerberosIV/* \
	include/krb5/* include/krb5/stock/* include/sys/* lib/* \
	lib/crypto/* lib/crypto/crc32/* lib/crypto/des/* lib/crypto/dk/* \
	lib/crypto/enc_provider/* lib/crypto/hash_provider/* \
	lib/crypto/keyhash_provider/* lib/crypto/old/* lib/crypto/raw/* \
	lib/crypto/sha1/* lib/crypto/arcfour/* lib/crypto/md4/* \
	lib/crypto/md5/* lib/crypto/yarrow/* \
	lib/gssapi/* lib/gssapi/generic/* lib/gssapi/krb5/* \
	lib/gssapi/mechglue/* lib/gssapi/spnego/* \
	lib/krb5/* lib/krb5/asn.1/* lib/krb5/krb/* \
	lib/krb5/ccache/* lib/krb5/ccache/ccapi/* \
	lib/krb5/error_tables/* \
	lib/krb5/keytab/* lib/krb5/keytab/file/* lib/krb5/keytab/srvtab/* \
	lib/krb5/os/* lib/krb5/posix/* lib/krb5/rcache/* \
	util/* util/et/* util/profile/*

WINFILES= \
    ccapi/common/* ccapi/common/win/* ccapi/common/win/OldCC/* \
    ccapi/lib/* ccapi/lib/win/* ccapi/lib/win/OldCC/* \
    ccapi/server/* ccapi/server/win/* \
    ccapi\test\* \
    util/windows/* windows/* windows/lib/* windows/cns/* \
	windows/wintel/* windows/gss/* windows/gina/* windows/ms2mit/* \
	windows/kfwlogon/*

WINBINARYFILES=	windows/*/*.ico windows/*/*.doc windows/*/*.hlp \
	windows/*/*.hpj windows/lib/*.lib

#
# Part of building the PC release has to be done on Unix. This includes
# anything the requires awk.
#
AWK = awk
AH  = util/et/et_h.awk
AC  = util/et/et_c.awk
INC = include/
ET  = lib/krb5/error_tables/
GG  = lib/gssapi/generic/
GK  = lib/gssapi/krb5/
PR  = util/profile/
CE  = util/et/

ETOUT =	\
	$(INC)asn1_err.h $(ET)asn1_err.c \
	$(INC)kdb5_err.h $(ET)kdb5_err.c \
	$(INC)krb5_err.h $(ET)krb5_err.c \
	$(INC)kv5m_err.h $(ET)kv5m_err.c \
	$(INC)krb524_err.h $(ET)krb524_err.c \
	$(PR)prof_err.h $(PR)prof_err.c \
	$(GG)gssapi_err_generic.h $(GG)gssapi_err_generic.c \
	$(GK)gssapi_err_krb5.h $(GK)gssapi_err_krb5.c

HOUT =	$(INC)krb5\krb5.h $(GG)gssapi.h $(PR)profile.h

CLEANUP= Makefile $(ETOUT) $(HOUT) \
	include/profile.h include/osconf.h \
	winfile.list


kerbsrc.win: kerbsrc.zip

winfile.list:
	echo $(FILES) $(WINFILES) | tr ' ' \\012 | \
		sed -f config/winexclude.sed > winfile.list

dos-Makefile:
	cat config/win-pre.in Makefile.in config/win-post.in | \
		sed -e "s/^##DOS##//" -e "s/^##DOS//" > Makefile.tmp
	mv Makefile.tmp Makefile

prep-windows: dos-Makefile awk-windows-mac

krbsrc83.zip: krbsrc83-is-obsolete

krbsrc83-is-obsolete:
	@echo "Win16 and krbsrc83.zip are no longer supported."
	@echo "We don't support building under 8.3 restricted filesystems"
	@echo "anymore.  You can still build for Win32 on filesystems"
	@echo "without 8.3 restrictions using kerbsrc.zip"
	@echo " "

kerbsrc.zip: dos-Makefile awk-windows-mac winfile.list
	rm -f kerbsrc.zip
	$(ZIP) -@Dl kerbsrc.zip < winfile.list
	$(ZIP) -D kerbsrc.zip $(WINBINARYFILES)
	rm -f $(CLEANUP)

kerbsrc-nt.zip: kerbsrc-nt-is-obsolete

kerbsrc-nt-is-obsolete:
	@echo "kerbsrc-nt.zip is now obsolete.  Just use and build kerbsrc.zip"
	@echo "We don't support building under 8.3 restricted filesystems"
	@echo "anymore, so what was kerbsrc-nt.zip is now kerbsrc.zip."
	@echo " "

$(INC)asn1_err.h: $(AH) $(ET)asn1_err.et
	$(AWK) -f $(AH) outfile=$@ $(ET)asn1_err.et
$(INC)kdb5_err.h: $(AH) $(ET)kdb5_err.et
	$(AWK) -f $(AH) outfile=$@ $(ET)kdb5_err.et
$(INC)krb5_err.h: $(AH) $(ET)krb5_err.et
	$(AWK) -f $(AH) outfile=$@ $(ET)krb5_err.et
$(INC)kv5m_err.h: $(AH) $(ET)kv5m_err.et
	$(AWK) -f $(AH) outfile=$@ $(ET)kv5m_err.et
$(INC)krb524_err.h: $(AH) $(ET)krb524_err.et
	$(AWK) -f $(AH) outfile=$@ $(ET)krb524_err.et
$(PR)prof_err.h: $(AH) $(PR)prof_err.et
	$(AWK) -f $(AH) outfile=$@ $(PR)prof_err.et
$(GG)gssapi_err_generic.h: $(AH) $(GG)gssapi_err_generic.et
	$(AWK) -f $(AH) outfile=$@ $(GG)gssapi_err_generic.et
$(GK)gssapi_err_krb5.h: $(AH) $(GK)gssapi_err_krb5.et
	$(AWK) -f $(AH) outfile=$@ $(GK)gssapi_err_krb5.et
$(CE)test1.h: $(AH) $(CE)test1.et
	$(AWK) -f $(AH) outfile=$@ $(CE)test1.et
$(CE)test2.h: $(AH) $(CE)test2.et
	$(AWK) -f $(AH) outfile=$@ $(CE)test2.et

$(ET)asn1_err.c: $(AC) $(ET)asn1_err.et
	$(AWK) -f $(AC) outfile=$@ $(ET)asn1_err.et
$(ET)kdb5_err.c: $(AC) $(ET)kdb5_err.et
	$(AWK) -f $(AC) outfile=$@ $(ET)kdb5_err.et
$(ET)krb5_err.c: $(AC) $(ET)krb5_err.et
	$(AWK) -f $(AC) outfile=$@ $(ET)krb5_err.et
$(ET)kv5m_err.c: $(AC) $(ET)kv5m_err.et
	$(AWK) -f $(AC) outfile=$@ $(ET)kv5m_err.et
$(ET)krb524_err.c: $(AC) $(ET)krb524_err.et
	$(AWK) -f $(AC) outfile=$@ $(ET)krb524_err.et
$(PR)prof_err.c: $(AC) $(PR)prof_err.et
	$(AWK) -f $(AC) outfile=$@ $(PR)prof_err.et
$(GG)gssapi_err_generic.c: $(AC) $(GG)gssapi_err_generic.et
	$(AWK) -f $(AC) outfile=$@ $(GG)gssapi_err_generic.et
$(GK)gssapi_err_krb5.c: $(AC) $(GK)gssapi_err_krb5.et
	$(AWK) -f $(AC) outfile=$@ $(GK)gssapi_err_krb5.et
$(CE)test1.c: $(AC) $(CE)test1.et
	$(AWK) -f $(AC) outfile=$@ $(CE)test1.et
$(CE)test2.c: $(AC) $(CE)test2.et
	$(AWK) -f $(AC) outfile=$@ $(CE)test2.et

KRBHDEP = $(INC)krb5\krb5.hin $(INC)krb5_err.h $(INC)kdb5_err.h \
	$(INC)kv5m_err.h $(INC)krb524_err.h $(INC)asn1_err.h

$(INC)krb5\krb5.h: $(KRBHDEP)
	rm -f $@
	cat $(KRBHDEP) > $@
$(PR)profile.h: $(PR)profile.hin $(PR)prof_err.h
	rm -f $@
	cat $(PR)profile.hin $(PR)prof_err.h > $@
$(GG)gssapi.h: $(GG)gssapi.hin
	rm -f $@
	cat $(GG)gssapi.hin > $@

awk-windows-mac: $(ETOUT) $(HOUT)

#
# The maybe-awk target needs to happen after AWK is defined.
#

##DOS##maybe-awk::
##DOS##!ifdef WHICH_CMD
##DOS##!if ![ $(WHICH_CMD) $(AWK) ]
##DOS##maybe-awk:: awk-windows-mac
##DOS##!endif
##DOS##!endif

clean-windows-mac:
	rm -f $(CLEANUP)

distclean-windows:
	config\rm.bat $(CLEANUP:^/=^\)
	config\rm.bat $(WINMAKEFILES)
	config\rm.bat $(KBINDIR)\*.dll $(KBINDIR)\*.exe
	@if exist $(KBINDIR)\nul rmdir $(KBINDIR)

install-windows::
	@if "$(KRB_INSTALL_DIR)"=="" @echo KRB_INSTALL_DIR is not defined!  Please define it.
	@if "$(KRB_INSTALL_DIR)"=="" @dir /b \nul\nul
	@if not exist "$(KRB_INSTALL_DIR)\$(NULL)" @echo The directory $(KRB_INSTALL_DIR) does not exist.  Please create it.
	@if not exist "$(KRB_INSTALL_DIR)\$(NULL)" @dir /b $(KRB_INSTALL_DIR)\nul
	@if not exist "$(KRB_INSTALL_DIR)\include\$(NULL)" @mkdir "$(KRB_INSTALL_DIR)\include"
	@if not exist "$(KRB_INSTALL_DIR)\include\krb5\$(NULL)" @mkdir "$(KRB_INSTALL_DIR)\include\krb5"
	@if not exist "$(KRB_INSTALL_DIR)\include\gssapi\$(NULL)" @mkdir "$(KRB_INSTALL_DIR)\include\gssapi"
	@if not exist "$(KRB_INSTALL_DIR)\lib\$(NULL)" @mkdir "$(KRB_INSTALL_DIR)\lib"
	@if not exist "$(KRB_INSTALL_DIR)\bin\$(NULL)" @mkdir "$(KRB_INSTALL_DIR)\bin"
	$(CP) include\krb5.h "$(KRB_INSTALL_DIR)\include\."
	$(CP) include\krb5\krb5.h "$(KRB_INSTALL_DIR)\include\krb5\."
	$(CP) include\win-mac.h "$(KRB_INSTALL_DIR)\include\."
	$(CP) include\profile.h "$(KRB_INSTALL_DIR)\include\."
	$(CP) include\com_err.h "$(KRB_INSTALL_DIR)\include\."
	$(CP) include\gssapi\gssapi.h "$(KRB_INSTALL_DIR)\include\gssapi\."
	$(CP) include\gssapi\gssapi_krb5.h "$(KRB_INSTALL_DIR)\include\gssapi\."
	$(CP) lib\$(OUTPRE)*.lib "$(KRB_INSTALL_DIR)\lib\."
	$(CP) lib\$(OUTPRE)*.dll "$(KRB_INSTALL_DIR)\bin\."
	$(CP) windows\cns\$(OUTPRE)krb5.exe "$(KRB_INSTALL_DIR)\bin\."
	$(CP) windows\wintel\$(OUTPRE)telnet.exe "$(KRB_INSTALL_DIR)\bin\."
	$(CP) windows\gss\$(OUTPRE)gss.exe "$(KRB_INSTALL_DIR)\bin\."
	$(CP) appl\gss-sample\$(OUTPRE)gss-server.exe "$(KRB_INSTALL_DIR)\bin\."
	$(CP) appl\gss-sample\$(OUTPRE)gss-client.exe "$(KRB_INSTALL_DIR)\bin\."
	$(CP) windows\ms2mit\$(OUTPRE)ms2mit.exe "$(KRB_INSTALL_DIR)\bin\."
	$(CP) appl\gssftp\ftp\$(OUTPRE)ftp.exe "$(KRB_INSTALL_DIR)\bin\."
	$(CP) ccapi\lib\win\$(OUTPRE)ccapi.dll "$(KRB_INSTALL_DIR)\bin\."
	$(CP) ccapi\server\win\$(OUTPRE)ccapiserver.exe "$(KRB_INSTALL_DIR)\bin\."
	$(CP) clients\kvno\$(OUTPRE)kvno.exe "$(KRB_INSTALL_DIR)\bin\."
	$(CP) clients\klist\$(OUTPRE)klist.exe "$(KRB_INSTALL_DIR)\bin\."
	$(CP) clients\kinit\$(OUTPRE)kinit.exe "$(KRB_INSTALL_DIR)\bin\."
	$(CP) clients\kdestroy\$(OUTPRE)kdestroy.exe "$(KRB_INSTALL_DIR)\bin\."
	$(CP) clients\kcpytkt\$(OUTPRE)kcpytkt.exe "$(KRB_INSTALL_DIR)\bin\."
	$(CP) clients\kdeltkt\$(OUTPRE)kdeltkt.exe "$(KRB_INSTALL_DIR)\bin\."
	$(CP) clients\kpasswd\$(OUTPRE)kpasswd.exe "$(KRB_INSTALL_DIR)\bin\."

install-unix:: 
	$(INSTALL_SCRIPT) krb5-config \
		$(DESTDIR)$(CLIENT_BINDIR)/krb5-config
	$(INSTALL_DATA) $(srcdir)/krb5-config.M $(DESTDIR)$(CLIENT_MANDIR)/krb5-config.1

krb5-config: $(srcdir)/krb5-config.in $(thisconfigdir)/config.status
	cd $(thisconfigdir) && $(SHELL) config.status krb5-config

# Test to ensure that krb5-config does not spit out things like
# $(PURE) or $(LDFLAGS) in case someone changes config/shlib.conf
check-unix:: krb5-config
	$(SHELL) $(srcdir)/t_krbconf
check-prerecurse: fake-install

COV_BUILD=	cov-build
COV_ANALYZE=	cov-analyze
COV_COMMIT=	cov-commit-defects --product "$(COV_PRODUCT)" --user "$(COV_USER)" --target "$(COV_TARGET)" --description "$(COV_DESC)"
COV_MAKE_LIB=	cov-make-library

COV_PRODUCT=	krb5
COV_USER=	admin
COV_DATADIR=
COV_TARGET=	$(host)
COV_DESC=

# Set to, e.g., "--all" or "--security".
COV_ANALYSES=
# Temporary directory, might as well put it in the build tree.
COV_TEMPDIR=	cov-temp
# Sources modeling some functions or macros confusing Prevent.
COV_MODELS=\
	$(SRCTOP)/util/coverity-models/threads.c

# Depend on Makefiles to ensure that (in maintainer mode) the configure
# scripts won't get rerun under cov-build.
coverity prevent cov: Makefiles
	$(COV_BUILD) --dir $(COV_TEMPDIR) $(MAKE) all
	$(COV_ANALYZE) $(COV_ANALYSES) --dir $(COV_TEMPDIR)
	if test "$(COV_DATADIR)" != ""; then \
		$(COV_COMMIT) --dir $(COV_TEMPDIR) --datadir $(COV_DATADIR); \
	else \
		echo "** Coverity Prevent analysis results not commit to Defect Manager"; \
	fi
# No dependencies here.
############################################################
## config/post.in
##

# in case there is no default target (very unlikely)
all::

check-windows::

##############################
# dependency generation
#

depend:: depend-postrecurse
depend-postrecurse: depend-recurse
depend-recurse: depend-prerecurse

depend-prerecurse:
depend-postrecurse:

depend-postrecurse: depend-update-makefile

ALL_DEP_SRCS= $(SRCS) $(EXTRADEPSRCS)

# be sure to check ALL_DEP_SRCS against *what it would be if SRCS and
# EXTRADEPSRCS are both empty*
$(BUILDTOP)/.depend-verify-srcdir:
	@if test "$(srcdir)" = "." ; then \
		echo 1>&2 error: cannot build dependencies with srcdir=. ; \
		echo 1>&2 "(can't distinguish generated files from source files)" ; \
		exit 1 ; \
	else \
		if test -r $(BUILDTOP)/.depend-verify-srcdir; then :; \
			else (set -x; touch $(BUILDTOP)/.depend-verify-srcdir); fi \
	fi
$(BUILDTOP)/.depend-verify-et: depend-verify-et-$(COM_ERR_VERSION)
depend-verify-et-k5:
	@if test -r $(BUILDTOP)/.depend-verify-et; then :; \
		else (set -x; touch $(BUILDTOP)/.depend-verify-et); fi
depend-verify-et-sys:
	@echo 1>&2 error: cannot build dependencies using system et package
	@exit 1
$(BUILDTOP)/.depend-verify-ss: depend-verify-ss-$(SS_VERSION)
depend-verify-ss-k5:
	@if test -r $(BUILDTOP)/.depend-verify-ss; then :; \
		else (set -x; touch $(BUILDTOP)/.depend-verify-ss); fi
depend-verify-ss-sys:
	@echo 1>&2 error: cannot build dependencies using system ss package
	@exit 1
$(BUILDTOP)/.depend-verify-gcc: depend-verify-gcc-yes
depend-verify-gcc-yes:
	@if test -r $(BUILDTOP)/.depend-verify-gcc; then :; \
		else (set -x; touch $(BUILDTOP)/.depend-verify-gcc); fi
depend-verify-gcc-no:
	@echo 1>&2 error: The '"depend"' rules are written for gcc.
	@echo 1>&2 Please use gcc, or update the rules to handle your compiler.
	@exit 1

DEP_CFG_VERIFY = $(BUILDTOP)/.depend-verify-srcdir \
	$(BUILDTOP)/.depend-verify-et $(BUILDTOP)/.depend-verify-ss
DEP_VERIFY = $(DEP_CFG_VERIFY) $(BUILDTOP)/.depend-verify-gcc

.d: $(ALL_DEP_SRCS) $(DEP_CFG_VERIFY) depend-dependencies
	if test "$(ALL_DEP_SRCS)" != " " ; then \
		$(RM) .dtmp && $(MAKE) .dtmp && mv -f .dtmp .d ; \
	else \
		touch .d ; \
	fi

# These are dependencies of the depend target that do not get fed to
# the compiler.  Examples include generated header files.
depend-dependencies:

# .dtmp must *always* be out of date so that $? can be used to perform
# VPATH searches on the sources.
#
# NOTE: This will fail when using Make programs whose VPATH support is
# broken.
.dtmp: $(ALL_DEP_SRCS)
	$(CC) -M -DDEPEND $(ALL_CFLAGS) $? > .dtmp

# NOTE: This will also generate spurious $(OUTPRE) and $(OBJEXT)
# references in rules for non-library objects in a directory where
# library objects happen to be built.  It's mostly harmless.
.depend: .d $(SRCTOP)/util/depfix.pl
	x=`$(CC) -print-libgcc-file-name` ; \
	perl $(SRCTOP)/util/depfix.pl \
		'$(SRCTOP)' '$(myfulldir)' '$(srcdir)' '$(BUILDTOP)' "$$x" '$(STLIBOBJS)' \
		< .d > .depend

# Temporarily keep the rule for removing the dependency line eater
# until we're sure we've gotten everything converted and excised the
# old stuff from Makefile.in files.
depend-update-makefile: .depend depend-recurse
	if test -n "$(SRCS)" ; then \
		$(CP) .depend $(srcdir)/deps.new ; \
	else \
		echo "# No dependencies here." > $(srcdir)/deps.new ; \
	fi
	$(SRCTOP)/config/move-if-changed $(srcdir)/deps.new $(srcdir)/deps
	sed -e '/^# +++ Dependency line eater +++/,$$d' \
		< $(srcdir)/Makefile.in > $(srcdir)/Makefile.in.new
	$(SRCTOP)/config/move-if-changed $(srcdir)/Makefile.in.new $(srcdir)/Makefile.in

DEPTARGETS = .depend .d .dtmp $(DEP_VERIFY)
DEPTARGETS_CLEAN = .depend .d .dtmp $(DEPTARGETS_._.)
DEPTARGETS_._. = $(DEP_VERIFY)

# Clear out dependencies.  Should only be used temporarily, e.g., while
# moving or renaming headers and then rebuilding dependencies.
undepend:: undepend-postrecurse
undepend-recurse:
undepend-postrecurse: undepend-recurse
	if test -n "$(SRCS)" ; then \
		sed -e '/^# +++ Dependency line eater +++/,$$d' \
			< $(srcdir)/Makefile.in \
			> $(srcdir)/Makefile.in.new ;\
		echo "# +++ Dependency line eater +++" >> $(srcdir)/Makefile.in.new ;\
		echo "# (dependencies temporarily removed)" >> $(srcdir)/Makefile.in.new ;\
		$(SRCTOP)/config/move-if-changed $(srcdir)/Makefile.in.new $(srcdir)/Makefile.in;\
	else :; fi

#
# end dependency generation
##############################

clean:: clean-$(WHAT)

clean-unix::
	$(RM) $(OBJS) $(DEPTARGETS_CLEAN) $(EXTRA_FILES) et-[ch]-*.et et-[ch]-*.[ch]
	-$(RM) -r $(srcdir)/$(thisconfigdir)/autom4te.cache

clean-windows::
	$(RM) *.$(OBJEXT)
	$(RM) msvc.pdb *.err

distclean:: distclean-$(WHAT)

distclean-normal-clean:
	$(MAKE) NORECURSE=true clean
distclean-prerecurse: distclean-normal-clean
distclean-nuke-configure-state:
	$(RM) config.log config.cache config.status Makefile
distclean-postrecurse: distclean-nuke-configure-state

Makefiles-prerecurse: Makefile

# thisconfigdir = relative path from this Makefile to config.status
# mydir = relative path from config.status to this Makefile
Makefile: $(srcdir)/Makefile.in $(srcdir)/deps $(thisconfigdir)/config.status \
		$(SRCTOP)/config/pre.in $(SRCTOP)/config/post.in
	cd $(thisconfigdir) && $(SHELL) config.status $(mydir)/Makefile
$(thisconfigdir)/config.status: $(srcdir)/$(thisconfigdir)/configure
	cd $(thisconfigdir) && $(SHELL) config.status --recheck
# autom4te.cache supposedly improves performance with multiple runs, but
# it breaks across versions, and around MIT we've got plenty of version
# mixing.  So nuke it.
$(srcdir)/$(thisconfigdir)/configure: # \
		$(srcdir)/$(thisconfigdir)/configure.in \
		$(SRCTOP)/patchlevel.h \
		$(SRCTOP)/aclocal.m4
	-$(RM) -r $(srcdir)/$(thisconfigdir)/autom4te.cache
	cd $(srcdir)/$(thisconfigdir) && \
		$(AUTOCONF) --include=$(CONFIG_RELTOPDIR) $(AUTOCONFFLAGS)
	-$(RM) -r $(srcdir)/$(thisconfigdir)/autom4te.cache

RECURSE_TARGETS=all-recurse clean-recurse distclean-recurse install-recurse \
	generate-files-mac-recurse \
	check-recurse depend-recurse undepend-recurse \
	Makefiles-recurse install-headers-recurse

# MY_SUBDIRS overrides any setting of SUBDIRS generated by the
# configure script that generated this Makefile.  This is needed when
# the configure script that produced this Makefile creates multiple
# Makefiles in different directories; the setting of SUBDIRS will be
# the same in each.
#
# LOCAL_SUBDIRS seems to account for the case where the configure
# script doesn't call any other subsidiary configure scripts, but
# generates multiple Makefiles.
$(RECURSE_TARGETS):
	@case "`echo 'x$(MFLAGS)'|sed -e 's/^x//' -e 's/ --.*$$//'`" \
		in *[ik]*) e="status=1" ;; *) e="exit 1";; esac; \
	do_subdirs="$(SUBDIRS)" ; \
	status=0; \
	if test -n "$$do_subdirs" && test -z "$(NORECURSE)"; then \
	for i in $$do_subdirs ; do \
		if test -d $$i && test -r $$i/Makefile ; then \
		case $$i in .);; *) \
			target=`echo $@|sed s/-recurse//`; \
			echo "making $$target in $(CURRENT_DIR)$$i..."; \
			if (cd $$i ; $(MAKE) \
			    CURRENT_DIR=$(CURRENT_DIR)$$i/ $$target) then :; \
			else eval $$e; fi; \
			;; \
		esac; \
		else \
			echo "Skipping missing directory $(CURRENT_DIR)$$i" ; \
		fi; \
	done; \
	else :; \
	fi;\
	exit $$status

##
## end of post.in
############################################################
