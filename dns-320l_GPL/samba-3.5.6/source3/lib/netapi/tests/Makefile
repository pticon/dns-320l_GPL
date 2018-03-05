KRB5LIBS=-L/home/roy/git_home/DNS-345_GPL/ADS/krb5-1.7.1/src/_install/lib -Wl,-rpath -Wl,/home/roy/git_home/DNS-345_GPL/ADS/krb5-1.7.1/src/_install/lib -L/home/roy/git_home/DNS-345_GPL/ADS/krb5-1.7.1/src/_install/lib -Wl,-rpath -Wl,/home/roy/git_home/DNS-345_GPL/ADS/krb5-1.7.1/src/_install/lib -lgssapi_krb5 -lkrb5 -lk5crypto -lcom_err -lresolv -ldl
LDAP_LIBS=-lldap -llber 
LIBS=-lresolv -lnsl -ldl -liconv -lnetapi -ltdb -ltalloc
DEVELOPER_CFLAGS=-g -Wall -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -DDEBUG_PASSWORD -DDEVELOPER
CCFLAGS=-I../ -L../../../bin  -I../lib/zlib -I/home/roy/git_home/DNS-345_GPL/ADS/krb5-1.7.1/src/_install/include -O $(FLAGS) -D_SAMBA_BUILD_=3 $(GTK_FLAGS)
CC=arm-mv5sft-linux-gnueabi-gcc
PICFLAG=-fPIC
LDFLAGS=-pie -L/home/roy/git_home/DNS-345_GPL/ADS/krb5-1.7.1/src/_install/lib -Wl,-rpath -Wl,/home/roy/git_home/DNS-345_GPL/ADS/krb5-1.7.1/src/_install/lib -Wl,--as-needed -L./bin -L/home/roy/git_home/DNS-345_GPL/samba-3.5.6/source3/../../libiconv-1.9.2/_install/lib
DYNEXP=-Wl,--export-dynamic
NETAPI_LIBS=$(LIBS) $(KRB5LIBS) $(LDAP_LIBS)
CMDLINE_LIBS=$(NETAPI_LIBS) $(POPT_OBJ)

# Compile a source file.
COMPILE_CC = $(CC) -I. $(CCFLAGS) $(PICFLAG) -c $< -o $@
COMPILE = $(COMPILE_CC)

PROGS = bin/netapitest

all: $(PROGS)

MAKEDIR = || exec false; \
	  if test -d "$$dir"; then :; else \
	  echo mkdir "$$dir"; \
	  mkdir -p "$$dir" >/dev/null 2>&1 || \
	  test -d "$$dir" || \
	  mkdir "$$dir" || \
	  exec false; fi || exec false

BINARY_PREREQS = bin/.dummy

bin/.dummy:
	@if (: >> $@ || : > $@) >/dev/null 2>&1; then :; else \
	  dir=bin $(MAKEDIR); fi
	@: >> $@ || : > $@ # what a fancy emoticon!

.c.o:
	@if (: >> $@ || : > $@) >/dev/null 2>&1; then rm -f $@; else \
	 dir=`echo $@ | sed 's,/[^/]*$$,,;s,^$$,.,'` $(MAKEDIR); fi
	@echo Compiling $*.c
	@$(COMPILE) && exit 0;\
		echo "The following command failed:" 1>&2;\
		echo "$(COMPILE_CC)" 1>&2;\
		$(COMPILE_CC) >/dev/null 2>&1

CMDLINE_OBJ = common.o
NETAPIBUFFER_OBJ = netapibuffer.o
NETAPITEST_OBJ = netapitest.o netlocalgroup.o netuser.o netgroup.o netdisplay.o netshare.o netfile.o netserver.o $(CMDLINE_OBJ)

bin/netapitest: $(BINARY_PREREQS) $(NETAPITEST_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(NETAPITEST_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

clean:
	-rm -f $(PROGS)
	-rm -f core */*~ *~ \
		*/*.o */*/*.o */*/*/*.o

