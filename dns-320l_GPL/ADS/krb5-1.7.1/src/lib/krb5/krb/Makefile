############################################################
## config/pre.in
## common prefix for all Makefile.in in the Kerberos V5 tree.
##

# These are set per-directory by autoconf 2.52 and 2.53:
#  srcdir=.
#  top_srcdir=../../..
# but these are only set by autoconf 2.53, and thus not useful to us on
# Mac OS X yet (as of 10.2):
#  abs_srcdir=/home/roy/DNS-345_GPL/packages/krb5-1.7.1/src/lib/krb5/krb
#  abs_top_srcdir=/home/roy/DNS-345_GPL/packages/krb5-1.7.1/src/lib/krb5/krb/../../..
#  builddir=.
#  abs_builddir=/home/roy/DNS-345_GPL/packages/krb5-1.7.1/src/lib/krb5/krb
#  top_builddir=../../../
#  abs_top_builddir=/home/roy/DNS-345_GPL/packages/krb5-1.7.1/src/lib/krb5/krb/../../../.
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
top_srcdir = ../../..

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
thisconfigdir=../../..
myfulldir=lib/krb5/krb
mydir=lib/krb5/krb
BUILDTOP=$(REL)..$(S)..$(S)..
RUN_SETUP = LD_LIBRARY_PATH=`echo $(PROG_LIBPATH) | sed -e "s/-L//g" -e "s/ /:/g"`; export LD_LIBRARY_PATH; 
PROG_LIBPATH=-L$(TOPLIBD)
PROG_RPATH=$(KRB5_LIBDIR)
LOCALINCLUDES = -I$(srcdir)/../os -I$(SRCTOP)
DEFS=-DLIBDIR=\"$(KRB5_LIBDIR)\"

##DOS##BUILDTOP = ..\..\..
##DOS##PREFIXDIR=krb
##DOS##OBJFILE=..\$(OUTPRE)$(PREFIXDIR).lst

STLIBOBJS= \
	addr_comp.o	\
	addr_order.o	\
	addr_srch.o	\
	appdefault.o	\
	auth_con.o	\
	bld_pr_ext.o	\
	bld_princ.o	\
	chk_trans.o	\
	chpw.o		\
	conv_creds.o	\
	conv_princ.o	\
	copy_addrs.o	\
	copy_auth.o	\
	copy_athctr.o	\
	copy_cksum.o    \
	copy_creds.o	\
	copy_data.o	\
	copy_key.o	\
	copy_princ.o	\
	copy_tick.o	\
	cp_key_cnt.o	\
	decode_kdc.o	\
	decrypt_tk.o	\
	deltat.o	\
        enc_helper.o	\
	encode_kdc.o	\
	encrypt_tk.o	\
	fast.o \
	free_rtree.o	\
	fwd_tgt.o	\
	gc_frm_kdc.o	\
	gc_via_tkt.o	\
	gen_seqnum.o	\
	gen_subkey.o	\
	get_creds.o	\
	get_in_tkt.o	\
	gic_keytab.o	\
	gic_opt.o	\
	gic_pwd.o	\
	in_tkt_sky.o	\
	init_ctx.o	\
	init_keyblock.o \
	kdc_rep_dc.o	\
	kerrs.o		\
	kfree.o		\
	mk_cred.o	\
	mk_error.o	\
	mk_priv.o	\
	mk_rep.o	\
	mk_req.o	\
	mk_req_ext.o	\
	mk_safe.o	\
	pac.o		\
	parse.o		\
	pr_to_salt.o	\
	preauth.o	\
	preauth2.o	\
	princ_comp.o	\
	rd_cred.o	\
	rd_error.o	\
	rd_priv.o	\
	rd_rep.o	\
	rd_req.o	\
	rd_req_dec.o	\
	rd_safe.o	\
	recvauth.o	\
	sendauth.o	\
	send_tgs.o	\
	ser_actx.o	\
	ser_adata.o	\
	ser_addr.o	\
	ser_auth.o	\
	ser_cksum.o	\
	ser_ctx.o	\
	ser_key.o	\
	ser_princ.o	\
	serialize.o	\
	set_realm.o	\
	srv_dec_tkt.o	\
	srv_rcache.o	\
	str_conv.o	\
	tgtname.o	\
	unparse.o	\
	valid_times.o	\
	vfy_increds.o	\
	vic_opt.o	\
	walk_rtree.o

