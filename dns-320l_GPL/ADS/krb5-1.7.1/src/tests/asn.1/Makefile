############################################################
## config/pre.in
## common prefix for all Makefile.in in the Kerberos V5 tree.
##

# These are set per-directory by autoconf 2.52 and 2.53:
#  srcdir=.
#  top_srcdir=../..
# but these are only set by autoconf 2.53, and thus not useful to us on
# Mac OS X yet (as of 10.2):
#  abs_srcdir=/home/roy/DNS-345_GPL/packages/krb5-1.7.1/src/tests/asn.1
#  abs_top_srcdir=/home/roy/DNS-345_GPL/packages/krb5-1.7.1/src/tests/asn.1/../..
#  builddir=.
#  abs_builddir=/home/roy/DNS-345_GPL/packages/krb5-1.7.1/src/tests/asn.1
#  top_builddir=../../
#  abs_top_builddir=/home/roy/DNS-345_GPL/packages/krb5-1.7.1/src/tests/asn.1/../../.
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
top_srcdir = ../..

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
thisconfigdir=../..
myfulldir=tests/asn.1
mydir=tests/asn.1
BUILDTOP=$(REL)..$(S)..
PROG_LIBPATH=-L$(TOPLIBD)
PROG_RPATH=$(KRB5_LIBDIR)
LDAP=no

RUN_SETUP = LD_LIBRARY_PATH=`echo $(PROG_LIBPATH) | sed -e "s/-L//g" -e "s/ /:/g"`; export LD_LIBRARY_PATH; 

SRCS= $(srcdir)/krb5_encode_test.c $(srcdir)/krb5_decode_test.c \
	$(srcdir)/ktest.c $(srcdir)/ktest_equal.c $(srcdir)/utility.c \
	$(srcdir)/trval.c $(srcdir)/t_trval.c

all:: krb5_encode_test krb5_decode_test t_trval

LOCALINCLUDES = -I$(srcdir)/../../lib/krb5/asn.1

ENCOBJS = krb5_encode_test.o ktest.o ktest_equal.o utility.o trval.o

krb5_encode_test: $(ENCOBJS) $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o krb5_encode_test $(ENCOBJS) $(KRB5_BASE_LIBS)

DECOBJS = krb5_decode_test.o ktest.o ktest_equal.o utility.o

krb5_decode_test: $(DECOBJS) $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o krb5_decode_test $(DECOBJS) $(KRB5_BASE_LIBS)

t_trval: t_trval.o
	$(CC) -o t_trval $(ALL_CFLAGS) t_trval.o

check:: check-encode check-encode-trval check-decode

check-decode: krb5_decode_test
	KRB5_CONFIG=$(SRCTOP)/config-files/krb5.conf ; \
		export KRB5_CONFIG ;\
		$(RUN_SETUP) $(VALGRIND) ./krb5_decode_test

expected_encode.out: reference_encode.out ldap_encode.out
	if test "$(LDAP)" = yes; then \
	  cat $(srcdir)/reference_encode.out $(srcdir)/ldap_encode.out > expected_encode.out; \
	else \
	  cat $(srcdir)/reference_encode.out > expected_encode.out; \
	fi

expected_trval.out: trval_reference.out ldap_trval.out
	if test "$(LDAP)" = yes; then \
	  cat $(srcdir)/trval_reference.out $(srcdir)/ldap_trval.out > expected_trval.out; \
	else \
	  cat $(srcdir)/trval_reference.out > expected_trval.out; \
	fi

check-encode: krb5_encode_test expected_encode.out
	KRB5_CONFIG=$(SRCTOP)/config-files/krb5.conf ; \
		export KRB5_CONFIG ;\
		$(RUN_SETUP) $(VALGRIND) ./krb5_encode_test > test.out
	cmp test.out expected_encode.out

check-encode-trval: krb5_encode_test expected_trval.out
	KRB5_CONFIG=$(SRCTOP)/config-files/krb5.conf ; \
		export KRB5_CONFIG ;\
		$(RUN_SETUP) $(VALGRIND) ./krb5_encode_test -t > trval.out
	cmp trval.out expected_trval.out

install::

clean::
	rm -f *~ *.o krb5_encode_test krb5_decode_test test.out trval t_trval expected_encode.out expected_trval.out trval.out


################ Dependencies ################
krb5_decode_test.o: ktest.h utility.h ktest_equal.h debug.h
krb5_encode_test.o: utility.h ktest.h debug.h
trval.o: trval.c
ktest.o: ktest.h utility.h
ktest_equal.o: ktest_equal.h
#utility.o: utility.h
#utility.h: krbasn1.h asn1buf.h
##############################################

# 
# Generated makefile dependencies follow.
#
$(OUTPRE)krb5_encode_test.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/kdb.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/lib/krb5/asn.1/asn1buf.h \
  $(SRCTOP)/lib/krb5/asn.1/krbasn1.h debug.h krb5_encode_test.c \
  ktest.h utility.h
$(OUTPRE)krb5_decode_test.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/kdb.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/lib/krb5/asn.1/asn1buf.h \
  $(SRCTOP)/lib/krb5/asn.1/krbasn1.h debug.h krb5_decode_test.c \
  ktest.h ktest_equal.h utility.h
$(OUTPRE)ktest.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/kdb.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/lib/krb5/asn.1/asn1buf.h \
  $(SRCTOP)/lib/krb5/asn.1/krbasn1.h ktest.c ktest.h \
  utility.h
$(OUTPRE)ktest_equal.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/kdb.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h ktest_equal.c ktest_equal.h
$(OUTPRE)utility.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  $(SRCTOP)/lib/krb5/asn.1/asn1buf.h $(SRCTOP)/lib/krb5/asn.1/krbasn1.h \
  utility.c utility.h
$(OUTPRE)trval.$(OBJEXT): trval.c
$(OUTPRE)t_trval.$(OBJEXT): t_trval.c trval.c
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
DEPTARGETS_../.._. = $(DEP_VERIFY)

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
