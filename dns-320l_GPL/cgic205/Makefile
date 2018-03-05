
OBJS = cgic.o cgictest.o capture.o

CFLAGS   = -Wall -O
LDFLAGS = -DALPHA -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
STRIP = -s
LIBS = -L./
INCS = -I./

SHARE = libcgic.so
cc = ${CC}
scc = ${CC} -shared

.SUFFIXES =
.SUFFIXES = .o .c

all: ${OBJS}
	${cc} cgic.o capture.o $(CFLAGS) $(LDFLAGS) $(STRIP) $(LIBS) $(INCS) -o capture
	${cc} cgic.o cgictest.o $(CFLAGS) $(LDFLAGS) $(STRIP) $(LIBS) $(INCS) -o cgictest.cgi
	$(scc) cgic.o $(CFLAGS) $(LDFLAGS) -o $(SHARE)

cgic.o: cgic.c
	${cc} $(CFLAGS) $(LDFLAGS) $(INCS) -fpic -c cgic.c -o cgic.o
	
.c.o:
	${cc} $(CFLAGS) $(LDFLAGS) $(INCS) -c $< -o $@
	
clean:
	rm -f *.o *.so cgictest.cgi capture

