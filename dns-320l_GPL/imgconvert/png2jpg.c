/* pngrewrite
   version 1.2.1  - 8 Feb 2003

  A quick and dirty utility to reduce unnecessarily large palettes 
  and bit depths in PNG image files.


  ** To compile in a unix-like environment, type something like:
  **   cc -o pngrewrite pngrewrite.c -lpng -lz -lm


  Web site: <http://www.pobox.com/~jason1/pngrewrite/>

  This program and source code may be used without restriction.

  Primary author: Jason Summers  <jason1@pobox.com>

  Contributing authors:

     Wayne Schlitt
       - Write grayscale images when possible
	   - Ability to sort palette by color usage
	   - Improved find_pal_entry performance

     Soren Anderson
	   - Changes to allow use in a unix-style pipeline
	   - Improved compatibility with MinGW and Cygwin
       - Maintain tIME chunks


  Pngrewrite will:

  * Remove any unused palette entries, and write a palette that is only as
    large as needed.

  * Remove (collapse) any duplicate palette entries.

  * Convert non-palette image to palette images, provided they contain no
    more than 256 different colors.

  * Write images as grayscale when possible.

  * Move any colors with transparency to the beginning of the palette, and
    write a tRNS chunk that is a small as possible.

  * Reduce the bit-depth (bits per pixel) as much as possible.

  All of this is just basic stuff that any respectable image writing program
  should always do automatically, but for some reason, many of them do not.


  Under no circumstances does pngrewrite change the actual pixel colors, or
  background color, or transparency of the image. If it ever does, that's a
  bug.

  --WARNING--
  This version of pngrewrite removes most extra (ancillary) information from
  the PNG file, such as text comments. Although this does make the file size
  smaller, the removed information may sometimes be important. For that
  reason, you should only use pngrewrite on image that were created by you.

  The only ancillary chunks that are NOT removed are:
     gAMA  - Image gamma setting
     sRGB  - srgb color space indicator
	 tIME  - creation time
	 pHYs  - physical pixel size
     bKGD and tRNS - Background color and transparency are maintained. The
        actual chunk may be modified according to the new color structure.

  If the original image was interlaced, the new one will also be interlaced.

  Pngrewrite will not work at all on images that have more than 256 colors.
  Colors with the same RGB values but a different level of transparency
  count as different colors. The background color counts as an extra color
  if it does not occur in the image.
  
  It will also not work at all on images that have a color depth of 16 bits,
  since they cannot have a palette.

  This is a very inefficient program. It is (relatively) slow, and uses tons
  of memory. To be specific, it uses about 5 bytes per pixel, no matter what
  the bit depth of the image is.

  This program is (hopefully) reasonably portable, and should compile
  without too much effort on most ANSI C compilers. It requires the libpng
  and zlib libraries.
*/
#define PNGREWRITEVERSION  "1.2.1"

#define PALSORT_BY_FREQUENCY


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* #include <values.h> */

#if defined( WIN32 ) && !defined (__GNUC__)
#include <search.h> /* for qsort */
#include <io.h>
#include <fcntl.h>
#endif

#if defined( WIN32 ) && defined (__GNUC__)
#  include <fcntl.h>
#endif

#ifndef WIN32
#include <unistd.h>  /* for isatty() */
#endif

/* #define PNG_DEBUG 1 */
#include "png.h"

typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
	unsigned int  count;
} tmppal_t;


tmppal_t tmppal[256];
int pal_used;
int new_bit_depth;
int valid_gray;

unsigned char *image1, *image2;
unsigned char **row_pointers;
int rowbytes, channels;
png_uint_32 width, height;
int bit_depth, color_type, interlace_type;
png_time savechunk_time; /*  S.A. */
/*  S.A.  - 30 bytes extra "garbage" safety zone, should use only 26 + \0 = 27  */
char time_as_string[57];
int ori_pal_size;

double image_gamma;
tmppal_t bkgd;             /* RGB background color */
int gray_trns;
unsigned char bkgd_pal;    /* new background color palette entry */
int srgb_intent;
png_uint_32 res_x,res_y;
int res_unit_type;

