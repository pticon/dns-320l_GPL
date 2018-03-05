
# -*- sh -*-

OPTFLAGS := -O2
SBINDIR=$(EXEC_PREFIX)/sbin
MANDIR=$(PREFIX)/share/man
INCLUDEDIR=$(PREFIX)/include
WITHOUT_XATTR=1
CC := $(CROSS)gcc
CFLAGS := -I./include -I../include $(OPTFLAGS)

ifeq ($(origin CROSS),undefined)
  BUILDDIR := .
else
# Remove the trailing slash to make the directory name
  BUILDDIR := $(CROSS:-=)
endif

ifeq ($(WITHOUT_XATTR), 1)
  CFLAGS += -DWITHOUT_XATTR
endif

RAWTARGETS = flash_eraseall nanddump nandwrite nandfirstgood

TARGETS = $(foreach target,$(RAWTARGETS),$(BUILDDIR)/$(target))

SYMLINKS =

%: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) -g -o $@ $^

$(BUILDDIR)/%.o: %.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -g -c -o $@ $< -g -Wp,-MD,$(BUILDDIR)/.$(<F).dep

.SUFFIXES:

all: $(TARGETS)
#	make -C $(BUILDDIR)/ubi-utils
	make -C ubi-utils
IGNORE=${wildcard $(BUILDDIR)/.*.c.dep}
-include ${IGNORE}

clean:
	rm -f $(BUILDDIR)/*.o $(TARGETS) $(BUILDDIR)/.*.c.dep $(SYMLINKS)
	if [ "$(BUILDDIR)x" != ".x" ]; then rm -rf $(BUILDDIR); fi
	make -C ubi-utils clean

$(SYMLINKS):
	ln -sf ../fs/jffs2/$@ $@

$(BUILDDIR)/flash_eraseall: $(BUILDDIR)/crc32.o $(BUILDDIR)/flash_eraseall.o
	$(CC) $(LDFLAGS) -o $@ $^

install: ${TARGETS}
	mkdir -p ${DESTDIR}/${SBINDIR}
	install -m0755 ${TARGETS} ${DESTDIR}/${SBINDIR}/
	mkdir -p ${DESTDIR}/${MANDIR}/man1
	gzip -9c mkfs.jffs2.1 > ${DESTDIR}/${MANDIR}/man1/mkfs.jffs2.1.gz
	make -C ubi-utils install
