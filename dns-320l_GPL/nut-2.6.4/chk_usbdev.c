#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <ctype.h>
#include <usb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define PROGNAME  "chk_usbdev"
#define VERSION   "0.1"

int32_t dev_busnum = -1;
int32_t dev_devnum = -1;

int32_t debug = 0;

#define DEBUG_MODE 1
#ifdef DEBUG_MODE
#define DBG_PRINT(a...) {						\
	if(debug){									\
		printf(a);								\
		fflush(stdout);							\
}}
#else
#define DBG_PRINT(a...)    do {} while(0)
#endif



static int32_t chk_dev_ok(void)
{
	struct usb_bus *bus;
    struct usb_device *dev;
	usb_dev_handle *udev;
	int32_t busnum, devnum;
	
    for (bus = usb_get_busses(); bus; bus = bus->next) {
		
    	DBG_PRINT("bus->dirname = %s\n", bus->dirname);
    	busnum = atoi(bus->dirname);
    	if(busnum != dev_busnum)
    		continue;
    	
    	dev = NULL;
    	
	    for (dev = bus->devices; dev; dev = dev->next) {
	    	DBG_PRINT("dev->filename = %s\n",dev->filename);
			devnum = atoi(dev->filename);
			if(devnum != dev_devnum)
				continue;
			
			if((dev->descriptor.idVendor != 0) && (dev->descriptor.idProduct != 0)) {
				udev = usb_open(dev);
				if (!udev)
					return -1;
				usb_close(udev);
				return 0;
			}
	    }
    }
	
	return -1;
}


static void usage(int exit_code)
{
	printf("%s version %s\n", PROGNAME, VERSION);
	printf(" %s built\n", __DATE__" "__TIME__);
	printf("Description: This program will check the given usb device is able to be opened.\n");
	printf("             If the device is ok, it will return 0. Otherwise, it will return 1.\n");
	printf("Usage: %s [OPTIONS]\n", PROGNAME);
	printf("  -h                    show version and the help message.\n");
	printf("  -b                    bus number. Required. Positive value.\n");
	printf("  -d                    device number. Required. Positive value.\n");
	printf("  -D                    print the debug messages.\n");
	exit(exit_code);
}


int main(int argc, char *argv[])
{
    int32_t ret;

	int c = 0;
	
    while((c = getopt(argc, argv, "hb:d:D")) != -1)
    {
        switch (c)
        {
			case 'h':
				usage(EXIT_SUCCESS);
				break;
			case 'b':
				if(strlen(optarg) > 0)
					dev_busnum=(int32_t)atoi(optarg);
				break;
			case 'd':
				if(strlen(optarg) > 0)
					dev_devnum=(int32_t)atoi(optarg);
				break;
			case 'D':
				debug=1;
				break;
            default:
                break;
        }
    }
	
	if(dev_busnum < 0 || dev_devnum < 0)
	{
		usage(EXIT_FAILURE);
	}
				
    usb_init();
    usb_set_debug(debug);
    usb_find_busses();
    usb_find_devices();
	
    ret = chk_dev_ok();
    if(ret == 0){
		DBG_PRINT("This could be opened.\n");
		return 0;
	}
	else{
		DBG_PRINT("Can not open device!\n");
		return 1;
	}
    
    return 0;
}
