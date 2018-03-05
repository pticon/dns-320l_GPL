//--------------------------------------------------------------------------
// Include file for jhead program.
//
// This include file only defines stuff that goes across modules.  
// I like to keep the definitions for macros and structures as close to 
// where they get used as possible, so include files only get stuff that 
// gets used in more than one file.
//--------------------------------------------------------------------------
#define _CRT_SECURE_NO_DEPRECATE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>

//--------------------------------------------------------------------------

#ifdef _WIN32
    #include <sys/utime.h>
#else
    #include <utime.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <errno.h>
    #include <limits.h>
#endif


typedef unsigned char uchar;

#ifndef TRUE
    #define TRUE 1
    #define FALSE 0
#endif

#define MAX_COMMENT_SIZE 2000

#ifdef _WIN32
    #define PATH_MAX _MAX_PATH
    #define SLASH '\\'
#else
    #ifndef PATH_MAX
        #define PATH_MAX 1024
    #endif
    #define SLASH '/'
#endif

#define DIR_ENTRY_ADDR(Start, Entry) (Start+2+12*(Entry))
//--------------------------------------------------------------------------
// This structure is used to store jpeg file sections in memory.
typedef struct {
    uchar *  Data;
    int      Type;
    unsigned Size;
}Section_t;

extern int ExifSectionIndex;

extern int DumpExifMap;

#define MAX_DATE_COPIES 10

//--------------------------------------------------------------------------
// This structure is used to store jpeg file sections in memory.
typedef struct {
	char *GpsLat;
    char *GpsLong;
    char *GpsAlt;
}Gpsinfo_t;

//--------------------------------------------------------------------------
// This structure stores Exif header image elements in a simple manner
// Used to store camera data as extracted from the various ways that it can be
// stored in an exif header
typedef struct {
    char  FileName     [PATH_MAX+1];
    time_t FileDateTime;

    struct {
        // Info in the jfif header.
        // This info is not used much - jhead used to just replace it with default
        // values, and over 10 years, only two people pointed this out.
        char  Present;
        char  ResolutionUnits;
        short XDensity;
        short YDensity;
    }JfifHeader;
	int	  ExifPresent;
    unsigned FileSize;
    char  CameraMake   [32];
    char  CameraModel  [40];
    char  DateTime     [20];
    int   Height, Width;
    int   Orientation;
    int   IsColor;
    int   Process;
    int   FlashUsed;
    float FocalLength;
    float ExposureTime;
    float ApertureFNumber;
    float Distance;
    float CCDWidth;
    float ExposureBias;
    float DigitalZoomRatio;
    int   FocalLength35mmEquiv; // Exif 2.2 tag - usually not present.
    int   Whitebalance;
    int   MeteringMode;
    int   ExposureProgram;
    int   ExposureMode;
    int   ISOequivalent;
    int   LightSource;
    int   DistanceRange;

    float xResolution;
    float yResolution;
    int   ResolutionUnit;

    char  Comments[MAX_COMMENT_SIZE];
    int   CommentWidthchars; // If nonzero, widechar comment, indicates number of chars.

    unsigned ThumbnailOffset;          // Exif offset to thumbnail
    unsigned ThumbnailSize;            // Size of thumbnail.
    unsigned LargestExifOffset;        // Last exif data referenced (to check if thumbnail is at end)

    char  ThumbnailAtEnd;              // Exif header ends with the thumbnail
                                       // (we can only modify the thumbnail if its at the end)
    int   ThumbnailSizeOffset;

    int  DateTimeOffsets[MAX_DATE_COPIES];
    int  numDateTimeTags;

    int GpsInfoPresent;
    char GpsLat[31];
    char GpsLong[31];
    char GpsAlt[20];
}ImageInfo_t;



#define EXIT_FAILURE  1
#define EXIT_SUCCESS  0

