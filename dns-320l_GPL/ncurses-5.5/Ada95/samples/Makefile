##############################################################################
# Copyright (c) 1998-2004,2005 Free Software Foundation, Inc.                #
#                                                                            #
# Permission is hereby granted, free of charge, to any person obtaining a    #
# copy of this software and associated documentation files (the "Software"), #
# to deal in the Software without restriction, including without limitation  #
# the rights to use, copy, modify, merge, publish, distribute, distribute    #
# with modifications, sublicense, and/or sell copies of the Software, and to #
# permit persons to whom the Software is furnished to do so, subject to the  #
# following conditions:                                                      #
#                                                                            #
# The above copyright notice and this permission notice shall be included in #
# all copies or substantial portions of the Software.                        #
#                                                                            #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR #
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   #
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    #
# THE ABOVE COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      #
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    #
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        #
# DEALINGS IN THE SOFTWARE.                                                  #
#                                                                            #
# Except as contained in this notice, the name(s) of the above copyright     #
# holders shall not be used in advertising or otherwise to promote the sale, #
# use or other dealings in this Software without prior written               #
# authorization.                                                             #
##############################################################################
#
#  Author:  Juergen Pfeifer, 1996
#
#  $Id: Makefile,v 1.1.1.1 2009/10/09 02:44:45 jack Exp $
#
.SUFFIXES:

SHELL		= /bin/sh
THIS		= Makefile

x		= 

srcdir		= .
prefix		= /usr
exec_prefix	= ${prefix}
libdir		= ${exec_prefix}/lib
includedir	= ${prefix}/include

INSTALL		= /usr/bin/install -c
INSTALL_DATA	= ${INSTALL} -m 644

AWK		= gawk
LN_S		= ln -s

CC		= arm-none-linux-gnueabi-gcc
CFLAGS		= -O2

CPPFLAGS	= -I. -I../../include  \
		  -DHAVE_CONFIG_H -I$(srcdir)

CCFLAGS		= $(CPPFLAGS) $(CFLAGS)

CFLAGS_NORMAL	= $(CCFLAGS)
CFLAGS_DEBUG	= $(CCFLAGS) -g -DTRACE
CFLAGS_PROFILE	= $(CCFLAGS) -pg
CFLAGS_SHARED	= $(CCFLAGS) -fPIC

CFLAGS_DEFAULT	= $(CFLAGS_NORMAL)

LINK		= $(CC)
LDFLAGS		=   

RANLIB		= arm-none-linux-gnueabi-ranlib
################################################################################
ada_srcdir=../src

LD_FLAGS     =  $(LOCAL_LIBS)    $(LDFLAGS)

ADA          = gnatmake
ADAFLAGS     = -O3 -gnatpn  -I$(srcdir)

ADAMAKE      = gnatmake
ADAMAKEFLAGS = -a -A$(srcdir) -A$(ada_srcdir) -A$(srcdir)/$(ada_srcdir)

ALIB         = terminal_interface
ABASE        = $(ALIB)-curses

CARGS        =-cargs $(ADAFLAGS)
LARGS        =-largs -L../../lib -lform -lmenu -lpanel -lncurses  $(LD_FLAGS) -lAdaCurses
 
PROGS        = tour rain ncurses 

TOUR_OBJS    =	tour.o sample.o sample-curses_demo.o sample-explanation.o 	\
		sample-form_demo.o sample-function_key_setting.o 		\
		sample-header_handler.o sample-helpers.o 			\
		sample-keyboard_handler.o sample-manifest.o sample-menu_demo.o	\
		sample-menu_demo-aux.o sample-text_io_demo.o			\
		sample-curses_demo-attributes.o sample-curses_demo-mouse.o	\
		sample-form_demo-aux.o sample-my_field_type.o

RAIN_OBJS    =	rain.o status.o

NCURSES_OBJS = ncurses.o        	ncurses2-getch_test.o \
	ncurses2-acs_and_scroll.o       ncurses2-m.o \
	ncurses2-acs_display.o          ncurses2-menu_test.o \
	ncurses2-attr_test.o            ncurses2-overlap_test.o \
	ncurses2-color_edit.o           ncurses2-slk_test.o \
	ncurses2-color_test.o           ncurses2-test_sgr_attributes.o \
	ncurses2-demo_forms.o           ncurses2-trace_set.o \
	ncurses2-demo_pad.o             ncurses2-util.o \
	ncurses2-demo_panels.o          ncurses2.o \
	ncurses2-flushinp_test.o


all ::  tour$x rain$x ncurses$x
	@echo made $@

sources :
	@echo made $@

libs \
install \
install.libs ::
	@echo made $@

uninstall \
uninstall.libs ::
	@echo made $@

ncurses$x :
	$(ADAMAKE) $(ADAMAKEFLAGS) ncurses $(CARGS) $(LARGS)

tour$x :	explain.msg
	$(ADAMAKE) $(ADAMAKEFLAGS) tour $(CARGS) $(LARGS)

explain.msg:	$(srcdir)/explain.txt
	cp $(srcdir)/explain.txt $@

rain$x :
	$(ADAMAKE) $(ADAMAKEFLAGS) rain $(CARGS) $(LARGS)

mostlyclean:
	@echo made $@

clean :: mostlyclean
	rm -f *.o *.ali b_t*.* *.s $(PROGS) a.out core b_*_test.c *.xr[bs] \
	explain.msg trace screendump 

distclean :: clean
	rm -f Makefile

realclean :: distclean
	@echo made $@
