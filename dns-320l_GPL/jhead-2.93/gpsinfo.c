//--------------------------------------------------------------------------
// Parsing of GPS info from exif header.
//
// Matthias Wandel,  Dec 1999 - Dec 2002 
//--------------------------------------------------------------------------
#include <math.h>
#include "jhead.h"

static const char * GpsTags[MAX_GPS_TAG+1]= {
    "VersionID       ",//0x00  
    "LatitudeRef     ",//0x01  
    "Latitude        ",//0x02  
    "LongitudeRef    ",//0x03  
    "Longitude       ",//0x04  
    "AltitudeRef     ",//0x05  
    "Altitude        ",//0x06  
    "TimeStamp       ",//0x07  
    "Satellites      ",//0x08  
    "Status          ",//0x09  
    "MeasureMode     ",//0x0A  
    "DOP             ",//0x0B  
    "SpeedRef        ",//0x0C  
    "Speed           ",//0x0D  
    "TrackRef        ",//0x0E  
    "Track           ",//0x0F  
    "ImgDirectionRef ",//0x10  
    "ImgDirection    ",//0x11  
    "MapDatum        ",//0x12  
    "DestLatitudeRef ",//0x13  
    "DestLatitude    ",//0x14  
    "DestLongitudeRef",//0x15  
    "DestLongitude   ",//0x16  
    "DestBearingRef  ",//0x17  
    "DestBearing     ",//0x18  
    "DestDistanceRef ",//0x19  
    "DestDistance    ",//0x1A  
    "ProcessingMethod",//0x1B  
    "AreaInformation ",//0x1C  
    "DateStamp       ",//0x1D  
    "Differential    ",//0x1E
};

