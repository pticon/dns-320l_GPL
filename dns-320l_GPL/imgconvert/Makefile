OBJS = 	imgconvert.o \
		gd_gif_in.o \
		gd.o \
		gd_io_file.o \
		gd_io_dp.o \
		gd_io.o \
		gdhelpers.o \
		gd_security.o \
		png.o \
		pngread.o \
		pngerror.o \
		pngmem.o \
		crc32.c \
		zutil.o \
		inflate.o \
		inftrees.o \
		inffast.o \
		pngrio.o \
		pngrutil.o \
		pngrtran.o \
		pngtrans.o \
		pngget.o \
		adler32.o \
		pngset.o \
		tif_unix.o \
		tif_dir.o \
		tif_strip.o \
		tif_read.o \
		tif_close.o \
		tif_open.o \
		tif_error.o \
		tif_dirinfo.o \
		tif_compress.o \
		tif_warning.o \
		tif_swab.o \
		tif_dirread.o \
		tif_tile.o \
		tif_flush.o \
		tif_codec.o \
		tif_aux.o \
		tif_dirwrite.o \
		tif_write.o \
		tif_dumpmode.o \
		tif_lzw.o \
		tif_packbits.o \
		tif_thunder.o \
		tif_next.o \
		tif_predict.o \
		read_tif.o 

#CC = /usr/bin/gcc
#-DDBG
DEBUG_FLAG =
.SUFFIXES= .o .c

imgconvert: ${OBJS}
	${CC} -O2 ${DEBUG_FLAG} -Wall -o $@ -s ${OBJS} -lm

.c.o:
	${CC} -O2 ${DEBUG_FLAG} -Wall -c $<

clean:
	/bin/rm -f core *.o imgconvert
