// A BMP truecolor to JPEG encoder
// Copyright 1999 Cristi Cuturicu
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gd.h"
#include "jtypes.h"
#include "jglobals.h"
#include "jtables.h"
#include "extern_var.h"

colorRGB *RGB_buffer; //image to be encoded
int	bitpersample;

#include "png.h"
typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
	unsigned int  count;
} tmppal_t;
png_uint_32 width, height;
int bit_depth,color_type,interlace_type,ori_pal_size;
tmppal_t bkgd;             /* RGB background color */
double image_gamma;
int srgb_intent;
png_uint_32 res_x,res_y;
int res_unit_type;
char time_as_string[57];
int rowbytes, channels;
unsigned char *image1;
unsigned char **row_pointers;
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


// ==========Read Tif==================

#include "tif_config.h"
#include "tiffio.h"
#include "tiffiop.h"
#include "read_tif.h"
// ==========Read Tif End==============

//#define DEBUG
#ifdef DEBUG
#define	DB(x)	x
#else
#define	DB(x)	
#endif

void write_APP0info()
//Nothing to overwrite for APP0info
{
 writeword(APP0info.marker);
 writeword(APP0info.length);
 writebyte('J');writebyte('F');writebyte('I');writebyte('F');writebyte(0);
 writebyte(APP0info.versionhi);writebyte(APP0info.versionlo);
 writebyte(APP0info.xyunits);
 writeword(APP0info.xdensity);writeword(APP0info.ydensity);
 writebyte(APP0info.thumbnwidth);writebyte(APP0info.thumbnheight);
}

void write_SOF0info()
// We should overwrite width and height
{
 writeword(SOF0info.marker);
 writeword(SOF0info.length);
 writebyte(SOF0info.precision);
 writeword(SOF0info.height);writeword(SOF0info.width);
 writebyte(SOF0info.nrofcomponents);
 writebyte(SOF0info.IdY);writebyte(SOF0info.HVY);writebyte(SOF0info.QTY);
 writebyte(SOF0info.IdCb);writebyte(SOF0info.HVCb);writebyte(SOF0info.QTCb);
 writebyte(SOF0info.IdCr);writebyte(SOF0info.HVCr);writebyte(SOF0info.QTCr);
}

void write_DQTinfo()
{
 BYTE i;
 writeword(DQTinfo.marker);
 writeword(DQTinfo.length);
 writebyte(DQTinfo.QTYinfo);for (i=0;i<64;i++) writebyte(DQTinfo.Ytable[i]);
 writebyte(DQTinfo.QTCbinfo);for (i=0;i<64;i++) writebyte(DQTinfo.Cbtable[i]);
}

void set_quant_table(BYTE *basic_table,BYTE scale_factor,BYTE *newtable)
// Set quantization table and zigzag reorder it
{
 BYTE i;
 long temp;
 for (i = 0; i < 64; i++) {
      temp = ((long) basic_table[i] * scale_factor + 50L) / 100L;
	/* limit the values to the valid range */
    if (temp <= 0L) temp = 1L;
    if (temp > 255L) temp = 255L; /* limit to baseline range if requested */
    newtable[zigzag[i]] = (WORD) temp;
			  }
}

void set_DQTinfo()
{
 BYTE scalefactor=50;// scalefactor controls the visual quality of the image
			 // the smaller is, the better image we'll get, and the smaller
			 // compression we'll achieve
 DQTinfo.marker=0xFFDB;
 DQTinfo.length=132;
 DQTinfo.QTYinfo=0;
 DQTinfo.QTCbinfo=1;
 set_quant_table(std_luminance_qt,scalefactor,DQTinfo.Ytable);
 set_quant_table(std_chrominance_qt,scalefactor,DQTinfo.Cbtable);
}

void write_DHTinfo()
{
 BYTE i;
 writeword(DHTinfo.marker);
 writeword(DHTinfo.length);
 writebyte(DHTinfo.HTYDCinfo);
 for (i=0;i<16;i++)  writebyte(DHTinfo.YDC_nrcodes[i]);
 for (i=0;i<=11;i++) writebyte(DHTinfo.YDC_values[i]);
 writebyte(DHTinfo.HTYACinfo);
 for (i=0;i<16;i++)  writebyte(DHTinfo.YAC_nrcodes[i]);
 for (i=0;i<=161;i++) writebyte(DHTinfo.YAC_values[i]);
 writebyte(DHTinfo.HTCbDCinfo);
 for (i=0;i<16;i++)  writebyte(DHTinfo.CbDC_nrcodes[i]);
 for (i=0;i<=11;i++)  writebyte(DHTinfo.CbDC_values[i]);
 writebyte(DHTinfo.HTCbACinfo);
 for (i=0;i<16;i++)  writebyte(DHTinfo.CbAC_nrcodes[i]);
 for (i=0;i<=161;i++) writebyte(DHTinfo.CbAC_values[i]);
}

void set_DHTinfo()
{
 BYTE i;
 DHTinfo.marker=0xFFC4;
 DHTinfo.length=0x01A2;
 DHTinfo.HTYDCinfo=0;
 for (i=0;i<16;i++)  DHTinfo.YDC_nrcodes[i]=std_dc_luminance_nrcodes[i+1];
 for (i=0;i<=11;i++)  DHTinfo.YDC_values[i]=std_dc_luminance_values[i];
 DHTinfo.HTYACinfo=0x10;
 for (i=0;i<16;i++)  DHTinfo.YAC_nrcodes[i]=std_ac_luminance_nrcodes[i+1];
 for (i=0;i<=161;i++) DHTinfo.YAC_values[i]=std_ac_luminance_values[i];
 DHTinfo.HTCbDCinfo=1;
 for (i=0;i<16;i++)  DHTinfo.CbDC_nrcodes[i]=std_dc_chrominance_nrcodes[i+1];
 for (i=0;i<=11;i++)  DHTinfo.CbDC_values[i]=std_dc_chrominance_values[i];
 DHTinfo.HTCbACinfo=0x11;
 for (i=0;i<16;i++)  DHTinfo.CbAC_nrcodes[i]=std_ac_chrominance_nrcodes[i+1];
 for (i=0;i<=161;i++) DHTinfo.CbAC_values[i]=std_ac_chrominance_values[i];
}

void write_SOSinfo()
//Nothing to overwrite for SOSinfo
{
 writeword(SOSinfo.marker);
 writeword(SOSinfo.length);
 writebyte(SOSinfo.nrofcomponents);
 writebyte(SOSinfo.IdY);writebyte(SOSinfo.HTY);
 writebyte(SOSinfo.IdCb);writebyte(SOSinfo.HTCb);
 writebyte(SOSinfo.IdCr);writebyte(SOSinfo.HTCr);
 writebyte(SOSinfo.Ss);writebyte(SOSinfo.Se);writebyte(SOSinfo.Bf);
}

void write_comment(BYTE *comment)
{
 WORD i,length;
 writeword(0xFFFE); //The COM marker
 length=strlen((const char *)comment);
 writeword(length+2);
 for (i=0;i<length;i++) writebyte(comment[i]);
}

void writebits(bitstring bs)
// A portable version; it should be done in assembler
{
 WORD value;
 SBYTE posval;//bit position in the bitstring we read, should be<=15 and >=0
 value=bs.value;
 posval=bs.length-1;
	while (posval>=0)
	{
   		if (value & mask[posval])
   			bytenew|=mask[bytepos];
   		posval--;
   		bytepos--;
   		if (bytepos<0) 
   		{ 
   			if (bytenew==0xFF) {
   				writebyte(0xFF);
   				writebyte(0);
   			}
		    else {
		    	writebyte(bytenew);
		    }
		    bytepos=7;
		    bytenew=0;
		}
  	}
}

void compute_Huffman_table(BYTE *nrcodes,BYTE *std_table,bitstring *HT)
{
 BYTE k,j;
 BYTE pos_in_table;
 WORD codevalue;
 codevalue=0; pos_in_table=0;
 for (k=1;k<=16;k++)
   {
     for (j=1;j<=nrcodes[k];j++) {HT[std_table[pos_in_table]].value=codevalue;
				  HT[std_table[pos_in_table]].length=k;
				  pos_in_table++;
				  codevalue++;
				 }
     codevalue*=2;
   }
}
void init_Huffman_tables()
{
 compute_Huffman_table(std_dc_luminance_nrcodes,std_dc_luminance_values,YDC_HT);
 compute_Huffman_table(std_dc_chrominance_nrcodes,std_dc_chrominance_values,CbDC_HT);
 compute_Huffman_table(std_ac_luminance_nrcodes,std_ac_luminance_values,YAC_HT);
 compute_Huffman_table(std_ac_chrominance_nrcodes,std_ac_chrominance_values,CbAC_HT);
}

void exitmessage(char *error_message)
{
 printf("%s\n",error_message);exit(EXIT_FAILURE);
}