OBJS=	$(OUTPRE)addr_comp.$(OBJEXT)	\
	$(OUTPRE)addr_order.$(OBJEXT)	\
	$(OUTPRE)addr_srch.$(OBJEXT)	\
	$(OUTPRE)appdefault.$(OBJEXT)	\
	$(OUTPRE)auth_con.$(OBJEXT)	\
	$(OUTPRE)bld_pr_ext.$(OBJEXT)	\
	$(OUTPRE)bld_princ.$(OBJEXT)	\
	$(OUTPRE)chk_trans.$(OBJEXT)	\
	$(OUTPRE)chpw.$(OBJEXT)		\
	$(OUTPRE)conv_creds.$(OBJEXT)	\
	$(OUTPRE)conv_princ.$(OBJEXT)	\
	$(OUTPRE)copy_addrs.$(OBJEXT)	\
	$(OUTPRE)copy_auth.$(OBJEXT)	\
	$(OUTPRE)copy_athctr.$(OBJEXT)	\
	$(OUTPRE)copy_cksum.$(OBJEXT)    \
	$(OUTPRE)copy_creds.$(OBJEXT)	\
	$(OUTPRE)copy_data.$(OBJEXT)	\
	$(OUTPRE)copy_key.$(OBJEXT)	\
	$(OUTPRE)copy_princ.$(OBJEXT)	\
	$(OUTPRE)copy_tick.$(OBJEXT)	\
	$(OUTPRE)cp_key_cnt.$(OBJEXT)	\
	$(OUTPRE)decode_kdc.$(OBJEXT)	\
	$(OUTPRE)decrypt_tk.$(OBJEXT)	\
	$(OUTPRE)deltat.$(OBJEXT)	\
        $(OUTPRE)enc_helper.$(OBJEXT)	\
	$(OUTPRE)encode_kdc.$(OBJEXT)	\
	$(OUTPRE)encrypt_tk.$(OBJEXT)	\
	$(OUTPRE)fast.$(OBJEXT) \
	$(OUTPRE)free_rtree.$(OBJEXT)	\
	$(OUTPRE)fwd_tgt.$(OBJEXT)	\
	$(OUTPRE)gc_frm_kdc.$(OBJEXT)	\
	$(OUTPRE)gc_via_tkt.$(OBJEXT)	\
	$(OUTPRE)gen_seqnum.$(OBJEXT)	\
	$(OUTPRE)gen_subkey.$(OBJEXT)	\
	$(OUTPRE)get_creds.$(OBJEXT)	\
	$(OUTPRE)get_in_tkt.$(OBJEXT)	\
	$(OUTPRE)gic_keytab.$(OBJEXT)	\
	$(OUTPRE)gic_opt.$(OBJEXT)	\
	$(OUTPRE)gic_pwd.$(OBJEXT)	\
	$(OUTPRE)in_tkt_sky.$(OBJEXT)	\
	$(OUTPRE)init_ctx.$(OBJEXT)	\
	$(OUTPRE)init_keyblock.$(OBJEXT) \
	$(OUTPRE)kdc_rep_dc.$(OBJEXT)	\
	$(OUTPRE)kerrs.$(OBJEXT)		\
	$(OUTPRE)kfree.$(OBJEXT)	\
	$(OUTPRE)mk_cred.$(OBJEXT)	\
	$(OUTPRE)mk_error.$(OBJEXT)	\
	$(OUTPRE)mk_priv.$(OBJEXT)	\
	$(OUTPRE)mk_rep.$(OBJEXT)	\
	$(OUTPRE)mk_req.$(OBJEXT)	\
	$(OUTPRE)mk_req_ext.$(OBJEXT)	\
	$(OUTPRE)mk_safe.$(OBJEXT)	\
	$(OUTPRE)pac.$(OBJEXT)		\
	$(OUTPRE)parse.$(OBJEXT)	\
	$(OUTPRE)pr_to_salt.$(OBJEXT)	\
	$(OUTPRE)preauth.$(OBJEXT)	\
	$(OUTPRE)preauth2.$(OBJEXT)	\
	$(OUTPRE)princ_comp.$(OBJEXT)	\
	$(OUTPRE)rd_cred.$(OBJEXT)	\
	$(OUTPRE)rd_error.$(OBJEXT)	\
	$(OUTPRE)rd_priv.$(OBJEXT)	\
	$(OUTPRE)rd_rep.$(OBJEXT)	\
	$(OUTPRE)rd_req.$(OBJEXT)	\
	$(OUTPRE)rd_req_dec.$(OBJEXT)	\
	$(OUTPRE)rd_safe.$(OBJEXT)	\
	$(OUTPRE)recvauth.$(OBJEXT)	\
	$(OUTPRE)sendauth.$(OBJEXT)	\
	$(OUTPRE)send_tgs.$(OBJEXT)	\
	$(OUTPRE)ser_actx.$(OBJEXT)	\
	$(OUTPRE)ser_adata.$(OBJEXT)	\
	$(OUTPRE)ser_addr.$(OBJEXT)	\
	$(OUTPRE)ser_auth.$(OBJEXT)	\
	$(OUTPRE)ser_cksum.$(OBJEXT)	\
	$(OUTPRE)ser_ctx.$(OBJEXT)	\
	$(OUTPRE)ser_key.$(OBJEXT)	\
	$(OUTPRE)ser_princ.$(OBJEXT)	\
	$(OUTPRE)serialize.$(OBJEXT)	\
	$(OUTPRE)set_realm.$(OBJEXT)	\
	$(OUTPRE)srv_dec_tkt.$(OBJEXT)	\
	$(OUTPRE)srv_rcache.$(OBJEXT)	\
	$(OUTPRE)str_conv.$(OBJEXT)	\
	$(OUTPRE)tgtname.$(OBJEXT)	\
	$(OUTPRE)unparse.$(OBJEXT)	\
	$(OUTPRE)valid_times.$(OBJEXT)	\
	$(OUTPRE)vfy_increds.$(OBJEXT)	\
	$(OUTPRE)vic_opt.$(OBJEXT)	\
	$(OUTPRE)walk_rtree.$(OBJEXT)