// jpgfile.c functions
typedef enum {
    READ_METADATA = 1,
    READ_IMAGE = 2,
    READ_ALL = 3,
    READ_ANY = 5        // Don't abort on non-jpeg files.
}ReadMode_t;


// prototypes for jhead.c functions
void ErrFatal(char * msg);
void ErrNonfatal(char * msg, int a1, int a2);
void FileTimeAsString(char * TimeStr);

// Prototypes for exif.c functions.
int Exif2tm(struct tm * timeptr, char * ExifTime);
void process_EXIF (unsigned char * CharBuf, unsigned int length);
int RemoveThumbnail(unsigned char * ExifSection);
void ShowImageInfo(int ShowFileInfo);
void ShowConciseImageInfo(void);
const char * ClearOrientation(void);
void PrintFormatNumber(void * ValuePtr, int Format, int ByteCount);
double ConvertAnyFormat(void * ValuePtr, int Format);
int Get16u(void * Short);
void Put16u(void * Short, unsigned short PutValue);
unsigned Get32u(void * Long);
int Get32s(void * Long);
void Put32u(void * Value, unsigned PutValue);
void create_EXIF(void);
int exif_set_tag(unsigned char *ExifSection,int tag,void *param);
unsigned char *exif_find_tag(unsigned char *DirStart, unsigned char *OffsetBase, int tag);


//--------------------------------------------------------------------------
// Exif format descriptor stuff
extern const int BytesPerFormat[];
#define NUM_FORMATS 12

#define FMT_BYTE       1 
#define FMT_STRING     2
#define FMT_USHORT     3
#define FMT_ULONG      4
#define FMT_URATIONAL  5
#define FMT_SBYTE      6
#define FMT_UNDEFINED  7
#define FMT_SSHORT     8
#define FMT_SLONG      9
#define FMT_SRATIONAL 10
#define FMT_SINGLE    11
#define FMT_DOUBLE    12

// makernote.c prototypes
extern void ProcessMakerNote(unsigned char * DirStart, int ByteCount,
                 unsigned char * OffsetBase, unsigned ExifLength);

// gpsinfo.c prototypes
void ProcessGpsInfo(unsigned char * ValuePtr, int ByteCount, 
                unsigned char * OffsetBase, unsigned ExifLength);
void SetGpsInfo( unsigned char * DirStart, Gpsinfo_t *gpsinfo, unsigned char *OffsetBase );
int create_gpsinfo(unsigned char *Buffer, int DataIndex );

// iptc.c prototpyes
void show_IPTC (unsigned char * CharBuf, unsigned int length);
void ShowXmp(Section_t XmpSection);

// Prototypes for myglob.c module
#ifdef _WIN32
void MyGlob(const char * Pattern , void (*FileFuncParm)(const char * FileName));
void SlashToNative(char * Path);
#endif

// Prototypes for paths.c module
int EnsurePathExists(const char * FileName);
void CatPath(char * BasePath, const char * FilePath);

// Prototypes from jpgfile.c
int ReadJpegSections (FILE * infile, ReadMode_t ReadMode);
void DiscardData(void);
void DiscardAllButExif(void);
int ReadJpegFile(const char * FileName, ReadMode_t ReadMode);
int ReplaceThumbnail(const char * ThumbFileName);
int SaveThumbnail(char * ThumbFileName);
int RemoveSectionType(int SectionType);
int RemoveUnknownSections(void);
void WriteJpegFile(const char * FileName);
Section_t * FindSection(int SectionType);
Section_t * CreateSection(int SectionType, unsigned char * Data, int size);
void ResetJpgfile(void);


// Variables from jhead.c used by exif.c
extern ImageInfo_t ImageInfo;
extern int ShowTags;

//--------------------------------------------------------------------------
// JPEG markers consist of one or more 0xFF bytes, followed by a marker
// code byte (which is not an FF).  Here are the marker codes of interest
// in this program.  (See jdmarker.c for a more complete list.)
//--------------------------------------------------------------------------