void set_numbers_category_and_bitcode()
{
 SDWORD nr;
 SDWORD nrlower,nrupper;
 //BYTE cat,value;
 BYTE cat;

 category_alloc=(BYTE *)malloc(65535*sizeof(BYTE));
 if (category_alloc==NULL) exitmessage("Not enough memory.");
 category=category_alloc+32767; //allow negative subscripts
 bitcode_alloc=(bitstring *)malloc(65535*sizeof(bitstring));
 if (bitcode_alloc==NULL) exitmessage("Not enough memory.");
 bitcode=bitcode_alloc+32767;
 nrlower=1;nrupper=2;
 for (cat=1;cat<=15;cat++) {
				 //Positive numbers
				 for (nr=nrlower;nr<nrupper;nr++)
				  { category[nr]=cat;
				bitcode[nr].length=cat;
				bitcode[nr].value=(WORD)nr;
				  }
				 //Negative numbers
				 for (nr=-(nrupper-1);nr<=-nrlower;nr++)
				  { category[nr]=cat;
				bitcode[nr].length=cat;
				bitcode[nr].value=(WORD)(nrupper-1+nr);
				  }
				 nrlower<<=1;
				 nrupper<<=1;
			   }
}

void precalculate_YCbCr_tables()
{
 WORD R,G,B;
 for (R=0;R<=255;R++) {YRtab[R]=(SDWORD)(65536*0.299+0.5)*R;
			   CbRtab[R]=(SDWORD)(65536*-0.16874+0.5)*R;
			   CrRtab[R]=(SDWORD)(32768)*R;
			  }
 for (G=0;G<=255;G++) {YGtab[G]=(SDWORD)(65536*0.587+0.5)*G;
			   CbGtab[G]=(SDWORD)(65536*-0.33126+0.5)*G;
			   CrGtab[G]=(SDWORD)(65536*-0.41869+0.5)*G;
			  }
 for (B=0;B<=255;B++) {YBtab[B]=(SDWORD)(65536*0.114+0.5)*B;
			   CbBtab[B]=(SDWORD)(32768)*B;
			   CrBtab[B]=(SDWORD)(65536*-0.08131+0.5)*B;
			  }
}

// Using a bit modified form of the FDCT routine from IJG's C source:
// Forward DCT routine idea taken from Independent JPEG Group's C source for
// JPEG encoders/decoders

/* For float AA&N IDCT method, divisors are equal to quantization
   coefficients scaled by scalefactor[row]*scalefactor[col], where
   scalefactor[0] = 1
   scalefactor[k] = cos(k*PI/16) * sqrt(2)    for k=1..7
   We apply a further scale factor of 8.
   What's actually stored is 1/divisor so that the inner loop can
   use a multiplication rather than a division. */
void prepare_quant_tables()
{
 double aanscalefactor[8] = {1.0, 1.387039845, 1.306562965, 1.175875602,
			   1.0, 0.785694958, 0.541196100, 0.275899379};
 BYTE row, col;
 BYTE i = 0;
 for (row = 0; row < 8; row++)
 {
   for (col = 0; col < 8; col++)
     {
       fdtbl_Y[i] = (float) (1.0 / ((double) DQTinfo.Ytable[zigzag[i]] *
			  aanscalefactor[row] * aanscalefactor[col] * 8.0));
       fdtbl_Cb[i] = (float) (1.0 / ((double) DQTinfo.Cbtable[zigzag[i]] *
			  aanscalefactor[row] * aanscalefactor[col] * 8.0));

	   i++;
     }
 }
}

void fdct_and_quantization(SBYTE *data,float *fdtbl,SWORD *outdata)
{
  float tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  float tmp10, tmp11, tmp12, tmp13;
  float z1, z2, z3, z4, z5, z11, z13;
  float *dataptr;
  float datafloat[64];
  float temp;
  SBYTE ctr;
  BYTE i;
  for (i=0;i<64;i++) datafloat[i]=data[i];

  /* Pass 1: process rows. */
  dataptr=datafloat;
  for (ctr = 7; ctr >= 0; ctr--) {
	tmp0 = dataptr[0] + dataptr[7];
    tmp7 = dataptr[0] - dataptr[7];
    tmp1 = dataptr[1] + dataptr[6];
    tmp6 = dataptr[1] - dataptr[6];
    tmp2 = dataptr[2] + dataptr[5];
    tmp5 = dataptr[2] - dataptr[5];
    tmp3 = dataptr[3] + dataptr[4];
    tmp4 = dataptr[3] - dataptr[4];

	/* Even part */

    tmp10 = tmp0 + tmp3;	/* phase 2 */
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;

    dataptr[0] = tmp10 + tmp11; /* phase 3 */
    dataptr[4] = tmp10 - tmp11;

    z1 = (tmp12 + tmp13) * ((float) 0.707106781); /* c4 */
	dataptr[2] = tmp13 + z1;	/* phase 5 */
    dataptr[6] = tmp13 - z1;

    /* Odd part */

    tmp10 = tmp4 + tmp5;	/* phase 2 */
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

	/* The rotator is modified from fig 4-8 to avoid extra negations. */
    z5 = (tmp10 - tmp12) * ((float) 0.382683433); /* c6 */
    z2 = ((float) 0.541196100) * tmp10 + z5; /* c2-c6 */
    z4 = ((float) 1.306562965) * tmp12 + z5; /* c2+c6 */
    z3 = tmp11 * ((float) 0.707106781); /* c4 */

    z11 = tmp7 + z3;		/* phase 5 */
    z13 = tmp7 - z3;

    dataptr[5] = z13 + z2;	/* phase 6 */
    dataptr[3] = z13 - z2;
	dataptr[1] = z11 + z4;
    dataptr[7] = z11 - z4;

    dataptr += 8;		/* advance pointer to next row */
  }

  /* Pass 2: process columns. */

  dataptr = datafloat;
  for (ctr = 7; ctr >= 0; ctr--) {
    tmp0 = dataptr[0] + dataptr[56];
    tmp7 = dataptr[0] - dataptr[56];
    tmp1 = dataptr[8] + dataptr[48];
    tmp6 = dataptr[8] - dataptr[48];
    tmp2 = dataptr[16] + dataptr[40];
    tmp5 = dataptr[16] - dataptr[40];
    tmp3 = dataptr[24] + dataptr[32];
    tmp4 = dataptr[24] - dataptr[32];

    /* Even part */

    tmp10 = tmp0 + tmp3;	/* phase 2 */
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;

    dataptr[0] = tmp10 + tmp11; /* phase 3 */
    dataptr[32] = tmp10 - tmp11;

	z1 = (tmp12 + tmp13) * ((float) 0.707106781); /* c4 */
    dataptr[16] = tmp13 + z1; /* phase 5 */
    dataptr[48] = tmp13 - z1;

    /* Odd part */

    tmp10 = tmp4 + tmp5;	/* phase 2 */
    tmp11 = tmp5 + tmp6;
    tmp12 = tmp6 + tmp7;

    /* The rotator is modified from fig 4-8 to avoid extra negations. */
	z5 = (tmp10 - tmp12) * ((float) 0.382683433); /* c6 */
    z2 = ((float) 0.541196100) * tmp10 + z5; /* c2-c6 */
    z4 = ((float) 1.306562965) * tmp12 + z5; /* c2+c6 */
    z3 = tmp11 * ((float) 0.707106781); /* c4 */

    z11 = tmp7 + z3;		/* phase 5 */
    z13 = tmp7 - z3;

    dataptr[40] = z13 + z2; /* phase 6 */
	dataptr[24] = z13 - z2;
    dataptr[8] = z11 + z4;
    dataptr[56] = z11 - z4;

    dataptr++;			/* advance pointer to next column */
  }

// Quantize/descale the coefficients, and store into output array
 for (i = 0; i < 64; i++) {
 /* Apply the quantization and scaling factor */
			   temp = datafloat[i] * fdtbl[i];
/* Round to nearest integer.
   Since C does not specify the direction of rounding for negative
   quotients, we have to force the dividend positive for portability.
   The maximum coefficient size is +-16K (for 12-bit data), so this
   code should work for either 16-bit or 32-bit ints.
*/
	   outdata[i] = (SWORD) ((SWORD)(temp + 16384.5) - 16384);
			  }
}

void process_DU(SBYTE *ComponentDU,float *fdtbl,SWORD *DC,
		bitstring *HTDC,bitstring *HTAC)
{
 bitstring EOB=HTAC[0x00];
 bitstring M16zeroes=HTAC[0xF0];
 BYTE i;
 BYTE startpos;
 BYTE end0pos;
 BYTE nrzeroes;
 BYTE nrmarker;
 SWORD Diff;

 fdct_and_quantization(ComponentDU,fdtbl,DU_DCT);
 //zigzag reorder
	for(i=0;i<=63;i++)
		DU[zigzag[i]]=DU_DCT[i];
	Diff=DU[0]-*DC;
	//printf("Diff %d\n",Diff);
	*DC=DU[0];
 //Encode DC
	if (Diff==0) 
		writebits(HTDC[0]); //Diff might be 0
	else {
		writebits(HTDC[category[Diff]]);
		writebits(bitcode[Diff]);
	}
 //Encode ACs
 for (end0pos=63;(end0pos>0)&&(DU[end0pos]==0);end0pos--) ;
 //end0pos = first element in reverse order !=0
	if (end0pos==0){
		writebits(EOB);
		return;
	}

 i=1;
 while (i<=end0pos)
  {
   startpos=i;
   for (; (DU[i]==0)&&(i<=end0pos);i++) ;
   nrzeroes=i-startpos;
   if (nrzeroes>=16) {
      for (nrmarker=1;nrmarker<=nrzeroes/16;nrmarker++) writebits(M16zeroes);
      nrzeroes=nrzeroes%16;
		     }
   writebits(HTAC[nrzeroes*16+category[DU[i]]]);writebits(bitcode[DU[i]]);
   i++;
  }
 if (end0pos!=63) writebits(EOB);
}

