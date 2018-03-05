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

int get_interface(struct usb_interface *interface, usb_dev_handle *udev)
{
	int i, ret;
	int counter = 0;
	char name[256];
	
	memset(name,'\0',sizeof(name));
	
	for (i = 0; i < interface->num_altsetting; i++) {
		ret = usb_get_driver_np(udev, interface->altsetting[i].bInterfaceNumber, name, 256);
		if (ret)
			continue;	
		
		else if (name[0] == '\0')
			continue;
		
		else {
			if (strstr(name,"hub")){
				continue;
			}
			++ counter;
		}
		memset(name,'\0',sizeof(name));
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

int check_device(int dc_flag) 
{
	int ret;
	int retofopen;
    struct usb_bus *bus;
	char string[128];
	
    for (bus = usb_get_busses(); bus; bus = bus->next) {
	    struct usb_device *dev;
		usb_dev_handle *udev;
			

	    for (dev = bus->devices; dev; dev = dev->next) {
			printf("Vendor = 0x%04x\t",dev->descriptor.idVendor);
					printf("Product = 0x%04x\n",dev->descriptor.idProduct);
					printf("Cls = 0x%02x\n",dev->config->interface->altsetting->bInterfaceClass);


			if((dev->descriptor.idVendor != 0) && (dev->descriptor.idProduct != 0)){
			    if (dc_flag) {
					if (dev->config->interface->altsetting->bInterfaceClass == 0x06) {
						printf("Vendor = 0x%04x\t",dev->descriptor.idVendor);
						printf("Product = 0x%04x\n",dev->descriptor.idProduct);
						printf("Cls = 0x%02x\n",dev->config->interface->altsetting->bInterfaceClass);
					
						udev = usb_open(dev);
						if (!udev) {	
							usb_close(udev);
							return -1;
						}
						else {
							usb_close(udev);
								//system("up_send_ctl usbExist 1");
							system("usb_disk mtp add");
							system("touch /tmp/dc_in");
							return 1;
						}
					}
					else {
						if (!access("/tmp/dc_in",F_OK))
							system("rm /tmp/dc_in");
					}
                }
			    else {
				/*  I am a USB - Printer . My device class is 0x07  */
				if (dev->config->interface->altsetting->bInterfaceClass == 0x07) {
					printf("Vendor = 0x%04x\t",dev->descriptor.idVendor);
					printf("Product = 0x%04x\n",dev->descriptor.idProduct);
					printf("Cls = 0x%02x\n",dev->config->interface->altsetting->bInterfaceClass);
					
					udev = usb_open(dev);
					if (!udev){
						usb_close(udev);
						return -1;
					}
					else {
						usb_close(udev);
						return 0;
					}
				}
				/*This may be a digital camera style.Class number = 0x06.*/
				#if 0
				else if (dev->config->interface->altsetting->bInterfaceClass == 0x06) {
					printf("Vendor = 0x%04x\t",dev->descriptor.idVendor);
					printf("Product = 0x%04x\n",dev->descriptor.idProduct);
					printf("Cls = 0x%02x\n",dev->config->interface->altsetting->bInterfaceClass);
					
					udev = usb_open(dev);
					if (!udev) {	
						usb_close(udev);
						return -1;
					}
					else {
						usb_close(udev);
						system("up_send_ctl usbExist 1");
						return 0;
					}
				}
				#endif				

				/*If we run here , that means we get the Vandor spec. class . Therefore , we have to check device sequentially.*/
				else if (dev->config->interface->altsetting->bInterfaceClass == 0xff) {
					printf("Vendor = 0x%04x\t",dev->descriptor.idVendor);
					printf("Product = 0x%04x\n",dev->descriptor.idProduct);
					printf("Cls = 0x%02x\n",dev->config->interface->altsetting->bInterfaceClass);
					udev = usb_open(dev);
					if (!udev) {	
						usb_close(udev);
						return -1;
					}
					if (dev->descriptor.iProduct) {
						ret = usb_get_string_simple(udev, dev->descriptor.iProduct, string, sizeof(string));
						if (ret > 0){
							usb_close(udev);
							/* Fill condition : hub */
							retofopen = open_device(dev);
							if (!retofopen)
								return -1;
							else 
								return 0;
							//check_device
						}		
					}
				}
				else{continue ;}
              		    }
			}
		}	
    }
	/* Nothing we can find . -1 as default return value . */
	return -1;
}

int main(int argc, char *argv[])
{
    int ret;
    int pre_dc_check = 0;    
 
    /* if precheck .... */
    if (argc == 2 && !strcmp(argv[1],"-p")) {
        pre_dc_check = 1 ;
    }

    usb_init();
    usb_find_busses();
    usb_find_devices();
    
    ret = check_device(pre_dc_check);
    if(ret == 0){
		printf("This may a printer .\n");
		system("touch /tmp/usb_printer");
		return 0;
	}
	else{
		printf("This may not a printer , please recheck.\n");
		return 0;
	}
}
