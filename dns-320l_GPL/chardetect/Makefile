#
#	makefile by wyc
#
#

CROSS_PREFIX = $(shell basename $(CC) -gcc)

CXXFLAGS = -O2

CXX = $(CROSS_PREFIX)-g++
STRIP = $(CROSS_PREFIX)-strip
AR = $(CROSS_PREFIX)-ar
RANLIB = $(CROSS_PREFIX)-ranlib

OBJS = CharDistribution.o nsCharSetProber.o nsMBCSGroupProber.o \
	   JpCntx.o nsEscCharsetProber.o nsMBCSSM.o \
	   LangBulgarianModel.o nsEscSM.o nsSBCharSetProber.o \
	   LangCyrillicModel.o nsEUCJPProber.o nsSBCSGroupProber.o \
	   LangGreekModel.o nsEUCKRProber.o nsSJISProber.o \
	   LangHebrewModel.o nsEUCTWProber.o nsUniversalDetector.o \
	   LangHungarianModel.o nsGB2312Prober.o nsUTF8Prober.o \
	   LangThaiModel.o nsHebrewProber.o \
	   nsBig5Prober.o nsLatin1Prober.o

LIB_CHARDETECT = libchardetect.a
EXEC = detect_char_encode

CPPFLAGS := $(CPPFLAGS)
CPPFLAGS +=  -I./ -I./tables
#CPPFLAGS += -DDEBUG_chardet


all: $(EXEC)

$(EXEC): test.o impl.o $(LIB_CHARDETECT)
	$(CXX) $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) -D_REENTRANT -o $(EXEC) test.o impl.o $(LIB_CHARDETECT) -s

test.o: test.cpp

$(LIB_CHARDETECT): $(OBJS)
	$(AR) cru $@ $(OBJS)
	$(RANLIB) $@

.cpp.o:
	$(CXX) $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) -D_REENTRANT -c -o $@ $<

clean:
	rm -f *.o $(EXEC) $(LIB_CHARDETECT)