void load_data_units_from_RGB_buffer(WORD xpos,WORD ypos)
{
 BYTE x,y;
 BYTE pos=0;
 DWORD location;
 BYTE R,G,B;
 location=ypos*Ximage+xpos;
	for (y=0;y<8;y++)
	{
		for (x=0;x<8;x++)
		{
			R=RGB_buffer[location].R;
			G=RGB_buffer[location].G;
			B=RGB_buffer[location].B;
			YDU[pos]=Y(R,G,B);
			CbDU[pos]=Cb(R,G,B);
			CrDU[pos]=Cr(R,G,B);
			location++;
			pos++;
		}
		location+=Ximage-8;
	}
}

void main_encoder()
{
 SWORD DCY=0,DCCb=0,DCCr=0; //DC coefficients used for differential encoding
 WORD xpos,ypos;
 //printf("main_encoder->\n");

 for (ypos=0;ypos<Yimage;ypos+=8)
  for (xpos=0;xpos<Ximage;xpos+=8)
   {
    load_data_units_from_RGB_buffer(xpos,ypos);
    process_DU(YDU,fdtbl_Y,&DCY,YDC_HT,YAC_HT);
    process_DU(CbDU,fdtbl_Cb,&DCCb,CbDC_HT,CbAC_HT);
    process_DU(CrDU,fdtbl_Cb,&DCCr,CbDC_HT,CbAC_HT);
   }

}