struct chunk_flags {
	unsigned int has_gama : 1;
	unsigned int has_bkgd : 1;
	unsigned int has_srgb : 1;
	unsigned int need_tIME_after_IDATs : 1;
	unsigned int has_phys : 1;
};
/*  initialize chunk-handling bitflags in read-png()  */
struct chunk_flags chnks;

#define PNG_READ_SIG_BYTES 7

int
read_png(char *file_name)
{
	png_structp png_ptr; //struct png_struct_def
	png_infop info_ptr;
	png_colorp ori_pal;
	int ori_bpp;
	unsigned char header[PNG_READ_SIG_BYTES];
	int is_png=0;
#ifdef WIN32
	int setmode_retval;
#endif

	FILE *fp;
	int i,j;
	png_color_16 *image_background;
	png_timep in_time;  /*  a struct POINTER  */
    extern png_time savechunk_time; /* NOT a pointer! S.A. */

	if(file_name != NULL)  {
		  if( (fp = fopen(file_name, "rb")) == NULL ) {
			fprintf(stderr, "Can't open file %s\n",file_name);
			return 0;
		  }
	} else {
		fp = stdin;

#if WIN32
		setmode_retval=setmode( fileno(fp), O_BINARY);
#endif

	}

	fread(header, 1, (int)PNG_READ_SIG_BYTES, fp);
	is_png = !png_sig_cmp(header, 0, PNG_READ_SIG_BYTES);
	 if(!is_png) {
		fprintf(stderr, "Not a PNG file: initial %d bytes are \"%s\"",
		   PNG_READ_SIG_BYTES, header);
		return 0;
	 }


	chnks.has_gama=0;
	chnks.has_bkgd=0;
	chnks.has_srgb=0;
	chnks.need_tIME_after_IDATs=0;
	chnks.has_phys=0;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL);

	if(png_ptr == NULL) {
		fprintf(stderr, "Error creating read_struct\n");
		if(file_name)
			fclose(fp);
		return 0;
	}

	png_set_sig_bytes(png_ptr, PNG_READ_SIG_BYTES);

	info_ptr = png_create_info_struct(png_ptr);
	png_debug(0, "Creating png info struct");
	if (info_ptr == NULL) {
		if(file_name)
			fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return 0;
	}

	png_debug(0, "Setting default png read function");
	png_set_read_fn(png_ptr, fp, NULL);

	if (setjmp(png_ptr->jmpbuf)) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		if(file_name)
			fclose(fp);
		fprintf(stderr, "Error reading file on descriptor '%i'\n", fileno(fp));
		return 0;
	}

	png_debug(0, "Starting png_init_io");
	png_init_io(png_ptr, fp);

	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		&interlace_type, NULL, NULL);

	if(bit_depth>8) {
		fprintf(stderr, "This image can't be converted because it has 16 bits/sample.\n");
		goto abort1;
	}


    if (color_type == PNG_COLOR_TYPE_PALETTE) {
		ori_pal_size=0;
		ori_bpp = bit_depth;

		/* we only do this to get the palette size so we can print it */
		png_get_PLTE(png_ptr,info_ptr,&ori_pal,&ori_pal_size);

		fprintf(stderr, "original palette size:   %3d, %2d bpp\n",ori_pal_size,ori_bpp);

        png_set_expand(png_ptr);
	}
	else {
		/* figure out bits per pixel so we can print it */
		ori_bpp= bit_depth*png_get_channels(png_ptr,info_ptr);
		fprintf(stderr, "original palette size: [n/a], %2d bpp\n",ori_bpp);
	}

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);
	/* if (bit_depth == 16)
        png_set_strip_16(png_ptr); */
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

	if (png_get_bKGD(png_ptr, info_ptr, &image_background)) {
		chnks.has_bkgd=1;
		bkgd.red=   (unsigned char)image_background->red;
		bkgd.green= (unsigned char)image_background->green;
		bkgd.blue=  (unsigned char)image_background->blue;
		bkgd.alpha=255;
	}

	if (png_get_gAMA(png_ptr, info_ptr, &image_gamma)) {
		chnks.has_gama=1;
	}

	if (png_get_sRGB(png_ptr, info_ptr, &srgb_intent)) {
		chnks.has_srgb=1;
	}

	if(png_get_valid(png_ptr,info_ptr,PNG_INFO_pHYs)) {
		chnks.has_phys=1;
		png_get_pHYs(png_ptr,info_ptr,&res_x,&res_y,&res_unit_type);
		if(res_x<1 || res_y<1) chnks.has_phys=0;
	}


