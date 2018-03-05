# $Id: Makefile.in,v 1.4 2006/08/19 09:12:14 dtucker Exp $

sysconfdir=${prefix}/etc
piddir=/var/run
srcdir=.
top_srcdir=../..


CC=arm-mv5sft-linux-gnueabi-gcc
LD=arm-mv5sft-linux-gnueabi-gcc
CFLAGS=-g -O2 -Wall -Wpointer-arith -Wuninitialized -Wsign-compare -Wno-pointer-sign -fstack-protector-all -std=gnu99 
CPPFLAGS=-I. -I.. -I$(srcdir) -I$(srcdir)/.. -I/home/roy/git_home/DNS-345_GPL/openssh-5.0p1/../zlib-1.2.3/_install//include  -DHAVE_CONFIG_H
EXEEXT=
LIBCOMPAT=../libopenbsd-compat.a
LIBS=-lcrypto -lutil -lz -lnsl  -lcrypt -lresolv -lresolv
LDFLAGS=-L/home/roy/git_home/DNS-345_GPL/openssh-5.0p1/../zlib-1.2.3/_install//lib  -fstack-protector-all $(LIBCOMPAT)

TESTPROGS=closefromtest$(EXEEXT) snprintftest$(EXEEXT) strduptest$(EXEEXT) \
	strtonumtest$(EXEEXT)

all:	t-exec ${OTHERTESTS}

%$(EXEEXT):	%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $< $(LIBCOMPAT) $(LIBS)

t-exec:	$(TESTPROGS)
	@echo running compat regress tests
	@for TEST in ""$?; do \
		echo "run test $${TEST}" ... 1>&2; \
		./$${TEST}$(EXEEXT) || exit $$? ; \
	done
	@echo finished compat regress tests

clean:
	rm -f *.o *.a core $(TESTPROGS) valid.out

distclean: clean
	rm -f Makefile *~