//--------------------------------------------------------------------------
// Process GPS info directory
//--------------------------------------------------------------------------
void ProcessGpsInfo(unsigned char * DirStart, int ByteCountUnused, unsigned char * OffsetBase, unsigned ExifLength)
{
    int de;
    unsigned a;
    int NumDirEntries;

    NumDirEntries = Get16u(DirStart);
    
    if (ShowTags){
        printf("(dir has %d entries)\n",NumDirEntries);
    }

    ImageInfo.GpsInfoPresent = TRUE;
    strcpy(ImageInfo.GpsLat, "? ?");
    strcpy(ImageInfo.GpsLong, "? ?");
    ImageInfo.GpsAlt[0] = 0; 

    for (de=0;de<NumDirEntries;de++){
        unsigned Tag, Format, Components;
        unsigned char * ValuePtr;
        int ComponentSize;
        unsigned ByteCount;
        unsigned char * DirEntry;
        DirEntry = DIR_ENTRY_ADDR(DirStart, de);

        if (DirEntry+12 > OffsetBase+ExifLength){
            ErrNonfatal("GPS info directory goes past end of exif",0,0);
            return;
        }

        Tag = Get16u(DirEntry);
        Format = Get16u(DirEntry+2);
        Components = Get32u(DirEntry+4);
     
        if ((Format-1) >= NUM_FORMATS) {
            // (-1) catches illegal zero case as unsigned underflows to positive large.
            ErrNonfatal("Illegal number format %d for Exif gps tag %04x", Format, Tag);
            continue;
        }

        ComponentSize = BytesPerFormat[Format];
        ByteCount = Components * ComponentSize;

        if (ByteCount > 4){
            unsigned OffsetVal;
            OffsetVal = Get32u(DirEntry+8);
            // If its bigger than 4 bytes, the dir entry contains an offset.
            if (OffsetVal+ByteCount > ExifLength){
                // Bogus pointer offset and / or bytecount value
                ErrNonfatal("Illegal value pointer for Exif gps tag %04x", Tag,0);
                continue;
            }
            ValuePtr = OffsetBase+OffsetVal;
        }else{
            // 4 bytes or less and value is in the dir entry itself
            ValuePtr = DirEntry+8;
        }

        switch(Tag){
            char FmtString[21];
            char TempString[50];
            double Values[3];

            case TAG_GPS_LAT_REF:
                ImageInfo.GpsLat[0] = ValuePtr[0];
                break;

            case TAG_GPS_LONG_REF:
                ImageInfo.GpsLong[0] = ValuePtr[0];
                break;

            case TAG_GPS_LAT:
            case TAG_GPS_LONG:
                if (Format != FMT_URATIONAL){
                    ErrNonfatal("Inappropriate format (%d) for Exif GPS coordinates!", Format, 0);
                }
                strcpy(FmtString, "%0.0fd %0.0fm %0.0fs");
                for (a=0;a<3;a++){
                    int den, digits;

                    den = Get32s(ValuePtr+4+a*ComponentSize);
                    digits = 0;
                    while (den > 1 && digits <= 6){
                        den = den / 10;
                        digits += 1;
                    }
                    if (digits > 6) digits = 6;
                    FmtString[1+a*7] = (char)('2'+digits+(digits ? 1 : 0));
                    FmtString[3+a*7] = (char)('0'+digits);

                    Values[a] = ConvertAnyFormat(ValuePtr+a*ComponentSize, Format);
                }

                sprintf(TempString, FmtString, Values[0], Values[1], Values[2]);

                if (Tag == TAG_GPS_LAT){
                    strncpy(ImageInfo.GpsLat+2, TempString, 29);
                }else{
                    strncpy(ImageInfo.GpsLong+2, TempString, 29);
                }
                break;

            case TAG_GPS_ALT_REF:
                ImageInfo.GpsAlt[0] = (char)(ValuePtr[0] ? '-' : ' ');
                break;

            case TAG_GPS_ALT:
                sprintf(ImageInfo.GpsAlt + 1, "%.2fm", 
                    ConvertAnyFormat(ValuePtr, Format));
                break;
        }

        if (ShowTags){
            // Show tag value.
            if (Tag < MAX_GPS_TAG){
                printf("        GPS%s =", GpsTags[Tag]);
            }else{
                // Show unknown tag
                printf("        Illegal GPS tag %04x=", Tag);
            }

            switch(Format){
                case FMT_UNDEFINED:
                    // Undefined is typically an ascii string.

                case FMT_STRING:
                    // String arrays printed without function call (different from int arrays)
                    {
                        printf("\"");
                        for (a=0;a<ByteCount;a++){
                            int ZeroSkipped = 0;
                            if (ValuePtr[a] >= 32){
                                if (ZeroSkipped){
                                    printf("?");
                                    ZeroSkipped = 0;
                                }
                                putchar(ValuePtr[a]);
                            }else{
                                if (ValuePtr[a] == 0){
                                    ZeroSkipped = 1;
                                }
                            }
                        }
                        printf("\"\n");
                    }
                    break;

                default:
                    // Handle arrays of numbers later (will there ever be?)
                    for (a=0;;){
                        PrintFormatNumber(ValuePtr+a*ComponentSize, Format, ByteCount);
                        if (++a >= Components) break;
                        printf(", ");
                    }
                    printf("\n");
            }
        }
    }
}
//------------------------------------------------------------------
int create_gpsinfo(unsigned char *Buffer, int DataIndex )
{
	int NumEntries;
	int DirIndex;
	int DataWriteIndex;
	
	if(!Buffer)
		return 0;
		
	DirIndex = 0;
    NumEntries = 7;
    DataWriteIndex = DataIndex + 2 + NumEntries*12 + 4;
	//printf("dataindex=%x datawrite=%x\n",DataIndex,DataWriteIndex);
    Put16u(Buffer+DirIndex, NumEntries);			// Number of entries
    DirIndex += 2;
   
   
    Put16u(Buffer+DirIndex, TAG_GPS_LAT_REF);		// Tag
    Put16u(Buffer+DirIndex + 2, FMT_STRING);		// Format
    Put32u(Buffer+DirIndex + 4, 2);					// Components
    Put32u(Buffer+DirIndex + 8, 0x4E); 				// Pointer or value. "N or S"
    DirIndex += 12;

    Put16u(Buffer+DirIndex, TAG_GPS_LAT);			// Tag
    Put16u(Buffer+DirIndex + 2, FMT_URATIONAL);		// Format
    Put32u(Buffer+DirIndex + 4, 3);					// Components
    Put32u(Buffer+DirIndex + 8, DataWriteIndex-8);	// Pointer or value.
	DirIndex += 12;
		
	DataWriteIndex += 24;
	
	Put16u(Buffer+DirIndex, TAG_GPS_LONG_REF);		// Tag
    Put16u(Buffer+DirIndex + 2, FMT_STRING);		// Format
    Put32u(Buffer+DirIndex + 4, 2);					// Components
    Put32u(Buffer+DirIndex + 8, 0x57); 				// Pointer or value. "E or W"
    DirIndex += 12;
    
	Put16u(Buffer+DirIndex, TAG_GPS_LONG);			// Tag
    Put16u(Buffer+DirIndex + 2, FMT_URATIONAL);		// Format
    Put32u(Buffer+DirIndex + 4, 3);					// Components
    Put32u(Buffer+DirIndex + 8, DataWriteIndex-8);	// Pointer or value.
	DirIndex += 12;
	
	DataWriteIndex += 24;

	Put16u(Buffer+DirIndex, TAG_GPS_ALT_REF);		// Tag
    Put16u(Buffer+DirIndex + 2, FMT_BYTE);			// Format
    Put32u(Buffer+DirIndex + 4, 1);					// Components
    Put32u(Buffer+DirIndex + 8, 0); 				// Pointer or value.
    DirIndex += 12;
    
	Put16u(Buffer+DirIndex, TAG_GPS_ALT);			// Tag
    Put16u(Buffer+DirIndex + 2, FMT_URATIONAL);		// Format
    Put32u(Buffer+DirIndex + 4, 1);					// Components
    Put32u(Buffer+DirIndex + 8, DataWriteIndex-8);	// Pointer or value.
	DirIndex += 12;

	DataWriteIndex += 8;
	
	Put16u(Buffer+DirIndex, TAG_GPS_TIMESTAMP);		// Tag
    Put16u(Buffer+DirIndex + 2, FMT_URATIONAL);		// Format
    Put32u(Buffer+DirIndex + 4, 3);					// Components
    Put32u(Buffer+DirIndex + 8, DataWriteIndex-8);	// Pointer or value.
	DirIndex += 12;

	
	DataWriteIndex += 24;
	
	// End of directory - contains optional link to continued directory.
    Put32u(Buffer+DirIndex, 0);
    
	return (DataWriteIndex-DataIndex);
}
//------------------------------------------------------------------
static void SetUrational( unsigned char * DirStart, double value )
{
	int den;
	double integral;
	double fractional;
	
	
	den = 1;	
	fractional = modf(value, &integral);
	while(fractional && den < 1000){
		den *= 10;
		fractional = modf((value*den), &integral);
	}
	
	Put32u(DirStart,integral );
	Put32u(DirStart+4, den );
}