//void load_bitmap(char *bitmap_name, WORD *Ximage_original, WORD *Yimage_original)
void load_bitmap(FILE *fp_bitmap, WORD *Ximage_original, WORD *Yimage_original)
{
 WORD Xdiv8,Ydiv8;
 BYTE nr_fillingbytes;//The number of the filling bytes in the BMP file
     // (the dimension in bytes of a BMP line on the disk is divisible by 4)
 colorRGB lastcolor;
 //WORD column;
 BYTE TMPBUF[64];
 WORD nrline_up,nrline_dn,nrline;
 WORD dimline;
 colorRGB *tmpline;
// FILE *fp_bitmap=fopen(bitmap_name,"rb");
 int  	colorbit=BITPERPIX_24;
 int	colorbyte;
 int	palettesize;
 PALETTERGB *paletteRGB=NULL;
 int	i,j,k;
 int	imagebegin;
 int	index;
 int	tmp;
 int	RowLength,RowEnd,ByteRead;

	memset(TMPBUF,0,sizeof(TMPBUF));
	if(fp_bitmap==NULL)
		exitmessage("Cannot open bitmap file.File not found ?");
	if(fread(TMPBUF,1,54,fp_bitmap)!=54)
	{
		printf("fread(TMPBUF,1,54,fp_bitmap)!=54\n");
		exitmessage("Need a truecolor BMP to encode.");
 	}
	//if ((TMPBUF[0]!='B')||(TMPBUF[1]!='M')||(TMPBUF[28]!=24))
	if ((TMPBUF[0]!='B')||(TMPBUF[1]!='M'))
 	{
		printf("(TMPBUF[0]!='B')||(TMPBUF[1]!='M')||(TMPBUF[28]!=24)\n");
		exitmessage("Need a truecolor BMP to encode.");
	}
	
 	Ximage=(WORD)TMPBUF[19]*256+TMPBUF[18];
 	Yimage=(WORD)TMPBUF[23]*256+TMPBUF[22];
	*Ximage_original=Ximage;
	*Yimage_original=Yimage; //Keep the old dimensions the image

 	if (Ximage%8!=0) 
 		Xdiv8=(Ximage/8)*8+8;
 	else 
 		Xdiv8=Ximage;

 	if (Yimage%8!=0) 
 		Ydiv8=(Yimage/8)*8+8;
 	else 
 		Ydiv8=Yimage;

 	//printf("Ximage %d,Yimage %d\n",Ximage,Yimage);
 	//printf("Xdiv8 %d,Ydiv8 %d\n",Xdiv8,Ydiv8);
//+Vincent10042005
	imagebegin = ((((( (TMPBUF[13] << 8) | TMPBUF[12]) << 8) | TMPBUF[11]) << 8) | TMPBUF[10]) ;
	//printf("imagebegin %x\n",imagebegin);
	if ( TMPBUF[28] != 24 )
	{
		colorbit=TMPBUF[28];
		
		if( TMPBUF[28] == BITPERPIX_8 || TMPBUF[28] == BITPERPIX_4 || TMPBUF[28] == BITPERPIX_1 )
		{
			palettesize = (imagebegin - 54)/4;
			//printf("palettesize %d\n",palettesize);
			//printf("TMPBUF[28] %d\n",(1 << (TMPBUF[28])));
			//if(palettesize != (1 << (TMPBUF[28])) )
				//exitmessage("Not standard file format for the BMP image.");
			paletteRGB = (PALETTERGB *)(malloc(palettesize*sizeof(PALETTERGB)));

			if (paletteRGB == NULL) 
 				exitmessage("paletteRGB - Not enough memory.");

			fread(paletteRGB,1,palettesize*sizeof(PALETTERGB),fp_bitmap);
//		   	for (i=0;i<palettesize;i++)
//		   		printf("%x-%x-%x ",paletteRGB[i].B,paletteRGB[i].G,paletteRGB[i].R);
//		   	printf("\n");
		}
		else if ( TMPBUF[28] == 32 )
		{
			//do nothing
		}
		else
			exitmessage("Not standard file format of the BMP image.");
	}
//Vincent10042005+
	//printf("colorbit %d\n",colorbit);
 // The image we encode shall be filled with the last line and the last column
 // from the original bitmap, until Ximage and Yimage are divisible by 8
 // Load BMP image from disk and complete X
	colorbyte = colorbit / 8;//Vincent10042005
	//printf("colorbyte %d\n",colorbyte);
	//RGB_buffer=(colorRGB *)(malloc(3*Xdiv8*Ydiv8));
	RGB_buffer=(colorRGB *)(malloc(sizeof(colorRGB)*Xdiv8*Ydiv8));//Vincent10042005
	//printf("<bmp>Xdiv8 %d,Ydiv8 %d\n",Xdiv8,Ydiv8);
	memset(RGB_buffer,0,sizeof(RGB_buffer));

	RowLength = ((Ximage * colorbit + 31) & ~31) >> 3 ;
	//printf("2:RowLength %d\n",RowLength);	

 	if (RGB_buffer==NULL) 
 		exitmessage("Not enough memory for the bitmap image.");

 	if (Ximage%4!=0) 
 		//nr_fillingbytes=4-(Ximage%4);
 		nr_fillingbytes=(Ximage%4);//Vincent10042005
 	else 
 		nr_fillingbytes=0;

	k=0;
	RowEnd = 0;
	ByteRead = 0;
 	//printf("imagebegin %x\n",imagebegin);
	fseek(fp_bitmap, imagebegin, SEEK_SET);
 	if( (colorbit == BITPERPIX_32) || (colorbit == BITPERPIX_24))
 	{
 		for (nrline=0;nrline<Yimage;nrline++)
 		{
 			for (i=nrline*Ximage;i<(nrline*Ximage)+Ximage;i++)
 			{
 				//fread(RGB_buffer+nrline*Xdiv8,1,Ximage*3,fp_bitmap);
 				//fread(RGB_buffer+nrline*Xdiv8,1,colorbyte,fp_bitmap);//Vincent10042005
 				fread(RGB_buffer+i,1,3,fp_bitmap);
 				if(colorbit == BITPERPIX_32)
 				{
 					fread(TMPBUF,1,1,fp_bitmap);
 				}
 			}

//		   	for (i=nrline*Ximage;i<(nrline*Ximage)+Ximage;i++)
//		   		printf("%x-%x-%x ",RGB_buffer[i].B,RGB_buffer[i].G,RGB_buffer[i].R);
//		   	printf("\n");

		   	if(colorbit == BITPERPIX_24)
		   		fread(TMPBUF,1,nr_fillingbytes,fp_bitmap);
	   		//memcpy(&lastcolor,RGB_buffer+nrline*Xdiv8+Ximage-1,3);
	   		memcpy(&lastcolor,RGB_buffer+nrline*Xdiv8+Ximage-1,colorbyte);//Vincent10042005
	   		//for (column=Ximage;column<Xdiv8;column++)
			//{
				//memcpy(RGB_buffer+nrline*Xdiv8+column,&lastcolor,colorbyte);
			//}
	 	}
	}
	else if(colorbit == BITPERPIX_8 )
	{
 		//printf("BITPERPIX %d\n",colorbit);
		if (Ximage%4!=0) 
			nr_fillingbytes=4-(Ximage%4);
		else 
			nr_fillingbytes=0;
 			
 		for (nrline=0;nrline<Yimage;nrline++)
 		{
			//printf("nrline %d\n",nrline);
 			for (i=(nrline*Ximage);i<((nrline*Ximage)+Ximage);i++)
 			{
 				//printf("i %d ",i);
 				fread(TMPBUF,1,1,fp_bitmap);
 				//printf("1TMPBUF[0] %x %x ",TMPBUF[0],TMPBUF);
 				for(j=0;j< (8/colorbit);j++)
 				{
 					//printf("2TMPBUF[0] %x %x ",TMPBUF[0],TMPBUF);
					index = TMPBUF[0] >> (8-colorbit);
 					//printf("index %x ",index);
 					//printf("%d,%d,%x-%x-%x",i,j,paletteRGB[index].B,paletteRGB[index].G,paletteRGB[index].R);
 					RGB_buffer[i].B = paletteRGB[index].B;
 					RGB_buffer[i].G = paletteRGB[index].G;
 					RGB_buffer[i].R = paletteRGB[index].R;
 					//printf(" %x-%x-%x \n",RGB_buffer[i].B,RGB_buffer[i].G,RGB_buffer[i].R);
 					TMPBUF[0] = ( TMPBUF[0] << (colorbit) ) >> colorbit;
 					i += j;
// 					printf("%d \n",i);
 				}
 			}

		   	fread(TMPBUF,1,nr_fillingbytes,fp_bitmap);
		   	//fread(TMPBUF,1,1,fp_bitmap);
		   	//printf("nrline %d 3TMPBUF[0] %x %x\n",nrline,TMPBUF[0],TMPBUF);
	   		//memcpy(&lastcolor,RGB_buffer+nrline*Xdiv8+Ximage-1,colorbyte);//Vincent10042005
	 	}
		free(paletteRGB);		
	}
	else if(colorbit == BITPERPIX_4)
	{
		if (Ximage%4!=0) 
			nr_fillingbytes=4-(Ximage%4);
		else 
			nr_fillingbytes=0;

		for (i=0;i<(Yimage*Ximage);)
		{
			fread(TMPBUF,1,1,fp_bitmap);
			for(j=0;j< (8/colorbit);j++)
			{
				index = TMPBUF[0] >> ((colorbit) - (colorbit*j));
 				RGB_buffer[i].B = paletteRGB[index].B;
 				RGB_buffer[i].G = paletteRGB[index].G;
 				RGB_buffer[i].R = paletteRGB[index].R;
 				TMPBUF[0] = TMPBUF[0] & 0x0f;
 				i ++;
 				if((i%(Ximage)) == 0)
 					break;
 			}
 			
 			if((i%(Ximage)) == 0)
 			{
 				tmp = nr_fillingbytes/2;
 				fread(TMPBUF,1,tmp,fp_bitmap);
 			}
 		}
		free(paletteRGB);		
	}// End of colorbit == BITPERPIX_4
	else if(colorbit == BITPERPIX_1)
	{
		//printf("BITPERPIX 1\n");
		if (Ximage%4!=0) 
			nr_fillingbytes=4-(Ximage%4);
		else 
			nr_fillingbytes=0;
		//printf("nr_fillingbytes %d\n",nr_fillingbytes);

		//for (i=0;i<(Yimage*Ximage);)
		for (i=0;i<(Yimage*RowLength);i++)
		{
			fread(TMPBUF,1,1,fp_bitmap);
			ByteRead ++;
			//printf("1TMPBUF[0] %x\n",TMPBUF[0]);
			if(RowEnd == 0)
			{
				for(j=0;j< (8/colorbit);j++)
				{
					//printf("2TMPBUF[0] %x ",TMPBUF[0]);
					//index = TMPBUF[0] >> (8 - colorbit - j);
					index = TMPBUF[0] >> (8 - colorbit );
					//printf("index %x ",index);
					//printf("%d,%d,%d,%x-%x-%x",i,j,k,paletteRGB[index].B,paletteRGB[index].G,paletteRGB[index].R);
	 				RGB_buffer[k].B = paletteRGB[index].B;
	 				RGB_buffer[k].G = paletteRGB[index].G;
	 				RGB_buffer[k].R = paletteRGB[index].R;
	 				//printf(" %x-%x-%x ",RGB_buffer[k].B,RGB_buffer[k].G,RGB_buffer[k].R);
	 				TMPBUF[0] = ( TMPBUF[0] << (colorbit) ) ;
	 				//printf("3TMPBUF[0] %x \n",TMPBUF[0]);
 					k ++;
 					if((k%(Ximage)) == 0)
 					{
 						RowEnd = 1;
 						break;
 					}
 				}
 			}
 			
 			//if((k%(Ximage)) == 0)
 			if(RowEnd == 1)
 			{
 				//printf("ByteRead %d\n",ByteRead);
 				fread(TMPBUF,1,RowLength-ByteRead,fp_bitmap);
 				ByteRead = 0;
 				RowEnd = 0;
 				if((k%(Ximage*Yimage)) == 0)
 					break;
 			}
 		}
		free(paletteRGB);		
	}// End of colorbit == BITPERPIX_1


 	//Ximage=Xdiv8;
 	dimline=Ximage*3;
// 	printf("Xdiv8 %d,Ximage %d, Yimage %d, dimline %d\n",Xdiv8 ,Ximage ,Yimage ,dimline);
 	//dimline=Ximage*colorbyte;//Vincent10042005
 	//tmpline=(colorRGB *)malloc(dimline);
 	tmpline=(colorRGB *)malloc(sizeof(colorRGB)*Ximage);

 if (tmpline==NULL) 
 	exitmessage("Not enough memory.");
 //Reorder in memory the inversed bitmap
 for (nrline_up=Yimage-1,nrline_dn=0;nrline_up>nrline_dn;nrline_up--,nrline_dn++)
 {
 	//memcpy(tmpline,RGB_buffer+nrline_up*Ximage, dimline);
 	for(i=0;i<Ximage;i++)
 	{
 		tmpline[i].B = RGB_buffer[nrline_up*Ximage+i].B;
 		tmpline[i].G = RGB_buffer[nrline_up*Ximage+i].G;
 		tmpline[i].R = RGB_buffer[nrline_up*Ximage+i].R;
 	}

   	//memcpy(RGB_buffer+nrline_up*Ximage,RGB_buffer+nrline_dn*Ximage,dimline);
 	for(i=0;i<Ximage;i++)
 	{
 		RGB_buffer[nrline_up*Ximage+i].B = RGB_buffer[nrline_dn*Ximage+i].B;
 		RGB_buffer[nrline_up*Ximage+i].G = RGB_buffer[nrline_dn*Ximage+i].G;
 		RGB_buffer[nrline_up*Ximage+i].R = RGB_buffer[nrline_dn*Ximage+i].R;
 	}
 	
   	//memcpy(RGB_buffer+nrline_dn*Ximage,tmpline,dimline);
 	for(i=0;i<Ximage;i++)
 	{
 		RGB_buffer[nrline_dn*Ximage+i].B = tmpline[i].B;
 		RGB_buffer[nrline_dn*Ximage+i].G = tmpline[i].G;
 		RGB_buffer[nrline_dn*Ximage+i].R = tmpline[i].R;
 	}
 }
 
 // Y completion:
 memcpy(tmpline,RGB_buffer+(Yimage-1)*Ximage,dimline);
 for (nrline=Yimage;nrline<Ydiv8;nrline++)
 {
 	memcpy(RGB_buffer+nrline*Ximage,tmpline,dimline);
 }

 Yimage=Ydiv8;
 //free(TMPBUF);
 free(tmpline);
 fclose(fp_bitmap);
}

void init_all()
{
 set_DQTinfo();
 set_DHTinfo();
 init_Huffman_tables();
 set_numbers_category_and_bitcode();
 precalculate_YCbCr_tables();
 prepare_quant_tables();
}


int
//read_png(char *file_name)
read_png(FILE *fp)
{
	png_structp png_ptr; //struct png_struct_def
	png_infop info_ptr;
	png_colorp ori_pal;
	int ori_bpp;
	unsigned char header[PNG_READ_SIG_BYTES];
	int is_png=0;

	//FILE *fp;
	int i,j;
	png_color_16 *image_background;
//	png_timep in_time;  /*  a struct POINTER  */
//    extern png_time savechunk_time; /* NOT a pointer! S.A. */
	int	skip1,skip2=0;

//	if(file_name != NULL)  
//	{
//		  if( (fp = fopen(file_name, "rb")) == NULL )
//		  {
//			fprintf(stderr, "Can't open file %s\n",file_name);
//			return 0;
//		  }
//	}
//	else
//	{
//		fp = stdin;
//	}

	fread(header, 1, (int)PNG_READ_SIG_BYTES, fp);
	is_png = !png_sig_cmp(header, 0, PNG_READ_SIG_BYTES);
	 if(!is_png) 
	 {
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

	if(png_ptr == NULL)
	{
		fprintf(stderr, "Error creating read_struct\n");
		//if(file_name)
			fclose(fp);
		return 0;
	}

	png_set_sig_bytes(png_ptr, PNG_READ_SIG_BYTES);

	info_ptr = png_create_info_struct(png_ptr);
	png_debug(0, "Creating png info struct");
	if (info_ptr == NULL)
	{
		//if(file_name)
			fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return 0;
	}

	png_debug(0, "Setting default png read function");
	png_set_read_fn(png_ptr, fp, NULL);

	if (setjmp(png_ptr->jmpbuf))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		//if(file_name)
			fclose(fp);
		fprintf(stderr, "Error reading file on descriptor '%i'\n", fileno(fp));
		return 0;
	}

	png_debug(0, "Starting png_init_io");
	png_init_io(png_ptr, fp);

	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
		&interlace_type, NULL, NULL);