SRCS=	$(srcdir)/addr_comp.c	\
	$(srcdir)/addr_order.c	\
	$(srcdir)/addr_srch.c	\
	$(srcdir)/appdefault.c	\
	$(srcdir)/auth_con.c	\
	$(srcdir)/bld_pr_ext.c	\
	$(srcdir)/bld_princ.c	\
	$(srcdir)/brand.c	\
	$(srcdir)/chk_trans.c	\
	$(srcdir)/chpw.c	\
	$(srcdir)/conv_creds.c	\
	$(srcdir)/conv_princ.c	\
	$(srcdir)/copy_addrs.c	\
	$(srcdir)/copy_auth.c	\
	$(srcdir)/copy_athctr.c	\
	$(srcdir)/copy_cksum.c   \
	$(srcdir)/copy_creds.c	\
	$(srcdir)/copy_data.c	\
	$(srcdir)/copy_key.c	\
	$(srcdir)/copy_princ.c	\
	$(srcdir)/copy_tick.c	\
	$(srcdir)/cp_key_cnt.c	\
	$(srcdir)/decode_kdc.c	\
	$(srcdir)/decrypt_tk.c	\
	$(srcdir)/deltat.c	\
	$(srcdir)/enc_helper.c	\
	$(srcdir)/encode_kdc.c	\
	$(srcdir)/encrypt_tk.c	\
	$(srcdir)/fast.c \
	$(srcdir)/free_rtree.c	\
	$(srcdir)/fwd_tgt.c	\
	$(srcdir)/gc_frm_kdc.c	\
	$(srcdir)/gc_via_tkt.c	\
	$(srcdir)/gen_seqnum.c	\
	$(srcdir)/gen_subkey.c	\
	$(srcdir)/get_creds.c	\
	$(srcdir)/get_in_tkt.c	\
	$(srcdir)/gic_keytab.c	\
	$(srcdir)/gic_opt.c	\
	$(srcdir)/gic_pwd.c	\
	$(srcdir)/in_tkt_sky.c	\
	$(srcdir)/init_ctx.c	\
	$(srcdir)/init_keyblock.c \
	$(srcdir)/kdc_rep_dc.c	\
	$(srcdir)/kerrs.c	\
	$(srcdir)/kfree.c	\
	$(srcdir)/mk_cred.c	\
	$(srcdir)/mk_error.c	\
	$(srcdir)/mk_priv.c	\
	$(srcdir)/mk_rep.c	\
	$(srcdir)/mk_req.c	\
	$(srcdir)/mk_req_ext.c	\
	$(srcdir)/mk_safe.c	\
	$(srcdir)/pac.c		\
	$(srcdir)/parse.c	\
	$(srcdir)/pr_to_salt.c	\
	$(srcdir)/preauth.c	\
	$(srcdir)/preauth2.c	\
	$(srcdir)/princ_comp.c	\
	$(srcdir)/rd_cred.c	\
	$(srcdir)/rd_error.c	\
	$(srcdir)/rd_priv.c	\
	$(srcdir)/rd_rep.c	\
	$(srcdir)/rd_req.c	\
	$(srcdir)/rd_req_dec.c	\
	$(srcdir)/rd_safe.c	\
	$(srcdir)/recvauth.c	\
	$(srcdir)/sendauth.c	\
	$(srcdir)/send_tgs.c	\
	$(srcdir)/ser_actx.c	\
	$(srcdir)/ser_adata.c	\
	$(srcdir)/ser_addr.c	\
	$(srcdir)/ser_auth.c	\
	$(srcdir)/ser_cksum.c	\
	$(srcdir)/ser_ctx.c	\
	$(srcdir)/ser_key.c	\
	$(srcdir)/ser_princ.c	\
	$(srcdir)/serialize.c	\
	$(srcdir)/set_realm.c	\
	$(srcdir)/srv_dec_tkt.c	\
	$(srcdir)/srv_rcache.c	\
	$(srcdir)/str_conv.c	\
	$(srcdir)/t_ad_fx_armor.c \
	$(srcdir)/tgtname.c	\
	$(srcdir)/unparse.c	\
	$(srcdir)/valid_times.c	\
	$(srcdir)/vfy_increds.c \
	$(srcdir)/vic_opt.c	\
	$(srcdir)/walk_rtree.c	\
	$(srcdir)/t_walk_rtree.c \
	$(srcdir)/t_kerb.c	\
	$(srcdir)/t_ser.c	\
	$(srcdir)/t_deltat.c	\
	$(srcdir)/t_expand.c	\
	$(srcdir)/t_pac.c	\
	$(srcdir)/t_princ.c

