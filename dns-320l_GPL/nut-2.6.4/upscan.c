#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include <ctype.h>
#include <usb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// int get_interface(struct usb_interface *interface, usb_dev_handle *udev)
// {
	// int i, ret;
	// char name[256];

	// for (i = 0; i < interface->num_altsetting; i++) 
	// {
		// ret = usb_get_driver_np(udev, interface->altsetting[i].bInterfaceNumber, name, 256);

		// if (ret)
		// {
			// return 0;
			//#//continue;
		// }
		// else if (name[0] == '\0')
		// {
			// return -1;//printf("DRIVER: %s\n", name);
			//#//continue;
		// }
		// else if (strstr(name,"usbhid"))
		// {
			// return 0;
		// }
		// else 
		// {////#//printf("DRIVER: %s\n", name);
			// return 1;
		// }
	// }
// }

// int get_configuration(struct usb_config_descriptor *config, usb_dev_handle *udev)
// {
	// int i,ret;

	// for (i = 0; i < config->bNumInterfaces; i++) 
	// {
		// ret = get_interface(&config->interface[i], udev);
		// return ret;
	// }
// }
int get_interface(struct usb_interface *interface, usb_dev_handle *udev)
{
	int i, ret;
	int counter = 0;
	char name[256];

	for (i = 0; i < interface->num_altsetting; i++) {
		ret = usb_get_driver_np(udev, interface->altsetting[i].bInterfaceNumber, name, 256);

		if (ret)
			continue;
		else if (name[0] == '\0')
			continue;
		else {
			printf("DRIVER: %s\n", name);
			if (strstr(name , "usblp") || strstr(name , "hub")){
				counter ++;
			}
		}
	}
	if (!counter)
		return 0;
	else
		return -1;
}

int get_configuration(struct usb_config_descriptor *config, usb_dev_handle *udev)
{
	int i;
	int ret;
	int counter = 0;

	for (i = 0; i < config->bNumInterfaces; i++) {
		ret = get_interface(&config->interface[i], udev);
		if (ret == -1){
			counter ++;
		}
	}
	if (!counter)
		return 0;
	else 
		return -1;
}


int open_device(struct usb_device *dev)
{
	usb_dev_handle *udev;
	int ret, i;
	int retofget = 0;
	char string[256], description[256];

	udev = usb_open(dev);
	if (!udev)
		return -1;

	if (dev->descriptor.iManufacturer) {
		ret = usb_get_string_simple(udev, dev->descriptor.iManufacturer, string, sizeof(string));
		if (ret > 0)
			snprintf(description, sizeof(description), "%s - ", string);
		else
			snprintf(description, sizeof(description), "%04X - ",
					dev->descriptor.idVendor);
	} else {
		snprintf(description, sizeof(description), "%04X - ",
				dev->descriptor.idVendor);
	}

	if (dev->descriptor.iProduct) {
		ret = usb_get_string_simple(udev, dev->descriptor.iProduct, string, sizeof(string));
		if (ret > 0)
			snprintf(description + strlen(description), sizeof(description) -
					strlen(description), "%s", string);
		else
			snprintf(description + strlen(description), sizeof(description) -
					strlen(description), "%04X", dev->descriptor.idProduct);
	} else {
		snprintf(description + strlen(description), sizeof(description) -
				strlen(description), "%04X", dev->descriptor.idProduct);
	}

	printf("%.*sDev #%d: %s\n", 0, "                    ", 
			dev->devnum, description);

	if (dev->descriptor.iSerialNumber) {
		 ret = usb_get_string_simple(udev, dev->descriptor.iSerialNumber, string, sizeof(string));
		 if (ret > 0)
			 printf("Serial Number: %s\n" , string);
	}


	if (!dev->config) {
		printf("  Couldn't retrieve descriptors\n");
		return -1;
	}

	for (i = 0; i < dev->descriptor.bNumConfigurations; i++)
		retofget = get_configuration(&dev->config[i], udev);

	usb_close(udev);
	
	if (!retofget)
		return 0;
	else
		return -1;
}

