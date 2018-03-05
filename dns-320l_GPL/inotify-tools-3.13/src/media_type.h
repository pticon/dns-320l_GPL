/*
 * MODULE FILE NAME: media_type.h
 *
 * DESCRIPTION: UPNP AV has supported media type format.
 *              
 * NOTES:
 *
 * CREATOR:
 *          Vincent Chung, 2008/01/09
 * HISTORY:
 *          $Log: media_type.h
 *
 */

#ifndef __MEDIA_TYPE_H__
#define __MEDIA_TYPE_H__

//+Vincent05252007
/* FC_UNKNOWN is used for boundary check,
 * never declare a type_map_s with file_class that equals to FC_UNKNOWN
 */
#define FC_UNKNOWN 0
#define FC_IMAGE 1
#define FC_VIDEO 2
#define FC_AUDIO 3
#define FC_PLIST 4
#define FC_SUBTITLE 5
#define FC_TEXT 6

/* Mime-Types and File Extension Mappings */
typedef struct type_map_s
{
	char *file_ext;
	char *mime_type;
	char file_class;
}Type_Map_T;

// #define MIME_TYPE_NUMBER 46			//alpha.darren 20090331
static Type_Map_T CDS_FileType_Map[]=
{
	/* Image Formats */
	{ ".bmp" , "image/bmp", FC_IMAGE  },
	{ ".gif" , "image/gif", FC_IMAGE  },
	{ ".jpg" , "image/jpeg", FC_IMAGE },
	{ ".jpeg", "image/jpeg", FC_IMAGE },
	{ ".jpe" , "image/jpeg", FC_IMAGE },
	{ ".tif" , "image/tiff", FC_IMAGE },
	{ ".tiff", "image/tiff", FC_IMAGE },
	{ ".png" , "image/png", FC_IMAGE  },

	/* Audio Formats */
	{ ".aif" , "audio/aiff", FC_AUDIO },
	{ ".aiff", "audio/aiff", FC_AUDIO },
	{ ".wav" , "audio/wav", FC_AUDIO  },
	{ ".wma" , "audio/x-ms-wma", FC_AUDIO},
	{ ".mp3" , "audio/mpeg", FC_AUDIO },
	{ ".mp2" , "audio/mpeg", FC_AUDIO  },
	{ ".pcm" , "audio/L16", FC_AUDIO},
	{ ".lpcm", "audio/L16", FC_AUDIO },
	{ ".mp1" , "audio/mpeg", FC_AUDIO  },
	{ ".mpa" , "audio/mpeg", FC_AUDIO  }, 
	{ ".flac", "audio/x-flac", FC_AUDIO},	//Alpha.Vincent12232008
	{ ".aac", "audio/x-aac", FC_AUDIO},
	{ ".m4a", "audio/x-m4a", FC_AUDIO},
	{ ".ape", "audio/octet-stream", FC_AUDIO},
	{ ".ogg", "application/ogg", FC_AUDIO},		// alpha.jeffrey20090615  "audio/ogg" -> "application/ogg" for DSM-510
	{ ".oga", "audio/ogg", FC_AUDIO},
	{ ".mp4", "video/mp4", FC_VIDEO},
	{ ".adts", "audio/mpeg", FC_AUDIO  },
	{ ".apl", "application/octet-stream", FC_AUDIO},
	{ ".mka", "audio/x-matroska", FC_AUDIO},	//20110520

	/* Video Formats */
	{ ".avi" , "video/avi", FC_VIDEO },
	{ ".mpeg", "video/mpeg", FC_VIDEO },
	{ ".mpg" , "video/mpeg", FC_VIDEO },
	{ ".mpeg2","video/mpeg2", FC_VIDEO},
	{ ".wmv" , "video/x-ms-wmv", FC_VIDEO},
	{ ".dat" , "video/mpeg", FC_VIDEO},
	{ ".vob",  "video/mpeg", FC_VIDEO},
	{ ".ts",  "video/mpeg", FC_VIDEO},
	{ ".mov", "video/quicktime", FC_VIDEO},
	{ ".mkv", "video/x-matroska", FC_VIDEO},
	{ ".m2ts", "video/mpeg", FC_VIDEO},
	{ ".vc1", "video/vc1", FC_VIDEO},
	{ ".divx", "video/avi", FC_VIDEO},		// alpha.jeffrey20090615  "video/divx" -> "video/avi" ("video/x-msvideo" ?) for DSM-510
	{ ".flv", "video/x-flv", FC_VIDEO},
	{ ".dv", "video/x-dv", FC_VIDEO},
	{ ".ogv", "video/ogg", FC_VIDEO},
	{ ".iso", "application/octet-stream", FC_VIDEO},
	//{ ".ifo", "content/DVD", FC_VIDEO},
	{ ".ifo", "application/octet-stream", FC_VIDEO},
	{ ".tts", "video/mpeg", FC_VIDEO},
	{ ".mpeg4","video/mpeg4", FC_VIDEO},	//20110520

	/* Other Formats */
	{ ".m3u", "", FC_PLIST},
	{ ".pls", "", FC_PLIST},
	{ ".wpl", "", FC_PLIST},
	{ ".srt",  "application/octet-stream", FC_SUBTITLE},
	{ 0, 0, 0}
};

int GetFileClass(char *FileExt, int *idx);

#endif /* _MEDIA_TYPE_H */