/*  S.A.  ..............................  */
    if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tIME))
    {
	   if(png_get_tIME(png_ptr, info_ptr, &in_time) == PNG_INFO_tIME)
	   {
		   sprintf(time_as_string, "%s",
	          png_convert_to_rfc1123(png_ptr, in_time));
	       savechunk_time = *in_time;
       }
    } else {  chnks.need_tIME_after_IDATs=1;  }


    png_read_update_info(png_ptr, info_ptr);

	rowbytes=png_get_rowbytes(png_ptr, info_ptr);
	channels=(int)png_get_channels(png_ptr, info_ptr);

	if(channels<3 || channels>4) {
		fprintf(stderr, "internal error: channels=%d\n",channels);
		goto abort1;
	}

	fprintf(stderr,"Image size is %ldx%ld  memory required=%.3fMB\n",
		width, height,
		(height*rowbytes + height*sizeof(unsigned char*) + width*height) / (1024. * 1024.) );

	image1= (unsigned char*)malloc(height*rowbytes);
	row_pointers = (unsigned char**)malloc(height * sizeof(unsigned char*));
	if(!image1 || !row_pointers) {
		fprintf(stderr, "Unable to allocate memory for image\n");
		goto abort1;
	}

	for(i=0;i<(int)height;i++) {
		row_pointers[i] = &image1[rowbytes*i];
	}

	png_read_image(png_ptr, row_pointers);
	printf("2rowbytes %d\n",rowbytes);
for(i=0;i<height*rowbytes;i++)
{
	//for(j=0;j<;j++)
		printf("%d-",image1[i]);
		if((i % (95) == 0) && i!=0)
			printf("\n");
}
	png_read_end(png_ptr, info_ptr);

/* S.A.  .................................  */
      if(chnks.need_tIME_after_IDATs &&
           png_get_valid(png_ptr,info_ptr,PNG_INFO_tIME))
      {
		   if(png_get_tIME(png_ptr, info_ptr, &in_time) == PNG_INFO_tIME)
		   {
			   sprintf(time_as_string, "%s",
				  png_convert_to_rfc1123(png_ptr, in_time));
			   savechunk_time = *in_time;
		   }
	  }
printf("info_ptr->width %d\n",info_ptr->width);
printf("info_ptr->height %d\n",info_ptr->height);
printf("sizeof(info_ptr->row_pointers) %d\n",sizeof(info_ptr->row_pointers));
for(i=0;i<width;i++)
	for(j=0;j<height;j++)
		printf("%x-",info_ptr->row_pointers[i][j]);
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	if(file_name)
		fclose(fp);

	return 1;

abort1:
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	if(file_name)
		fclose(fp);
	return 0;
}