# Someday, when we have a "maintainer mode", do this right:
BISON=bison
BISONFLAGS= # -v -> .output; -d -> .h
DELTAT_DEP=# x-deltat.y
##WIN32##DELTAT_DEP=

$(srcdir)/deltat.c : $(DELTAT_DEP)
	(cd $(srcdir) && $(BISON) $(BISONFLAGS) -o deltat.c x-deltat.y)

##DOS##LIBOBJS = $(OBJS)

all-unix:: all-libobjs
clean-unix:: clean-libobjs

COMERRLIB=$(TOPLIBD)/libcom_err.a

T_WALK_RTREE_OBJS= t_walk_rtree.o walk_rtree.o tgtname.o unparse.o \
	free_rtree.o bld_pr_ext.o copy_data.o

T_KERB_OBJS= t_kerb.o conv_princ.o unparse.o set_realm.o str_conv.o

T_SER_OBJS= t_ser.o ser_actx.o ser_adata.o ser_addr.o ser_auth.o ser_cksum.o \
	ser_ctx.o ser_key.o ser_princ.o serialize.o 

T_DELTAT_OBJS= t_deltat.o deltat.o

T_PAC_OBJS= t_pac.o pac.o

T_PRINC_OBJS= t_princ.o parse.o unparse.o

t_walk_rtree: $(T_WALK_RTREE_OBJS) $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o t_walk_rtree $(T_WALK_RTREE_OBJS) $(KRB5_BASE_LIBS)
t_ad_fx_armor: t_ad_fx_armor.o
	$(CC_LINK) -o $@ t_ad_fx_armor.o $(KRB5_BASE_LIBS)

t_authdata: t_authdata.o copy_auth.o
	$(CC_LINK) -o $@ t_authdata.o copy_auth.o $(KRB5_BASE_LIBS)

t_kerb: $(T_KERB_OBJS) $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o t_kerb $(T_KERB_OBJS) $(KRB5_BASE_LIBS)

t_ser: $(T_SER_OBJS) $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o t_ser $(T_SER_OBJS) $(KRB5_BASE_LIBS) 

t_deltat : $(T_DELTAT_OBJS) $(SUPPORT_DEPLIB)
	$(CC_LINK) -o t_deltat $(T_DELTAT_OBJS) $(SUPPORT_LIB)

T_EXPAND_OBJS=t_expand.o
t_expand.o : t_expand.c
t_expand : $(T_EXPAND_OBJS) $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o t_expand $(T_EXPAND_OBJS) $(KRB5_BASE_LIBS)

t_pac: $(T_PAC_OBJS) $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o t_pac $(T_PAC_OBJS) $(KRB5_BASE_LIBS)

