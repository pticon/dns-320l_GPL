
bin_PROGRAMS = inotifywait inotifywatch inotifybt inotify_itune inotify_upnp
inotifywait_SOURCES = inotifywait.c common.c common.h
inotifywatch_SOURCES = inotifywatch.c common.c common.h
inotifybt_SOURCES = inotifybt.c common.c common.h
inotify_itune_SOURCES = inotify_itune.c common.c common.h
inotify_upnp_SOURCES = inotify_upnp.c common.c common.h

AM_CFLAGS = -std=c99 -I../libinotifytools/src -I/home/roy/git_home/DNS-345/module/include -L../libinotifytools/src -L/home/roy/git_home/DNS-345/module/lib -lpthread -lxmldbc -s
LDADD = ../libinotifytools/src/libinotifytools.la

dist-hook:
	touch -d '2 days ago' $(distdir)/*