#if 0
int
write_new_png(char *fn)
{
    png_structp  png_ptr;
    png_infop  info_ptr;
	png_color palette[256];
	unsigned char trans[256];
	FILE *fp;
	int num_trans;
	int i;
	png_color_16 newtrns;
	png_color_16 newbackground;

	memset(&newtrns      ,0,sizeof(png_color_16));
	memset(&newbackground,0,sizeof(png_color_16));

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
      NULL, NULL);
	if (!png_ptr) {
		fprintf(stderr, "out of memory\n");
		return 0;
	}

	if(fn) {
		fp=fopen(fn,"wb");
		if(!fp) {
			fprintf(stderr, "can't write to file %s\n",fn);
			png_destroy_write_struct(&png_ptr, NULL);
			return 0;
		}
	}
	else { /*  write to stdout */
		fp = stdout;
#ifdef WIN32
		setmode( fileno(fp), O_BINARY);
#endif
	}


    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
		fprintf(stderr, "can't create info struct\n");
        goto abort1;
    }

	if (setjmp(png_ptr->jmpbuf)) {
		fprintf(stderr, "Error writing file\n");
		goto abort1;
	}

	png_init_io(png_ptr, fp);


	if( valid_gray ) {
	    png_set_IHDR(png_ptr, info_ptr, width, height, new_bit_depth,
			 PNG_COLOR_TYPE_GRAY, interlace_type,
			 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		if(gray_trns>=0) {
			newtrns.gray = gray_trns;
			png_set_tRNS(png_ptr, info_ptr, NULL, 1, &newtrns);
		}

	}
	else {
	    png_set_IHDR(png_ptr, info_ptr, width, height, new_bit_depth,
			 PNG_COLOR_TYPE_PALETTE, interlace_type,
			 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		/* ... set palette colors ... */

		num_trans=0;  /* number of transparent palette entries */

		for(i=0;i<pal_used;i++) {
			palette[i].red=tmppal[i].red;
			palette[i].green=tmppal[i].green;
			palette[i].blue=tmppal[i].blue;

			trans[i]=tmppal[i].alpha;
			if(trans[i]<255) num_trans=i+1;
		}
		png_set_PLTE(png_ptr, info_ptr, palette, pal_used);

		if(num_trans>0) {
			png_set_tRNS(png_ptr, info_ptr, trans, num_trans, 0);
		}
	}
	

	if(chnks.has_gama) png_set_gAMA(png_ptr, info_ptr, image_gamma);
	if(chnks.has_srgb) png_set_sRGB(png_ptr, info_ptr, srgb_intent);
	if(chnks.has_phys) png_set_pHYs(png_ptr, info_ptr, res_x, res_y, res_unit_type);

	if(chnks.has_bkgd) {
		if(valid_gray)
			newbackground.gray = bkgd_pal;
		else
			newbackground.index = bkgd_pal;
		png_set_bKGD(png_ptr, info_ptr, &newbackground);
	}
	

	png_write_info(png_ptr, info_ptr);

	png_set_packing(png_ptr);

	/* re-use row_pointers array */
	for(i=0;i<(int)height;i++) {
		row_pointers[i]= &image2[i*width];
	}

	png_write_image(png_ptr, row_pointers);
/*  S.A.  ............................  */
    if(savechunk_time.year)
    {
		//fprintf(stderr, "inputchunk tIME copied:  %s\n", time_as_string);
        png_set_tIME(png_ptr, info_ptr, &savechunk_time);
	}

	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, (png_infopp)NULL);

	if(fn)
		fclose(fp);

	free(row_pointers);
	free(image2);
	return 1;

abort1:
	png_destroy_write_struct(&png_ptr, NULL);
	if(fn)
		fclose(fp);
	return 0;
}
#endif
#if 0
/* set ignorealpha to 1 if you don't care if the alpha value matches
 * (for the background) */
int
add_to_palette(tmppal_t *pal, tmppal_t *color, int *pal_used, int ignorealpha)
{
	int used;

	used=(*pal_used);
	if(used>=256) {
		fprintf(stderr, "This image can't be converted because it has more than 256 colors.\n");
		return 0;
	}
	pal[used].red=color->red;
	pal[used].green=color->green;
	pal[used].blue=color->blue;
	pal[used].alpha= ignorealpha?255:color->alpha;
	pal[used].count = 1;
	(*pal_used)++;
	return 1;
}
#endif
/*
void debug_print_pal(tmppal_t *pal, int used)
{
	int i;
	for(i=0;i<used;i++)
		fprintf(stderr, "%d. %d %d %d %d\n",i,pal[i].red,pal[i].green,pal[i].blue,pal[i].alpha);
}
*/

#if 0
/* Sort the palette to put transparent colors first,
 * then sort by how frequently the color is used.  Sorting by frequency
 * will often help the png filters make the image more compressible.
 * It also makes it easier for people to see which colors aren't used much
 * and allow them to manually reduce the color palette. */