t_princ: $(T_PRINC_OBJS) $(KRB5_BASE_DEPLIBS)
	$(CC_LINK) -o t_princ $(T_PRINC_OBJS) $(KRB5_BASE_LIBS)

TEST_PROGS= t_walk_rtree t_kerb t_ser t_deltat t_expand t_authdata t_pac \
	t_princ

check-unix:: $(TEST_PROGS)
	KRB5_CONFIG=$(srcdir)/t_krb5.conf ; export KRB5_CONFIG ;\
	$(RUN_SETUP) $(VALGRIND) ./t_kerb \
		parse_name tytso \
		parse_name tytso@SHAZAAM \
		parse_name tytso/root@VEGGIE.COM \
		parse_name tytso/tuber/carrot@VEGGIE.COM \
		parse_name tytso/a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/s/t \
		parse_name tytso/a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/s/t@FOO \
		parse_name tytso\\\\0/\\0@B\\n\\t\\\\GAG \
		parse_name tytso/\\n/\\b\\t@B\\0hacky-test \
		parse_name \\/slash/\\@atsign/octa\\/thorpe@\\/slash\\@at\\/sign \
		425_conv_principal rcmd e40-po ATHENA.MIT.EDU \
		425_conv_principal rcmd mit ATHENA.MIT.EDU \
		425_conv_principal rcmd lithium ATHENA.MIT.EDU \
		425_conv_principal rcmd tweedledumb CYGNUS.COM \
		425_conv_principal rcmd uunet UU.NET \
		425_conv_principal zephyr zephyr ATHENA.MIT.EDU \
		425_conv_principal kadmin ATHENA.MIT.EDU ATHENA.MIT.EDU \
		524_conv_principal host/e40-po.mit.edu@ATHENA.MIT.EDU \
		524_conv_principal host/foobar.stanford.edu@stanford.edu \
		set_realm marc@MIT.EDU CYGNUS.COM \
		> test.out
	cmp test.out $(srcdir)/t_ref_kerb.out
	$(RM) test.out
	KRB5_CONFIG=$(srcdir)/t_krb5.conf ; export KRB5_CONFIG ;\
		$(RUN_SETUP) $(VALGRIND) ./t_ser
	$(RUN_SETUP) $(VALGRIND) ./t_deltat
	$(RUN_SETUP) $(VALGRIND) sh $(srcdir)/transit-tests
	KRB5_CONFIG=$(srcdir)/t_krb5.conf ; export KRB5_CONFIG ;\
		$(RUN_SETUP) $(VALGRIND) sh $(srcdir)/walktree-tests
	KRB5_CONFIG=$(srcdir)/t_krb5.conf ; export KRB5_CONFIG ;\
	$(RUN_SETUP) $(VALGRIND) ./t_authdata
	$(RUN_SETUP) $(VALGRIND) ./t_pac
	$(RUN_SETUP) $(VALGRIND) ./t_princ

clean::
	$(RM) $(OUTPRE)t_walk_rtree$(EXEEXT) $(OUTPRE)t_walk_rtree.$(OBJEXT) \
		$(OUTPRE)t_kerb$(EXEEXT) $(OUTPRE)t_kerb.$(OBJEXT)	\
		$(OUTPRE)t_ser$(EXEEXT) $(OUTPRE)t_ser.$(OBJEXT)	\
		$(OUTPRE)t_deltat$(EXEEXT) $(OUTPRE)t_deltat.$(OBJEXT) \
		$(OUTPRE)t_expand$(EXEEXT) $(OUTPRE)t_expand.$(OBJEXT)  \
	$(OUTPRE)t_authdata$(EXEEXT) $(OUTPRE)t_authdata.$(OBJEXT)


### config/libobj.in
#
# Makefile fragment that builds object files for libraries.
#
# The following variables must be set in Makefile.in:
#
# STLIBOBJS	list of .o objects; this must not contain variable
#		references.

.SUFFIXES: .c .so .po
.c.so:
	$(CC) $(PICFLAGS) -DSHARED $(ALL_CFLAGS) -c $< -o $*.so.o && $(MV) $*.so.o $*.so
.c.po:
	$(CC) $(PROFFLAGS) $(ALL_CFLAGS) -c $< -o $*.po.o && $(MV) $*.po.o $*.po

# rules to generate object file lists

OBJS.ST: $(STLIBOBJS) Makefile
	@echo $(STLIBOBJS) > $@
	: updated $@

OBJS.SH: $(SHLIBOBJS) Makefile
	@echo $(SHLIBOBJS) > $@
	: updated $@