#define M_SOF0  0xC0          // Start Of Frame N
#define M_SOF1  0xC1          // N indicates which compression process
#define M_SOF2  0xC2          // Only SOF0-SOF2 are now in common use
#define M_SOF3  0xC3
#define M_SOF5  0xC5          // NB: codes C4 and CC are NOT SOF markers
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8          // Start Of Image (beginning of datastream)
#define M_EOI   0xD9          // End Of Image (end of datastream)
#define M_SOS   0xDA          // Start Of Scan (begins compressed data)
#define M_JFIF  0xE0          // Jfif marker
#define M_EXIF  0xE1          // Exif marker.  Also used for XMP data!
#define M_XMP   0x10E1        // Not a real tag (same value in file as Exif!)
#define M_COM   0xFE          // COMment 
#define M_DQT   0xDB
#define M_DHT   0xC4
#define M_DRI   0xDD
#define M_IPTC  0xED          // IPTC marker

//--------------------------------------------------------------------------
// Describes tag values

#define TAG_INTEROP_INDEX          0x0001
#define TAG_INTEROP_VERSION        0x0002
#define TAG_IMAGE_WIDTH            0x0100
#define TAG_IMAGE_LENGTH           0x0101
#define TAG_BITS_PER_SAMPLE        0x0102
#define TAG_COMPRESSION            0x0103
#define TAG_PHOTOMETRIC_INTERP     0x0106
#define TAG_FILL_ORDER             0x010A
#define TAG_DOCUMENT_NAME          0x010D
#define TAG_IMAGE_DESCRIPTION      0x010E
#define TAG_MAKE                   0x010F
#define TAG_MODEL                  0x0110
#define TAG_SRIP_OFFSET            0x0111
#define TAG_ORIENTATION            0x0112
#define TAG_SAMPLES_PER_PIXEL      0x0115
#define TAG_ROWS_PER_STRIP         0x0116
#define TAG_STRIP_BYTE_COUNTS      0x0117
#define TAG_X_RESOLUTION           0x011A
#define TAG_Y_RESOLUTION           0x011B
#define TAG_PLANAR_CONFIGURATION   0x011C
#define TAG_RESOLUTION_UNIT        0x0128
#define TAG_TRANSFER_FUNCTION      0x012D
#define TAG_SOFTWARE               0x0131
#define TAG_DATETIME               0x0132
#define TAG_ARTIST                 0x013B
#define TAG_WHITE_POINT            0x013E
#define TAG_PRIMARY_CHROMATICITIES 0x013F
#define TAG_TRANSFER_RANGE         0x0156
#define TAG_JPEG_PROC              0x0200
#define TAG_THUMBNAIL_OFFSET       0x0201
#define TAG_THUMBNAIL_LENGTH       0x0202
#define TAG_Y_CB_CR_COEFFICIENTS   0x0211
#define TAG_Y_CB_CR_SUB_SAMPLING   0x0212
#define TAG_Y_CB_CR_POSITIONING    0x0213
#define TAG_REFERENCE_BLACK_WHITE  0x0214
#define TAG_RELATED_IMAGE_WIDTH    0x1001
#define TAG_RELATED_IMAGE_LENGTH   0x1002
#define TAG_CFA_REPEAT_PATTERN_DIM 0x828D
#define TAG_CFA_PATTERN1           0x828E
#define TAG_BATTERY_LEVEL          0x828F
#define TAG_COPYRIGHT              0x8298
#define TAG_EXPOSURETIME           0x829A
#define TAG_FNUMBER                0x829D
#define TAG_IPTC_NAA               0x83BB
#define TAG_EXIF_OFFSET            0x8769
#define TAG_INTER_COLOR_PROFILE    0x8773
#define TAG_EXPOSURE_PROGRAM       0x8822
#define TAG_SPECTRAL_SENSITIVITY   0x8824
#define TAG_GPSINFO                0x8825
#define TAG_ISO_EQUIVALENT         0x8827
#define TAG_OECF                   0x8828
#define TAG_EXIF_VERSION           0x9000
#define TAG_DATETIME_ORIGINAL      0x9003
#define TAG_DATETIME_DIGITIZED     0x9004
#define TAG_COMPONENTS_CONFIG      0x9101
#define TAG_CPRS_BITS_PER_PIXEL    0x9102
#define TAG_SHUTTERSPEED           0x9201
#define TAG_APERTURE               0x9202
#define TAG_BRIGHTNESS_VALUE       0x9203
#define TAG_EXPOSURE_BIAS          0x9204
#define TAG_MAXAPERTURE            0x9205
#define TAG_SUBJECT_DISTANCE       0x9206
#define TAG_METERING_MODE          0x9207
#define TAG_LIGHT_SOURCE           0x9208
#define TAG_FLASH                  0x9209
#define TAG_FOCALLENGTH            0x920A
#define TAG_SUBJECTAREA            0x9214
#define TAG_MAKER_NOTE             0x927C
#define TAG_USERCOMMENT            0x9286
#define TAG_SUBSEC_TIME            0x9290
#define TAG_SUBSEC_TIME_ORIG       0x9291
#define TAG_SUBSEC_TIME_DIG        0x9292