int palsortfunc(const void* p1, const void* p2)
{
	tmppal_t *e1,*e2;
	int s1,s2;

	e1=(tmppal_t*)p1;
	e2=(tmppal_t*)p2;

	if(e1->alpha==255 && e2->alpha<255) return 1;
	if(e1->alpha<255 && e2->alpha==255) return -1;

#ifdef PALSORT_BY_FREQUENCY
	if(e1->count<e2->count) return 1;
	if(e1->count>e2->count) return -1;
#endif

	s1=e1->red+e1->green+e1->blue;
	s2=e2->red+e2->green+e2->blue;
	if(s1>s2) return 1;
	if(s1<s2) return -1;
	return 0;
}
#endif
#if 0
int
find_pal_entry(unsigned char r, unsigned char g, unsigned char b, 
				   unsigned char a, int ignorealpha, int reset)
{
	int i;
	static int prev_entry;
	static int prev_r = (-1), prev_g = (-1), prev_b = (-1);
	static int prev_a = (-1);

	if(reset) {
		prev_r = prev_g = prev_b = prev_a = (-1); 
		return -1;
	}

	if(prev_r == r && prev_g == g  && prev_b == b && prev_a == a)
	    return prev_entry;

	for(i=0;i<pal_used;i++) {
		if(tmppal[i].red==r && tmppal[i].green==g && tmppal[i].blue==b
		   && (tmppal[i].alpha==a || ignorealpha)) {

		    prev_r = r;
		    prev_g = g;
		    prev_b = b;
		    prev_a = a;
		    prev_entry = i;
		    
		    return i;
		}
	}
	return (-1);
}
#endif
#if 0
int make_new_png(void)
{
	tmppal_t thispix;
	unsigned char *p;
	int x,y;
	int palent;
	int i;
	int gray_trns_palentry; /* temp palette entry */
	unsigned char gray_trns_shade; /* 0 through 255 */

	pal_used=0;

	for(y=0;y<(int)height;y++) {
		for(x=0;x<(int)width;x++) {
			p=&row_pointers[y][x*channels];
				thispix.red=p[0];
				thispix.green=p[1];
				thispix.blue=p[2];
				if(channels==4) thispix.alpha=p[3];
				else thispix.alpha=255;
			palent = find_pal_entry(p[0],p[1],p[2],thispix.alpha,0,0);
			if(palent<0) {
			    if(!add_to_palette(tmppal,&thispix,&pal_used,0))
				return 0;
			}
			else
			    tmppal[palent].count++;
		}
	}

	if(chnks.has_bkgd) {
		bkgd.alpha= 255; /* actually not needed */
		palent = find_pal_entry(bkgd.red,bkgd.green,bkgd.blue,bkgd.alpha,1,0);
		if(palent<0) {
			add_to_palette(tmppal,&bkgd,&pal_used,1);
		}
		else
		    tmppal[palent].count++;
	}


	/* determine bit depth and whether we should do grayscale or palette */
	new_bit_depth=8;
	if(pal_used<=16) new_bit_depth=4;
	if(pal_used<=4) new_bit_depth=2;
	if(pal_used<=2) new_bit_depth=1;

	/* figure out if this is a valid grayscale image */

	gray_trns_palentry = -1;

	valid_gray = 1;
	for( i = 0; i < pal_used; i++ ) {

	    if( tmppal[i].red != tmppal[i].green
		   || tmppal[i].red != tmppal[i].blue)
	    {
			// not gray
			valid_gray = 0;
			break;
	    }

		if(tmppal[i].alpha!=255 && tmppal[i].alpha!=0) {
			valid_gray=0;
			break;
		}

		if(tmppal[i].alpha == 0) {
			if(gray_trns_palentry != -1) { valid_gray=0; break; } // multiple transparent colors
			gray_trns_palentry = i;  // binary transparency ok (so far)
			gray_trns_shade = tmppal[i].red;
		}


	    switch( tmppal[i].red ) {
	    case 0: case 255:
			continue;
			//break;

	    case 85: case 170:
			if( new_bit_depth >= 2 )
				continue;
			break;
		
	    case  17:  case 34:  case 51:  case 68:
	    case 102: case 119: case 136: case 153:
	    case 187: case 204: case 221: case 238:
			if( new_bit_depth >= 4 )
				continue;
			break;

	    default:
			if( new_bit_depth >= 8 )
				continue;
				//break;
		}
	    
	    valid_gray = 0;
	    break;
	}

	// One thing the above doesn't check for is a nontransparent
	// grayscale that's the same as the transparent grayscale color.
	// In this case we have to use palette color.
	if(valid_gray && gray_trns_palentry != -1) {
		if(-1 != find_pal_entry(gray_trns_shade,gray_trns_shade,gray_trns_shade,255,0,0)) {
			valid_gray = 0;
		}
	}

	/* put the palette in a good order */

	if(valid_gray) {
		// If grayscale, create a "fake" palette consisting of all
		// available gray shades.
		
		switch(new_bit_depth) {
		case 8:
			pal_used=256;
			for(i=0;i<pal_used;i++) {
				tmppal[i].red = tmppal[i].green = tmppal[i].blue = i;
				tmppal[i].alpha = 255;
			}
			break;
		case 4:
			pal_used=16;
			for(i=0;i<pal_used;i++) {
				tmppal[i].red = tmppal[i].green = tmppal[i].blue = i*17;
				tmppal[i].alpha = 255;
			}
			break;
		case 2:
			pal_used=4;
			for(i=0;i<pal_used;i++) {
				tmppal[i].red = tmppal[i].green = tmppal[i].blue = i*85;
				tmppal[i].alpha = 255;
			}
			break;
		case 1:
			pal_used=2;
			for(i=0;i<pal_used;i++) {
				tmppal[i].red = tmppal[i].green = tmppal[i].blue = i*255;
				tmppal[i].alpha = 255;
			}
			break;
		}

		gray_trns = -1;
		// handle grayscale binary transparency
		if(gray_trns_palentry != -1) {
			gray_trns=find_pal_entry(gray_trns_shade,gray_trns_shade,gray_trns_shade,255,0,0);
			if(gray_trns == -1) {
				fprintf(stderr,"internal error: can't find transparent grayscale color\n");
				exit(1);
			}
			tmppal[gray_trns].alpha = 0;
		}

	}
	else {
		qsort((void*)tmppal,pal_used,sizeof(tmppal_t),palsortfunc);
	}


	if(valid_gray)
		fprintf(stderr, "saving as grayscale:          %2d bpp\n",new_bit_depth);
	else
		fprintf(stderr, "new palette size:        %3d, %2d bpp\n",pal_used,new_bit_depth);



	/* reset the find_pal_entry() cache */
	find_pal_entry(0,0,0,0,0,1);
	//palent = find_pal_entry(tmppal[0].red,tmppal[0].green,tmppal[0].blue,thispix.alpha,0);
	//palent = find_pal_entry(tmppal[1].red,tmppal[1].green,tmppal[1].blue,thispix.alpha,0);

	/* debug_print_pal(tmppal,pal_used); */

	/* now create the new image */
	image2 = (unsigned char*)malloc(width*height);
	if(!image2) {
		fprintf(stderr, "out of memory\n");
		return 0;
	}

	for(y=0;y<(int)height;y++) {
		for(x=0;x<(int)width;x++) {
			p=&row_pointers[y][x*channels];

			palent = find_pal_entry(p[0],p[1],p[2],(unsigned char)((channels==4)?p[3]:255),0,0);
			if(palent<0) {
				fprintf(stderr, "internal error: can't locate palette entry\n");
				return 0;
			}
			image2[y*width + x] = (unsigned char)palent;
		}
	}

	if(chnks.has_bkgd) {
		palent = find_pal_entry(bkgd.red,bkgd.green,bkgd.blue,255,1,0);
		if(palent<0) {
			fprintf(stderr, "internal error: can't locale palette entry for bkgd\n");
			return 0;
		}
		bkgd_pal = (unsigned char)palent;
	}

	free(image1);

	return 1;
}
#endif
int
main(int argc,char **argv)
{
	char *out_name = argv[argc - 1];
	char *input;
	char specstamp[38]="";
	int  i,j;

	if( !(argc==3) ) {
		if(!(  argv[1] && !isatty(fileno(stdin))  )) {
		   fprintf(stderr, "\npngrewrite v" PNGREWRITEVERSION ": PNG image palette optimizer%s\n", specstamp);
		   fprintf(stderr,
	"Usage: %s infile.png outfile.png OR |%s outfile.png\n",argv[0], argv[0]);
		   exit(1);
	    }
	}

	if((argc==2 && !isatty( fileno(stdin) )) )  {
		input = NULL;
	} else {
		input = argv[1];
		if(!strcmp(input,"-")) input = NULL;
	}

	if(!strcmp(out_name,"-")) out_name = NULL;

	if(read_png(input)) {
		//printf("info_ptr.pixel_depth %d\n",png_infoppixel_depth);
		//for(i=0;i<info_ptr)
//		if(make_new_png()) {
//			write_new_png(out_name);
//		}

	}
	return 0;
}