OBJS.PF: $(PFLIBOBJS) Makefile
	@echo $(PFLIBOBJS) > $@
	: updated $@

all-libobjs: $(OBJLISTS)

clean-libobjs:
	$(RM) OBJS.ST OBJS.SH OBJS.PF $(STLIBOBJS) $(SHLIBOBJS) $(PFLIBOBJS)

Makefile: $(SRCTOP)/config/libobj.in
config.status: $(SRCTOP)/config/shlib.conf

# clean-unix:: clean-libobjs
# all-unix:: all-libobjs

###
### end config/libobj.in

# 
# Generated makefile dependencies follow.
#
addr_comp.so addr_comp.po $(OUTPRE)addr_comp.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h addr_comp.c
addr_order.so addr_order.po $(OUTPRE)addr_order.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h addr_order.c
addr_srch.so addr_srch.po $(OUTPRE)addr_srch.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h addr_srch.c
appdefault.so appdefault.po $(OUTPRE)appdefault.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h appdefault.c
auth_con.so auth_con.po $(OUTPRE)auth_con.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h auth_con.c auth_con.h
bld_pr_ext.so bld_pr_ext.po $(OUTPRE)bld_pr_ext.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h bld_pr_ext.c
bld_princ.so bld_princ.po $(OUTPRE)bld_princ.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h bld_princ.c
brand.so brand.po $(OUTPRE)brand.$(OBJEXT): $(SRCTOP)/patchlevel.h \
  brand.c
chk_trans.so chk_trans.po $(OUTPRE)chk_trans.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h chk_trans.c
chpw.so chpw.po $(OUTPRE)chpw.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  auth_con.h chpw.c
conv_creds.so conv_creds.po $(OUTPRE)conv_creds.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h conv_creds.c
conv_princ.so conv_princ.po $(OUTPRE)conv_princ.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h conv_princ.c
copy_addrs.so copy_addrs.po $(OUTPRE)copy_addrs.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h copy_addrs.c
copy_auth.so copy_auth.po $(OUTPRE)copy_auth.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h copy_auth.c
copy_athctr.so copy_athctr.po $(OUTPRE)copy_athctr.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h copy_athctr.c
copy_cksum.so copy_cksum.po $(OUTPRE)copy_cksum.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h copy_cksum.c
copy_creds.so copy_creds.po $(OUTPRE)copy_creds.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h copy_creds.c
copy_data.so copy_data.po $(OUTPRE)copy_data.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h copy_data.c
copy_key.so copy_key.po $(OUTPRE)copy_key.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h copy_key.c
copy_princ.so copy_princ.po $(OUTPRE)copy_princ.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h copy_princ.c
copy_tick.so copy_tick.po $(OUTPRE)copy_tick.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h copy_tick.c
cp_key_cnt.so cp_key_cnt.po $(OUTPRE)cp_key_cnt.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h cp_key_cnt.c
decode_kdc.so decode_kdc.po $(OUTPRE)decode_kdc.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h decode_kdc.c
decrypt_tk.so decrypt_tk.po $(OUTPRE)decrypt_tk.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h decrypt_tk.c
deltat.so deltat.po $(OUTPRE)deltat.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  deltat.c
enc_helper.so enc_helper.po $(OUTPRE)enc_helper.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h enc_helper.c
encode_kdc.so encode_kdc.po $(OUTPRE)encode_kdc.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h encode_kdc.c
encrypt_tk.so encrypt_tk.po $(OUTPRE)encrypt_tk.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h encrypt_tk.c
fast.so fast.po $(OUTPRE)fast.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  fast.c fast.h int-proto.h
free_rtree.so free_rtree.po $(OUTPRE)free_rtree.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h free_rtree.c
fwd_tgt.so fwd_tgt.po $(OUTPRE)fwd_tgt.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  fwd_tgt.c
gc_frm_kdc.so gc_frm_kdc.po $(OUTPRE)gc_frm_kdc.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h gc_frm_kdc.c int-proto.h
gc_via_tkt.so gc_via_tkt.po $(OUTPRE)gc_via_tkt.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h gc_via_tkt.c int-proto.h
gen_seqnum.so gen_seqnum.po $(OUTPRE)gen_seqnum.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h gen_seqnum.c
gen_subkey.so gen_subkey.po $(OUTPRE)gen_subkey.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h gen_subkey.c
get_creds.so get_creds.po $(OUTPRE)get_creds.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h get_creds.c int-proto.h
get_in_tkt.so get_in_tkt.po $(OUTPRE)get_in_tkt.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(srcdir)/../os/os-proto.h \
  fast.h get_in_tkt.c int-proto.h