//	if(bit_depth>8)
//	{
//		fprintf(stderr, "This image can't be converted because it has 16 bits/sample.\n");
//		goto abort1;
//	}


    if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
		ori_pal_size=0;
		ori_bpp = bit_depth;

		/* we only do this to get the palette size so we can print it */
		png_get_PLTE(png_ptr,info_ptr,&ori_pal,&ori_pal_size);

		//fprintf(stderr, "original palette size:   %3d, %2d bpp\n",ori_pal_size,ori_bpp);

        png_set_expand(png_ptr);
	}
	else
	{
		/* figure out bits per pixel so we can print it */
		ori_bpp= bit_depth*png_get_channels(png_ptr,info_ptr);
		//fprintf(stderr, "original palette size: [n/a], %2d bpp\n",ori_bpp);
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

	if (png_get_bKGD(png_ptr, info_ptr, &image_background))
	{
		chnks.has_bkgd=1;
		bkgd.red=   (unsigned char)image_background->red;
		bkgd.green= (unsigned char)image_background->green;
		bkgd.blue=  (unsigned char)image_background->blue;
		bkgd.alpha=255;
	}

	if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
	{
		chnks.has_gama=1;
	}

	if (png_get_sRGB(png_ptr, info_ptr, &srgb_intent))
	{
		chnks.has_srgb=1;
	}

	if(png_get_valid(png_ptr,info_ptr,PNG_INFO_pHYs))
	{
		chnks.has_phys=1;
		png_get_pHYs(png_ptr,info_ptr,&res_x,&res_y,&res_unit_type);
		if(res_x<1 || res_y<1) chnks.has_phys=0;
	}


/*  S.A.  ..............................  */
//    if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tIME))
//    {
//	   if(png_get_tIME(png_ptr, info_ptr, &in_time) == PNG_INFO_tIME)
//	   {
//		   sprintf(time_as_string, "%s",
//	          png_convert_to_rfc1123(png_ptr, in_time));
//	       savechunk_time = *in_time;
//       }
//    }
//    else
//    {
//    	chnks.need_tIME_after_IDATs=1;
//    }


    png_read_update_info(png_ptr, info_ptr);

	rowbytes=png_get_rowbytes(png_ptr, info_ptr);
	channels=(int)png_get_channels(png_ptr, info_ptr);

	if(channels<3 || channels>4) {
		fprintf(stderr, "internal error: channels=%d\n",channels);
		goto abort1;
	}

//	fprintf(stderr,"Image size is %ldx%ld  memory required=%.3fMB\n",
//		width, height,
//		(height*rowbytes + height*sizeof(unsigned char*) + width*height) / (1024. * 1024.) );

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

	free(row_pointers);

	//printf("2rowbytes %d\n",rowbytes);
	//printf("info_ptr->width %d\n",info_ptr->width);
	//printf("info_ptr->height %d\n",info_ptr->height);

	RGB_buffer=(colorRGB *)(malloc(4*info_ptr->width*info_ptr->height));
	memset(RGB_buffer,0,sizeof(RGB_buffer));
 	if (!RGB_buffer)
 	{
		printf("Not enough memory for the png image.\n");
		goto abort1;
 	}

	skip1 = rowbytes / info_ptr->width;
	//printf("skip1 %d\n",skip1);
	if((skip1 != 3 && skip1 != 4 && skip1 != 6 && skip1 != 8))
	{
		printf("PNG: Wrong pixel per sample\n");
		goto abort1;
	}

	switch(skip1)
	{
		case 3:
			skip1 = 0;
			skip2 = 0;
			break;
		case 4:
			skip1 = 0;
			skip2 = 1;
			break;
		case 6:
			skip1 = 1;
			skip2 = 0;
			break;
		case 8:
			skip1 = 1;
			skip2 = 2;
			break;
	}
	j=0;

	for(i=0;i<height*rowbytes;i++)
	{
		RGB_buffer[j].R = image1[i];
		i+=skip1;
		RGB_buffer[j].G = image1[i+=1];
		i+=skip1;
		RGB_buffer[j].B = image1[i+=1];
		i+=skip1;
		i+=skip2;

		//printf("%d-%d-%d ",RGB_buffer[j].R,RGB_buffer[j].G,RGB_buffer[j].B);
		j++;
	}

	png_read_end(png_ptr, info_ptr);
	free(image1);
/* S.A.  .................................  */
//      if(chnks.need_tIME_after_IDATs &&
//           png_get_valid(png_ptr,info_ptr,PNG_INFO_tIME))
//      {
//		   if(png_get_tIME(png_ptr, info_ptr, &in_time) == PNG_INFO_tIME)
//		   {
//			   sprintf(time_as_string, "%s",
//				  png_convert_to_rfc1123(png_ptr, in_time));
//			   savechunk_time = *in_time;
//		   }
//	  }


	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

//	if(file_name)
//		fclose(fp);

	return 1;

abort1:
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
//	if(file_name)
		fclose(fp);
	return 0;
}

void new_resolution(colorRGB **RGB_buffer_2d,int newW,int newH)
{
	double			dX,dY,x,y;
	int				i,j;
	unsigned char	nclr1,nclr2,nclr3,nclr4;
	int				ix,iy;                    // integer value of (x,y)
	double			fx,fy;                    // float value of (x,y)
	
	dX = (double)Ximage/(double)newW;
  	dY = (double)Yimage/(double)newH;
  	//printf("dX %f, dY %f\n",dX,dY);
  	
	for(j = 0 ; j < newH ; j++)
	{
    	for(i =0 ; i < newW ; i++)
		{
     		x = dX*(double)i;
      		y = dY*(double)j;
      		ix = (int)x;         iy = (int)y;
       		fx = x - (float)ix;  fy = y - (float)iy;

       		nclr1 = RGB_buffer_2d[iy][ix].R;
			nclr2 = RGB_buffer_2d[iy+1][ix].R;
			nclr3 = RGB_buffer_2d[iy][ix+1].R;
			nclr4 = RGB_buffer_2d[iy+1][ix+1].R;
			RGB_buffer[j*newW+i].R= (BYTE)(	(1.0-fx)*(1.0-fy)*(double)nclr1 +
											(fx)*(1.0-fy)*(double)nclr2 +
											(1.0-fx)*(fy)*(double)nclr3 +
											(fx)*(fy)*(double)nclr4);
			
			nclr1 = RGB_buffer_2d[iy][ix].G;
			nclr2 = RGB_buffer_2d[iy+1][ix].G;
			nclr3 = RGB_buffer_2d[iy][ix+1].G;
			nclr4 = RGB_buffer_2d[iy+1][ix+1].G;
			RGB_buffer[j*newW+i].G= (BYTE)(	(1.0-fx)*(1.0-fy)*(double)nclr1 +
											(fx)*(1.0-fy)*(double)nclr2 +
											(1.0-fx)*(fy)*(double)nclr3 +
											(fx)*(fy)*(double)nclr4);

			nclr1 = RGB_buffer_2d[iy][ix].B;
			nclr2 = RGB_buffer_2d[iy+1][ix].B;
			nclr3 = RGB_buffer_2d[iy][ix+1].B;
			nclr4 = RGB_buffer_2d[iy+1][ix+1].B;
			RGB_buffer[j*newW+i].B= (BYTE)(	(1.0-fx)*(1.0-fy)*(double)nclr1 +
											(fx)*(1.0-fy)*(double)nclr2 +
											(1.0-fx)*(fy)*(double)nclr3 +
											(fx)*(fy)*(double)nclr4);
		}
	}
  	//printf("new_resolution(): <--\n");
}