#define TAG_WINXP_TITLE            0x9c9b // Windows XP - not part of exif standard.
#define TAG_WINXP_COMMENT          0x9c9c // Windows XP - not part of exif standard.
#define TAG_WINXP_AUTHOR           0x9c9d // Windows XP - not part of exif standard.
#define TAG_WINXP_KEYWORDS         0x9c9e // Windows XP - not part of exif standard.
#define TAG_WINXP_SUBJECT          0x9c9f // Windows XP - not part of exif standard.

#define TAG_FLASH_PIX_VERSION      0xA000
#define TAG_COLOR_SPACE            0xA001
#define TAG_PIXEL_X_DIMENSION      0xA002
#define TAG_PIXEL_Y_DIMENSION      0xA003
#define TAG_RELATED_AUDIO_FILE     0xA004
#define TAG_INTEROP_OFFSET         0xA005
#define TAG_FLASH_ENERGY           0xA20B
#define TAG_SPATIAL_FREQ_RESP      0xA20C
#define TAG_FOCAL_PLANE_XRES       0xA20E
#define TAG_FOCAL_PLANE_YRES       0xA20F
#define TAG_FOCAL_PLANE_UNITS      0xA210
#define TAG_SUBJECT_LOCATION       0xA214
#define TAG_EXPOSURE_INDEX         0xA215
#define TAG_SENSING_METHOD         0xA217
#define TAG_FILE_SOURCE            0xA300
#define TAG_SCENE_TYPE             0xA301
#define TAG_CFA_PATTERN            0xA302
#define TAG_CUSTOM_RENDERED        0xA401
#define TAG_EXPOSURE_MODE          0xA402
#define TAG_WHITEBALANCE           0xA403
#define TAG_DIGITALZOOMRATIO       0xA404
#define TAG_FOCALLENGTH_35MM       0xA405
#define TAG_SCENE_CAPTURE_TYPE     0xA406
#define TAG_GAIN_CONTROL           0xA407
#define TAG_CONTRAST               0xA408
#define TAG_SATURATION             0xA409
#define TAG_SHARPNESS              0xA40A
#define TAG_DISTANCE_RANGE         0xA40C
#define TAG_IMAGE_UNIQUE_ID        0xA420

#define TAG_GPS_LAT_REF		1
#define TAG_GPS_LAT			2
#define TAG_GPS_LONG_REF	3
#define TAG_GPS_LONG		4
#define TAG_GPS_ALT_REF		5
#define TAG_GPS_ALT			6
#define TAG_GPS_TIMESTAMP	7

#define MAX_GPS_TAG 0x1e