gic_keytab.so gic_keytab.po $(OUTPRE)gic_keytab.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h gic_keytab.c
gic_opt.so gic_opt.po $(OUTPRE)gic_opt.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  gic_opt.c int-proto.h
gic_pwd.so gic_pwd.po $(OUTPRE)gic_pwd.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  gic_pwd.c
in_tkt_sky.so in_tkt_sky.po $(OUTPRE)in_tkt_sky.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h in_tkt_sky.c
init_ctx.so init_ctx.po $(OUTPRE)init_ctx.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h $(SRCTOP)/patchlevel.h \
  $(srcdir)/../krb5_libinit.h brand.c init_ctx.c
init_keyblock.so init_keyblock.po $(OUTPRE)init_keyblock.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h init_keyblock.c
kdc_rep_dc.so kdc_rep_dc.po $(OUTPRE)kdc_rep_dc.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h kdc_rep_dc.c
kerrs.so kerrs.po $(OUTPRE)kerrs.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  kerrs.c
kfree.so kfree.po $(OUTPRE)kfree.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  kfree.c
mk_cred.so mk_cred.po $(OUTPRE)mk_cred.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  auth_con.h cleanup.h mk_cred.c
mk_error.so mk_error.po $(OUTPRE)mk_error.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h mk_error.c
mk_priv.so mk_priv.po $(OUTPRE)mk_priv.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  auth_con.h cleanup.h mk_priv.c
mk_rep.so mk_rep.po $(OUTPRE)mk_rep.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  auth_con.h mk_rep.c
mk_req.so mk_req.po $(OUTPRE)mk_req.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  auth_con.h mk_req.c
mk_req_ext.so mk_req_ext.po $(OUTPRE)mk_req_ext.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h auth_con.h mk_req_ext.c
mk_safe.so mk_safe.po $(OUTPRE)mk_safe.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  auth_con.h cleanup.h mk_safe.c
pac.so pac.po $(OUTPRE)pac.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/k5-utf8.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h pac.c
parse.so parse.po $(OUTPRE)parse.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  parse.c
pr_to_salt.so pr_to_salt.po $(OUTPRE)pr_to_salt.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h pr_to_salt.c
preauth.so preauth.po $(OUTPRE)preauth.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  preauth.c
preauth2.so preauth2.po $(OUTPRE)preauth2.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h fast.h int-proto.h \
  preauth2.c
princ_comp.so princ_comp.po $(OUTPRE)princ_comp.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/k5-unicode.h $(SRCTOP)/include/k5-utf8.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h princ_comp.c
rd_cred.so rd_cred.po $(OUTPRE)rd_cred.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  auth_con.h cleanup.h rd_cred.c
rd_error.so rd_error.po $(OUTPRE)rd_error.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h rd_error.c
rd_priv.so rd_priv.po $(OUTPRE)rd_priv.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  auth_con.h cleanup.h rd_priv.c
rd_rep.so rd_rep.po $(OUTPRE)rd_rep.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  auth_con.h rd_rep.c
rd_req.so rd_req.po $(OUTPRE)rd_req.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  auth_con.h rd_req.c
rd_req_dec.so rd_req_dec.po $(OUTPRE)rd_req_dec.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h auth_con.h rd_req_dec.c
rd_safe.so rd_safe.po $(OUTPRE)rd_safe.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  auth_con.h cleanup.h rd_safe.c
recvauth.so recvauth.po $(OUTPRE)recvauth.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h auth_con.h recvauth.c
sendauth.so sendauth.po $(OUTPRE)sendauth.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h auth_con.h sendauth.c
send_tgs.so send_tgs.po $(OUTPRE)send_tgs.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h send_tgs.c
ser_actx.so ser_actx.po $(OUTPRE)ser_actx.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h auth_con.h int-proto.h \
  ser_actx.c