//int read_tif(char *in_file){
//  TIFF *image;
//  uint16 photo, bps, spp, fillorder;
////  uint32 width;
//  tsize_t stripSize;
//  unsigned long imageOffset, result;
//  int stripMax, stripCount;
//  char *buffer, tempbyte;
//  unsigned long bufferSize, count;
////  uint32 height; //Vincent03072006
//  int	j;
//
//  // Open the TIFF image
//  if((image = TIFFOpen(in_file, "r")) == NULL){
//    printf("11\n");
//    fprintf(stderr, "Could not open incoming image\n");
//    //exit(42);
//    return 0;
//  }
//
//  // Check that it is of a type that we support
//  if((TIFFGetField(image, TIFFTAG_BITSPERSAMPLE, &bps) == 0) || (bps != 1)){
//    printf("22\n");
//    fprintf(stderr, "Either undefined or unsupported number of bits per sample\n");
//    //exit(42);
////    return 0;
//  }
//
//  if((TIFFGetField(image, TIFFTAG_SAMPLESPERPIXEL, &spp) == 0) || (spp != 1)){
//    printf("33\n");
//    fprintf(stderr, "Either undefined or unsupported number of samples per pixel\n");
//    //exit(42);
////    return 0;
//  }
//
//  // Read in the possibly multiple strips
//  stripSize = TIFFStripSize (image);
//  stripMax = TIFFNumberOfStrips (image);
//  imageOffset = 0;
//  
//  bufferSize = TIFFNumberOfStrips (image) * stripSize;
//  if((buffer = (char *) malloc(bufferSize)) == NULL){
//    printf("44\n");
//    fprintf(stderr, "Could not allocate enough memory for the uncompressed image\n");
//    //exit(42);
//    return 0;
//  }
//  
//  for (stripCount = 0; stripCount < stripMax; stripCount++){
//    if((result = TIFFReadEncodedStrip (image, stripCount,
//				      buffer + imageOffset,
//				      stripSize)) == -1){
//      printf("55\n");
//      fprintf(stderr, "Read error on input strip number %d\n", stripCount);
//      //exit(42);
//      return 0;
//    }
//
//    imageOffset += result;
//  }
//
//  // Deal with photometric interpretations
//  if(TIFFGetField(image, TIFFTAG_PHOTOMETRIC, &photo) == 0){
//    printf("66\n");
//    fprintf(stderr, "Image has an undefined photometric interpretation\n");
//    //exit(42);
//    return 0;
//  }
//  
////  if(photo != PHOTOMETRIC_MINISWHITE){
////  //if(photo == PHOTOMETRIC_MINISWHITE){
////    // Flip bits
////    printf("Fixing the photometric interpretation\n");
////
////    for(count = 0; count < bufferSize; count++)
////      buffer[count] = ~buffer[count];
////  }
//
//  // Deal with fillorder
//  if(TIFFGetField(image, TIFFTAG_FILLORDER, &fillorder) == 0){
//    printf("77\n");
//    fprintf(stderr, "Image has an undefined fillorder\n");
//    //exit(42);
////    return 0;
//  }
//  
//  if(fillorder != FILLORDER_MSB2LSB){
//    // We need to swap bits -- ABCDEFGH becomes HGFEDCBA
//    printf("Fixing the fillorder\n");
//
//    for(count = 0; count < bufferSize; count++){
//      tempbyte = 0;
//      if(buffer[count] & 128) tempbyte += 1;
//      if(buffer[count] & 64) tempbyte += 2;
//      if(buffer[count] & 32) tempbyte += 4;
//      if(buffer[count] & 16) tempbyte += 8;
//      if(buffer[count] & 8) tempbyte += 16;
//      if(buffer[count] & 4) tempbyte += 32;
//      if(buffer[count] & 2) tempbyte += 64;
//      if(buffer[count] & 1) tempbyte += 128;
//      buffer[count] = tempbyte;
//    }
//  }
//     
//  // Do whatever it is we do with the buffer -- we dump it in hex
//  if(TIFFGetField(image, TIFFTAG_IMAGEWIDTH, &width) == 0){
//    printf("88\n");
//    fprintf(stderr, "Image does not define its width\n");
//    //exit(42);
//    return 0;
//  }
//  
//  if(TIFFGetField(image, TIFFTAG_IMAGELENGTH, &height) == 0){
//    printf("99\n");
//    fprintf(stderr, "Image does not define its height\n");
//    //exit(42);
//    return 0;
//  }
//  
//  printf("width %d, height %d, bufferSize %d\n",width, height, bufferSize);
//  Ximage = width;
//  Yimage = height;
//  RGB_buffer=(colorRGB *)(malloc(sizeof(colorRGB)*width*height));
//  memset(RGB_buffer,0,sizeof(RGB_buffer));
//  
//  j=0;
//  for(count = 0; count < bufferSize; count++){
//    	printf("%03d-%03d-%03d ", (unsigned char) buffer[count], (unsigned char) buffer[count+1], (unsigned char) buffer[count+2]);
//    	if((count != 0) && ((count+1) % (width*3)) == 0)
//    		printf("\n");
//    	else 
//	    	printf(" ");
//
//		RGB_buffer[j].R = buffer[count];
//		RGB_buffer[j].G = buffer[count+=1];
//		RGB_buffer[j].B = buffer[count+=1];	
//		j++;
//  }
//
//  free(buffer);
//  TIFFClose(image);
//
//  return 1;
//}