//------------------------------------------------------------------
void SetGpsInfo( unsigned char * DirStart, Gpsinfo_t *gpsinfo, unsigned char *OffsetBase )
{
	int de;
	int NumDirEntries;
    int Tag, Format, Components;
    int OffsetVal;
    unsigned char * DirEntry;
    char *p;
    char LATInfo[4][8];
    char LONGInfo[4][8];
    char ALTInfo[2][8];
    unsigned tmp;
    
    // parse gpsinfo
    de = 0;
    p = strtok(gpsinfo->GpsLat, ":");
    sprintf(&LATInfo[de][0], p);
    while( ( p = strtok(NULL, ":")) && de++ < 4){
    	sprintf(&LATInfo[de][0], p);
    }
    //for(de = 0; de <4; de++){
	//	printf("%s\n",LATInfo[de]);
	//}
	de = 0;
    p = strtok(gpsinfo->GpsLong, ":");
    sprintf(&LONGInfo[de][0], p);
    while( ( p = strtok(NULL, ":")) && de++ < 4){
    	sprintf(&LONGInfo[de][0], p);
    }
	
	de = 0;
    p = strtok(gpsinfo->GpsAlt, ":");
    sprintf(&ALTInfo[de][0], p);
    while( ( p = strtok(NULL, ":")) && de++ < 4){
    	sprintf(&ALTInfo[de][0], p);
    }
		
    // parse entry
    NumDirEntries = Get16u(DirStart);
    
    for (de=0;de<NumDirEntries;de++){	
    	
    	DirEntry = DIR_ENTRY_ADDR(DirStart, de);
    	
    	Tag = Get16u(DirEntry);
        Format = Get16u(DirEntry+2);
        Components = Get32u(DirEntry+4);
        
        switch(Tag){
			case TAG_GPS_LAT_REF:
				//printf("LAT = %x\n",*LATInfo[0]); 
				Put32u(&tmp, *LATInfo[0]);
                Put32u(DirEntry + 8, tmp);
                break;

            case TAG_GPS_LONG_REF:
            	//printf("LONG = %d\n",*LONGInfo[0]);
            	Put32u(&tmp, *LONGInfo[0]);
                Put32u(DirEntry + 8, tmp);
                break;

            case TAG_GPS_LAT:
            	OffsetVal = Get32u(DirEntry+8);
            	SetUrational( OffsetBase+OffsetVal, atof(LATInfo[1]));
            	SetUrational( OffsetBase+OffsetVal+8, atof(LATInfo[2]));
            	SetUrational( OffsetBase+OffsetVal+16, atof(LATInfo[3]));
            	break;
            case TAG_GPS_LONG:
            	OffsetVal = Get32u(DirEntry+8);
 				SetUrational( OffsetBase+OffsetVal, atof(LONGInfo[1]));
            	SetUrational( OffsetBase+OffsetVal+8, atof(LONGInfo[2]));
            	SetUrational( OffsetBase+OffsetVal+16, atof(LONGInfo[3]));
                break;

            case TAG_GPS_ALT_REF:
            	// Sea level 0:above the sea level / 1:below the sea level 
                Put32u(DirEntry + 8, *ALTInfo[0]);
                break;

            case TAG_GPS_ALT:
            	OffsetVal = Get32u(DirEntry+8);
            	SetUrational( OffsetBase+OffsetVal,atof(ALTInfo[1]) );
                break;
                
            default:
            	break;    
        }
        
     
    }
}