ser_adata.so ser_adata.po $(OUTPRE)ser_adata.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h int-proto.h ser_adata.c
ser_addr.so ser_addr.po $(OUTPRE)ser_addr.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h int-proto.h ser_addr.c
ser_auth.so ser_auth.po $(OUTPRE)ser_auth.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h int-proto.h ser_auth.c
ser_cksum.so ser_cksum.po $(OUTPRE)ser_cksum.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h int-proto.h ser_cksum.c
ser_ctx.so ser_ctx.po $(OUTPRE)ser_ctx.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  ser_ctx.c
ser_key.so ser_key.po $(OUTPRE)ser_key.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  int-proto.h ser_key.c
ser_princ.so ser_princ.po $(OUTPRE)ser_princ.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h int-proto.h ser_princ.c
serialize.so serialize.po $(OUTPRE)serialize.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h serialize.c
set_realm.so set_realm.po $(OUTPRE)set_realm.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h set_realm.c
srv_dec_tkt.so srv_dec_tkt.po $(OUTPRE)srv_dec_tkt.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h srv_dec_tkt.c
srv_rcache.so srv_rcache.po $(OUTPRE)srv_rcache.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h srv_rcache.c
str_conv.so str_conv.po $(OUTPRE)str_conv.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/kdb.h $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h str_conv.c
t_ad_fx_armor.so t_ad_fx_armor.po $(OUTPRE)t_ad_fx_armor.$(OBJEXT): \
  $(BUILDTOP)/include/krb5/krb5.h $(COM_ERR_DEPS) t_ad_fx_armor.c
tgtname.so tgtname.po $(OUTPRE)tgtname.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  int-proto.h tgtname.c
unparse.so unparse.po $(OUTPRE)unparse.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  unparse.c
valid_times.so valid_times.po $(OUTPRE)valid_times.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h valid_times.c
vfy_increds.so vfy_increds.po $(OUTPRE)vfy_increds.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h int-proto.h vfy_increds.c
vic_opt.so vic_opt.po $(OUTPRE)vic_opt.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  vic_opt.c
walk_rtree.so walk_rtree.po $(OUTPRE)walk_rtree.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h int-proto.h walk_rtree.c
t_walk_rtree.so t_walk_rtree.po $(OUTPRE)t_walk_rtree.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h t_walk_rtree.c
t_kerb.so t_kerb.po $(OUTPRE)t_kerb.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(COM_ERR_DEPS) $(SRCTOP)/include/krb5.h \
  t_kerb.c
t_ser.so t_ser.po $(OUTPRE)t_ser.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  auth_con.h t_ser.c
t_deltat.so t_deltat.po $(OUTPRE)t_deltat.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h t_deltat.c
t_expand.so t_expand.po $(OUTPRE)t_expand.$(OBJEXT): \
  $(BUILDTOP)/include/autoconf.h $(BUILDTOP)/include/krb5/krb5.h \
  $(BUILDTOP)/include/osconf.h $(BUILDTOP)/include/profile.h \
  $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h $(SRCTOP)/include/k5-err.h \
  $(SRCTOP)/include/k5-gmt_mktime.h $(SRCTOP)/include/k5-int-pkinit.h \
  $(SRCTOP)/include/k5-int.h $(SRCTOP)/include/k5-platform.h \
  $(SRCTOP)/include/k5-plugin.h $(SRCTOP)/include/k5-thread.h \
  $(SRCTOP)/include/krb5.h $(SRCTOP)/include/krb5/locate_plugin.h \
  $(SRCTOP)/include/krb5/preauth_plugin.h $(SRCTOP)/include/port-sockets.h \
  $(SRCTOP)/include/socket-utils.h chk_trans.c t_expand.c
t_pac.so t_pac.po $(OUTPRE)t_pac.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  t_pac.c
t_princ.so t_princ.po $(OUTPRE)t_princ.$(OBJEXT): $(BUILDTOP)/include/autoconf.h \
  $(BUILDTOP)/include/krb5/krb5.h $(BUILDTOP)/include/osconf.h \
  $(BUILDTOP)/include/profile.h $(COM_ERR_DEPS) $(SRCTOP)/include/k5-buf.h \
  $(SRCTOP)/include/k5-err.h $(SRCTOP)/include/k5-gmt_mktime.h \
  $(SRCTOP)/include/k5-int-pkinit.h $(SRCTOP)/include/k5-int.h \
  $(SRCTOP)/include/k5-platform.h $(SRCTOP)/include/k5-plugin.h \
  $(SRCTOP)/include/k5-thread.h $(SRCTOP)/include/krb5.h \
  $(SRCTOP)/include/krb5/locate_plugin.h $(SRCTOP)/include/krb5/preauth_plugin.h \
  $(SRCTOP)/include/port-sockets.h $(SRCTOP)/include/socket-utils.h \
  t_princ.c
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
DEPTARGETS_../../.._. = $(DEP_VERIFY)

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