/*-----------------------------------------------------------------
* ROUTINE NAME - calculate_value
*------------------------------------------------------------------
* FUNCTION: calculate IFD Entry value
*
* INPUT:
* OUTPUT:
* RETURN:
*      0 1 2 3 4 5 6 7 8 9 10 11                 
* NOTE:1 0 0 3 0 0 0 1 1 0 0  0
*----------------------------------------------------------------*/
IFDENTRY calculate_value(unsigned char * IFDEnt,int	bigendian) {
	int		fieldtype[13]={0,1,1,SHORT,LONG,8,1,1,2,4,8,4,8};
	int		i,j;
	IFDENTRY IfdEntry;
	
	memset(&IfdEntry,0,sizeof(IfdEntry));
	if(bigendian)
	{
		IfdEntry.valuebyte = fieldtype[IFDEnt[3]+IFDEnt[2]*0x100];
		IfdEntry.numofvalue = 	IFDEnt[7] + 
								IFDEnt[6]*0x100 +
								IFDEnt[5]*0x10000 +
								IFDEnt[4]*0x1000000 ;
		
		if(IfdEntry.numofvalue==1)
		{
			i=0;
			for(j=7+IfdEntry.valuebyte; j>7 ; j--)
			{
				IfdEntry.value_offset += IFDEnt[j]*(1<<(i*8));
				i++;
			}
		}
		else if(IfdEntry.numofvalue ==3)
		{
			i=0;
			for(j=11; j>7 ; j--)
			{
				IfdEntry.value_offset += IFDEnt[j]*(1<<(i*8));
				i++;
			}
			//printf("+++IfdEntry.value_offset %x\n",IfdEntry.value_offset);
		}
	}
	else //little-endian
	{
		IfdEntry.valuebyte = fieldtype[IFDEnt[2]+IFDEnt[3]*0x100];
		IfdEntry.numofvalue = 	IFDEnt[4] + 
								IFDEnt[5]*0x100 +
								IFDEnt[6]*0x10000 +
								IFDEnt[7]*0x1000000 ;
		
		IfdEntry.value_offset = IFDEnt[8] + 
								IFDEnt[9]*0x100 +
								IFDEnt[10]*0x10000 +
								IFDEnt[11]*0x1000000 ;
	}
	return IfdEntry;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - get_tif_size
*------------------------------------------------------------------
* FUNCTION: get tif image file high and width resolution
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
void get_tif_size(PIMGSIZE ImgSize) {
	unsigned char		*check_img_type;
	unsigned char		header_array[8];
	long		ifd_skip_offset;
	unsigned char		dir_num_array[2];
	int			dir_num;
	int 		i,j,k;
	int			count=0;
	IFDENTRY	IfdEntry1;
	IFDENTRY	IfdEntry2;
	int			RGBimage=0;
	int			bigendian=0;

	memset(&IfdEntry1,0,sizeof(IfdEntry1));
	memset(&IfdEntry2,0,sizeof(IfdEntry2));	

	rewind(ImgSize->fp);
	fread(&header_array, 1, 8, ImgSize->fp);
	
	if(header_array[0]=='M' && header_array[1] =='M')
		bigendian=1;
	//printf("bigendian %d\n",bigendian);
	//fseek(ImgSize->fp,2,SEEK_CUR);

	//fread(&ifd_skip_array, BYTE, 4, ImgSize->fp);
	if(bigendian)
	{
		ifd_skip_offset = 	header_array[4]*0x1000000 + 
							header_array[5]*0x10000 + 
							header_array[6]*0x100 + 
							header_array[7];
	}	
	else
	{
		ifd_skip_offset = 	header_array[7]*0x1000000 + 
							header_array[6]*0x10000 + 
							header_array[5]*0x100 + 
							header_array[4];
	}
	fseek(ImgSize->fp,ifd_skip_offset,SEEK_SET);
	//printf("header_array %s\n",header_array);
	//printf("ifd_skip_offset %d\n",ifd_skip_offset);	
	fread(&dir_num_array, 1, 2, ImgSize->fp);
	if(bigendian)
		dir_num =	dir_num_array[0]*0x100 + dir_num_array[1];
	else
		dir_num =	dir_num_array[1]*0x100 + dir_num_array[0];
	
	//printf("dir_num %d\n",dir_num);


	check_img_type = (unsigned char *) malloc (12*sizeof(unsigned char));
	for(i=0;i < dir_num;i++)
	{
		fread(check_img_type, 1, 12, ImgSize->fp);
		if(bigendian)
		{
			if(check_img_type[1] == 0x00 && check_img_type[0] == 0x01) //width
			{
					IfdEntry1 = calculate_value(check_img_type,bigendian);
					ImgSize->width = IfdEntry1.value_offset;
					count++;
			}
			else if(check_img_type[1] == 0x01 && check_img_type[0] == 0x01)//high
			{
					IfdEntry1 = calculate_value(check_img_type,bigendian);
					ImgSize->high = IfdEntry1.value_offset;
					count++;
			}
			else if(check_img_type[1] == 0x02 && check_img_type[0] == 0x01) //depth
			{
				if( check_img_type[7] == 0x01 )
				{
					IfdEntry1 = calculate_value(check_img_type,bigendian);
					ImgSize->depth = IfdEntry1.value_offset;
					//printf("big-endian ImgSize->depth %d\n",ImgSize->depth);
					count++;
				}
				else if(check_img_type[7] == 0x03)
				{
					IfdEntry2 = calculate_value(check_img_type,bigendian);
					//printf("IfdEntry2.value_offset %x\n",IfdEntry2.value_offset);
					RGBimage =1;
					count++;
				}
			}
		}
		else//little-endian
		{
			if(check_img_type[0] == 0x00 && check_img_type[1] == 0x01) //width
			{
					IfdEntry1 = calculate_value(check_img_type,bigendian);
					ImgSize->width = IfdEntry1.value_offset;
					count++;
			}
			else if(check_img_type[0] == 0x01 && check_img_type[1] == 0x01)//high
			{
					IfdEntry1 = calculate_value(check_img_type,bigendian);
					ImgSize->high = IfdEntry1.value_offset;
					count++;
			}
			else if(check_img_type[0] == 0x02 && check_img_type[1] == 0x01) //depth
			{
				if( check_img_type[4] == 0x01 )
				{
					IfdEntry1 = calculate_value(check_img_type,bigendian);
					ImgSize->depth = IfdEntry1.value_offset;
					//printf("little-endian ImgSize->depth %d\n",ImgSize->depth);
					count++;
				}
				else if(check_img_type[4] == 0x03)
				{
					IfdEntry2 = calculate_value(check_img_type,bigendian);
					//printf("IfdEntry2.value_offset %x\n",IfdEntry2.value_offset);
					RGBimage =1;
					count++;
				}
			}
		}//little-endian
		if(count == 3)
			break;
	}//i

	if(RGBimage)
	{
		//printf("---IfdEntry2.value_offset %x\n",IfdEntry2.value_offset);
		fseek(ImgSize->fp,IfdEntry2.value_offset,SEEK_SET);
		if(bigendian)
		{
			for(i=0;i<IfdEntry2.numofvalue;i++)
			{
				//printf("i %d\n",i);
				fread(check_img_type, 1, IfdEntry2.valuebyte, ImgSize->fp);
				k=0;
				for(j=IfdEntry2.valuebyte-1;j > 0;j--)
				{
					ImgSize->depth += check_img_type[j]*(1<<(k*8));
					k++;
				}
				//printf("IfdEntry2.valuebyte %d,IfdEntry2.numofvalue %d\n",IfdEntry2.valuebyte,IfdEntry2.numofvalue);
			}
		}
		else
		{
			//printf("RGB-little\n");
			for(i=0;i<IfdEntry2.numofvalue;i++)
			{
				fread(check_img_type, 1, IfdEntry2.valuebyte, ImgSize->fp);	

				for(j=0;j<IfdEntry2.valuebyte;j++)
				{
					ImgSize->depth += check_img_type[j]*(1<<(j*8));
				}
				//printf("IfdEntry2.valuebyte %d,IfdEntry2.numofvalue %d\n",IfdEntry2.valuebyte,IfdEntry2.numofvalue);
			}
		}
	}
	//printf("--->ImgSize.depth %d,ImgSize.width %d,ImgSize.high %d\n",ImgSize->depth,ImgSize->width,ImgSize->high);
	free(check_img_type);
}

/*-----------------------------------------------------------------
* ROUTINE NAME - main         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:argv[1]: b:bmp  g:gif  p:png 
*		argv[2]: input file name
*		argv[3]: output file name
*		argv[4]: resize flag, 0:none 1:resize
*		argv[5]: new x(width)
*		argv[6]: new y(height)
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int main(int argc, char **argv)
{
	FILE 		*in;
	int			i,j;
	WORD 		Xdiv8,Ydiv8;
	bitstring 	fillbits;
	char		ch;
	//int			result;
	WORD 		Ximage_original,Yimage_original;
	gdImagePtr 	im=NULL;// for gif
	int			MAXnewW,MAXnewH;
	int			newW=0,newH=0;
	double		scale;
	int			resize=0;
	colorRGB 	**tmp_RGB;
	TIFF		*tif;
	//int			status;
	IMGSIZE 	ImgSize;

	ch = argv[1][0];
	
	resize = atoi(argv[4]);
	MAXnewW = atoi(argv[5]);
	MAXnewH = atoi(argv[6]);

	in = fopen(argv[2], "rb");
	if (!in) {
		fprintf(stderr, "Input file does not exist!\n");
		exit(1);
	}

	switch(ch)
	{
		case 'b':
			load_bitmap(in, &Ximage_original, &Yimage_original);
//			printf("\nbmp:Ximage_original %d,Yimage_original %d\n",Ximage_original,Yimage_original);
//			for(i=0;i<Yimage_original*Ximage_original;i++)
//			{
//				printf("%d-%x-%x-%x ",i,RGB_buffer[i].B,RGB_buffer[i].G,RGB_buffer[i].R);
//				if(i % (Ximage_original-1) == 0 && i!=0)
//					printf("\n");
//			}
//			printf("\n");
			break;
		case 'g':
			im = gdImageCreateFromGif(in);
			if (!im) {
				fprintf(stderr, "Image is not GIF format!\n");
				exit(1);
			}
			fclose(in);
			Ximage_original = im->sx;
			Yimage_original = im->sy;
			Ximage	=im->sx;
			Yimage	=im->sy;
			//printf("gif:Ximage_original %d,Yimage_original %d\n",Ximage_original,Yimage_original);
			break;
		case 'p':
			//printf("<== png file ==>\n");
			read_png(in);
			Ximage_original = width;
			Yimage_original = height;
			Ximage	= width;
			Yimage	= height;
			//printf("width %d\n",width);
			//printf("height %d\n",height);
			
			fclose(in);
			//if (0 != err)
				//error_exit(err);
			break;
		case 't':
			fclose(in);
			
			//if(resize)
			{
    			memset(&ImgSize,0,sizeof(ImgSize));
    			if((ImgSize.fp = fopen(argv[2], "r")) == NULL)
    			{
    			    printf("main(): open tif file error\n");
    			    return 0;
    			}

    			get_tif_size(&ImgSize);
    			fclose(ImgSize.fp);
//    			printf("ImgSize.depth %d,ImgSize.width %d,ImgSize.high %d\n",ImgSize.depth,ImgSize.width,ImgSize.high);
    			bitpersample = ImgSize.depth;

				tif = read_tif(argv[2]);

				width = tif->tif_dir.td_imagewidth;
				height = tif->tif_dir.td_imagelength;
				
				Ximage_original = width;
				Yimage_original = height;
				Ximage	= Ximage_original;
				Yimage	= Yimage_original;
            	
//				printf("2tif:Ximage_original %d,Yimage_original %d\n",Ximage_original,Yimage_original);
			}
			//else
				//exit(1);
			break;
	}

//	for(i=0;i<Yimage_original*Ximage_original;i++)
//	{
//		printf("%d-%x-%x-%x ",i,RGB_buffer[i].B,RGB_buffer[i].G,RGB_buffer[i].R);
//		if(i % (Ximage_original-1) == 0 && i!=0)
//			printf("\n");
//	}
//	printf("\n");

	//printf("argv[3] %s\n",argv[3]);
	fp_jpeg_stream = fopen(argv[3], "wb");
	if (!fp_jpeg_stream) {
		fprintf(stderr, "Output file cannot be written to!\n");
		if (ch == 'g') 
			gdImageDestroy(im);
		exit(1);	
	}

 	init_all();

	// compute new image resolution 
	if(resize && (Ximage_original > MAXnewW || Yimage_original>MAXnewH))
	{
		scale = (double)MAXnewW / (double)Ximage_original;
		//printf("1scale %f\n",scale);
		newH = scale * Yimage_original;
		if( newH > MAXnewH)
		{
			scale = (double)MAXnewH / (double)Yimage_original;
			newW = scale * Ximage_original;
			newH = scale * Yimage_original;
			//printf("2scale %f\n",scale);
		}
		else
			newW = scale * Ximage_original;
	}
	else
	{
		resize = 0;
	}
//	printf("2newH %d,newW %d,oriH %d,oriW %d,MaxnewH %d, MAXnewW %d\n",
//			newH,newW,Ximage_original,Yimage_original,MAXnewH,MAXnewW);

	switch(ch)
	{
		case 'b':
			if(resize)
			{
				#ifdef DBG
				printf("<imgconvert>bmp newH %d,newW %d, Ximage %d,Yimage %d\n",
						newH,
						newW,
						Ximage,
						Yimage
						);
				#endif
				tmp_RGB = (colorRGB **) malloc (sizeof(colorRGB)*Yimage*3);
   				for(i=0;i<Yimage;i++)
   					tmp_RGB[i]= (colorRGB *) malloc (sizeof(colorRGB)*Ximage*3);
				
				for(i=0;i<Yimage;i++)
				{
					for(j=0;j<Ximage;j++)
					{
						tmp_RGB[i][j].R = RGB_buffer[(i*Ximage)+j].R;
						tmp_RGB[i][j].G = RGB_buffer[(i*Ximage)+j].G;
						tmp_RGB[i][j].B = RGB_buffer[(i*Ximage)+j].B;
					}
				}
				memset(RGB_buffer,0,sizeof(RGB_buffer));

				new_resolution(tmp_RGB, newW, newH);
				free(tmp_RGB);

				SOF0info.width=newW;
				SOF0info.height=newH;
				Ximage=newW;
				Yimage=newH;
 				if (Ximage%8!=0) 
 					Xdiv8=(Ximage/8)*8+8;
 				else 
 					Xdiv8=Ximage;

 				if (Yimage%8!=0) 
 					Ydiv8=(Yimage/8)*8+8;
 				else 
 					Ydiv8=Yimage;
			}
			else
			{
				SOF0info.width=Ximage_original;
				SOF0info.height=Yimage_original;	
			}
			break;
		case 'g':
			if(resize)
			{
				#ifdef DBG
				printf("<imgconvert>gif newH %d,newW %d, Ximage %d,Yimage %d\n",
						newH,
						newW,
						Ximage,
						Yimage
						);
				#endif
				tmp_RGB = (colorRGB **) malloc (sizeof(colorRGB)*Yimage*3);
   				for(i=0;i<Yimage;i++)
   					tmp_RGB[i]= (colorRGB *) malloc (sizeof(colorRGB)*Ximage*3);
				
				for(i=0;i<Yimage;i++)
				{
					for(j=0;j<Ximage;j++)
					{
						int idx = im->pixels[i][j];
						tmp_RGB[i][j].R = im->red[idx];
						tmp_RGB[i][j].G = im->green[idx];
						tmp_RGB[i][j].B = im->blue[idx];
					}
				}


				//Ximage=newW;
				//Yimage=newH;
 				if (newW%8!=0) 
 					Xdiv8=(newW/8)*8+8;
 				else 
 					Xdiv8=newW;

 				if (newH%8!=0) 
 					Ydiv8=(newH/8)*8+8;
 				else 
 					Ydiv8=newH;

				RGB_buffer=(colorRGB *)(malloc(4*Xdiv8*Ydiv8));
				memset(RGB_buffer,0,sizeof(RGB_buffer));

				new_resolution(tmp_RGB, newW, newH);
				free(tmp_RGB);

				SOF0info.width=newW;
				SOF0info.height=newH;
				Ximage=newW;
				Yimage=newH;
 				
 				#ifdef DBG
 				printf("<imgconvert>gif_resize end\n");
 				#endif
			}
			else
			{
				//Ximage	=im->sx;
				//Yimage	=im->sy;
				SOF0info.width	=(WORD)im->sx;
				SOF0info.height	=(WORD)im->sy;
 				if (Ximage%8!=0) 
 					Xdiv8=(Ximage/8)*8+8;
 				else 
	 				Xdiv8=Ximage;
            	
				if (Yimage%8!=0) 
 					Ydiv8=(Yimage/8)*8+8;
 				else 
 					Ydiv8=Yimage;
            	
				RGB_buffer=(colorRGB *)(malloc(4*Xdiv8*Ydiv8));
				for(i=0;i<Yimage;i++)
					for(j=0;j<Ximage;j++)
					{
						int idx = im->pixels[i][j];
						RGB_buffer[(i*Ximage)+j].B = im->blue[idx];
						RGB_buffer[(i*Ximage)+j].G = im->green[idx];
						RGB_buffer[(i*Ximage)+j].R = im->red[idx];
						//printf("%d-%x-%x-%x ",(i*Ximage)+j,RGB_buffer[(i*Ximage)+j].B,RGB_buffer[(i*Ximage)+j].G,RGB_buffer[(i*Ximage)+j].R);
					}
				#ifdef DBG
				printf("<imgconvert>gif_no resize end\n");
				#endif
			}
			break;
		case 'p':
			if(resize)
			{
				#ifdef DBG
				printf("<imgconvert>png newH %d,newW %d, Ximage %d,Yimage %d\n",
						newH,
						newW,
						Ximage,
						Yimage
						);
				#endif
				tmp_RGB = (colorRGB **) malloc (sizeof(colorRGB)*Yimage*3);
   				for(i=0;i<Yimage;i++)
   					tmp_RGB[i]= (colorRGB *) malloc (sizeof(colorRGB)*Ximage*4);
				
				for(i=0;i<Yimage;i++)
				{
					for(j=0;j<Ximage;j++)
					{
						tmp_RGB[i][j].R = RGB_buffer[(i*Ximage)+j].R;
						tmp_RGB[i][j].G = RGB_buffer[(i*Ximage)+j].G;
						tmp_RGB[i][j].B = RGB_buffer[(i*Ximage)+j].B;
					}
				}

				memset(RGB_buffer,0,sizeof(RGB_buffer));

				new_resolution(tmp_RGB, newW, newH);
				free(tmp_RGB);

				SOF0info.width=newW;
				SOF0info.height=newH;
				Ximage=newW;
				Yimage=newH;
 				if (Ximage%8!=0) 
 					Xdiv8=(Ximage/8)*8+8;
 				else 
 					Xdiv8=Ximage;

 				if (Yimage%8!=0) 
 					Ydiv8=(Yimage/8)*8+8;
 				else 
 					Ydiv8=Yimage;
 				#ifdef DBG
 				printf("<imgconvert>png_resize end\n");
 				#endif
			}
			else
			{
				SOF0info.width	= width;
				SOF0info.height	= height;
 				if (Ximage%8!=0) 
 					Xdiv8=(Ximage/8)*8+8;
 				else 
	 				Xdiv8=Ximage;
            	
				if (Yimage%8!=0) 
 					Ydiv8=(Yimage/8)*8+8;
 				else 
 					Ydiv8=Yimage;
 				#ifdef DBG
 				printf("<imgconvert>png_no resize end\n");
 				#endif
 			}
			break;
		case 't':
			if(resize)
			{
				#ifdef DBG
				printf("<imgconvert>tif newH %d,newW %d, Ximage %d,Yimage %d\n",
						newH,
						newW,
						Ximage,
						Yimage
						);
				#endif
				tmp_RGB = (colorRGB **) malloc (sizeof(colorRGB)*Yimage*3);
   				for(i=0;i<Yimage;i++)
   					tmp_RGB[i]= (colorRGB *) malloc (sizeof(colorRGB)*Ximage*4);
				
				for(i=0;i<Yimage;i++)
				{
					for(j=0;j<Ximage;j++)
					{
						tmp_RGB[i][j].R = RGB_buffer[(i*Ximage)+j].R;
						tmp_RGB[i][j].G = RGB_buffer[(i*Ximage)+j].G;
						tmp_RGB[i][j].B = RGB_buffer[(i*Ximage)+j].B;
					}
				}

				memset(RGB_buffer,0,sizeof(RGB_buffer));

				new_resolution(tmp_RGB, newW, newH);
				free(tmp_RGB);

				SOF0info.width=newW;
				SOF0info.height=newH;
				Ximage=newW;
				Yimage=newH;
 				if (Ximage%8!=0) 
 					Xdiv8=(Ximage/8)*8+8;
 				else 
 					Xdiv8=Ximage;

 				if (Yimage%8!=0) 
 					Ydiv8=(Yimage/8)*8+8;
 				else 
 					Ydiv8=Yimage;
 				#ifdef DBG
 				printf("<imgconvert>tif_resize end\n");
 				#endif
			}
			else // we don't need it
			{
				SOF0info.width	= width;
				SOF0info.height	= height;
 				if (Ximage%8!=0) 
 					Xdiv8=(Ximage/8)*8+8;
 				else 
	 				Xdiv8=Ximage;
            	
				if (Yimage%8!=0) 
 					Ydiv8=(Yimage/8)*8+8;
 				else 
 					Ydiv8=Yimage;
 				#ifdef DBG
 				printf("<imgconvert>tif_no resize end\n");
 				#endif
 			}
			break;
	}	

	writeword(0xFFD8); //SOI: Start of image
 	write_APP0info();//JFIF
	write_DQTinfo();//Quantization Table
	write_SOF0info();
	write_DHTinfo();//Huffman Table
	write_SOSinfo();

	bytenew=0;bytepos=7;
	main_encoder();
	//Do the bit alignment of the EOI marker
	if (bytepos>=0) {
		fillbits.length=bytepos+1;
		fillbits.value=(1<<(bytepos+1))-1;
		writebits(fillbits);
	}
 	writeword(0xFFD9); //EOI

 	free(RGB_buffer);
 	free(category_alloc);
 	free(bitcode_alloc);
	fclose(fp_jpeg_stream);
	if (ch == 'g') 
		gdImageDestroy(im);


	return 0;
}