int check_device() 
{
	int ret;
	int retofopen;
    struct usb_bus *bus;
	char string[128];
#if defined(ENABLE_RETRY_GET_USB_DESC) 
	/* 2013-06-26 Tim Tsay, A quick fix to DNS-340L, the UPS will not return their description at the first request sometime. */
	/* This fix refer to the upscan on DNS-320,325 GPL */
	int xi;
	const int max_desc_retry = 5;
#endif	
    for (bus = usb_get_busses(); bus; bus = bus->next) 
	{
	    struct usb_device *dev;
		usb_dev_handle *udev;
		
	    for (dev = bus->devices; dev; dev = dev->next) 
		{
			if((dev->descriptor.idVendor != 0) && (dev->descriptor.idProduct != 0))
			{
				if (dev->config->interface->altsetting->bInterfaceClass == 3) 
				{
					//printf("descriptor.idVendor = %x\n",dev->descriptor.idVendor);
					//printf("descriptor.idProduct = %x\n",dev->descriptor.idProduct);
					//printf("Correct Cls = %x\n",dev->config->interface->altsetting->bInterfaceClass);
					
					udev = usb_open(dev);
					if (!udev)
					{	
						usb_close(udev);
						return -1;
					}
					if (dev->descriptor.iProduct) 
					{
#if defined(ENABLE_RETRY_GET_USB_DESC) 
						/* 2013-06-26 Tim Tsay, A quick fix to DNS-340L, the UPS will not return their description at the first request sometime*/
						/* This fix refer to the upscan on DNS-320,325 GPL */
						
						for (xi = 0 ; xi < max_desc_retry ; xi ++)
						{
							ret = usb_get_string_simple(udev, dev->descriptor.iProduct, string, sizeof(string));
							if (ret > 0)
							{
								if((strstr(string,"Keyboard") != NULL)  || (strstr(string,"Mouse") != NULL) || (strstr(string,"Joystick") != NULL))
								{
									usb_close(udev);
									return -1;
								}
								usb_close(udev);
								//printf("Get descriptor = %s\n", string);
								return 0;
							}
						}
#else
						ret = usb_get_string_simple(udev, dev->descriptor.iProduct, string, sizeof(string));
						if (ret > 0)
						{
							if((strstr(string,"Keyboard") != NULL)  || (strstr(string,"Mouse") != NULL) || (strstr(string,"Joystick") != NULL))
							{
								usb_close(udev);
								return -1;
							}
							usb_close(udev);
							return 0;
							//check_device
						}
#endif
					}
				}
				else if (dev->config->interface->altsetting->bInterfaceClass == 0xff)
				{
					udev = usb_open(dev);
					if (!udev)
					{	
						usb_close(udev);
						return -1;
					}
					if (dev->descriptor.iProduct) 
					{
						ret = usb_get_string_simple(udev, dev->descriptor.iProduct, string, sizeof(string));
						if (ret > 0)
						{
							if((strstr(string,"Keyboard") != NULL)  || (strstr(string,"Mouse") != NULL) || (strstr(string,"Joystick") != NULL))
							{
								usb_close(udev);
								return -1;
							}
							usb_close(udev);
							retofopen = open_device(dev);
							if (!retofopen)
							return 0;
							else 
								return -1;
							//check_device
						}		
					}
				}
				else{
					continue ;
				}
			}
		}	
    }
	return -1;
}

int main(int argc, char *argv[])
{
    int ret;

    usb_init();
    usb_set_debug(0);
    usb_find_busses();
    usb_find_devices();
    
	// if((w_lock = stat("/tmp/ups_loading",&stb)) >=0)
	// {
		// if((fpp = stat("/tmp/ups_sok",&sta)) >= 0 || (fpp = stat("/tmp/ups_sno",&sta)) >= 0){
			// system("rm /tmp/ups_loading");
		// }
	// }
    ret = check_device();
    if(ret == 0){
		printf("it may a UPS device .\n");
		system("touch /tmp/upsin");
		return 0;
	}
	else{
		printf("This may not a UPS device , please check it\n");
		return 0;
	}
	//main
}

//main
	// if (ret == 1) {
		// printf("Get Over\n");
		// return 0;    
    // }
	// else if (ret == 0){
		// printf("It may a UPS Device!    Try to mount Driver...\n");
		// if((fpp = stat("/tmp/ups_sok",&sta)) < 0 && (fpp = stat("/tmp/ups_sno",&sta)) < 0){
			// if((w_lock = stat("/tmp/ups_loading",&stb)) <0){
				// system("touch /tmp/ups_loading");
				// system("upsc start &");
				// return 0;
			// }
			// else{
				// return 0;
			// }		
		// }
		// else{
			// system("upsc stop");
			// sleep(1);
			// if((w_lock = stat("/tmp/ups_loading",&stb)) <0){
				// system("touch /tmp/ups_loading");
				// system("upsc start &");
				// return 0;
			// }
			// else{
				// return 0;
			// }
		// }
	// }
	// else{
		// printf("not a UPS , check tmp\n");
		// if((fpp = stat("/tmp/ups_sok",&sta)) >= 0 || (fpp = stat("/tmp/ups_sno",&sta)) >= 0){
			// system("upsc stop");
			// return -1;
		// }	
	// }

//check_device
// for (i = 0; i < dev->descriptor.bNumConfigurations; i++)
							// {
								// for (i = 0; i < dev->descriptor.bNumConfigurations; i++)
								// rbk_getdrv = get_configuration(&dev->config[i], udev);
								// if(rbk_getdrv == 1)
								// {
									// usb_close(udev);
									// return 1;
								// }
								// else if(rbk_getdrv == 0)
								// {
									// usb_close(udev);
									// return 0;
								// }
								// else
								// {
									// usb_close(udev);
									// return -1;
								// }
							// }


