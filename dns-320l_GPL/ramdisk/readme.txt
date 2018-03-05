sudo ./mkimage -A arm -O linux -T ramdisk -C gzip -a 0x00e00000 -n Ramdisk -d ramdisk_el.gz uRamdisk
