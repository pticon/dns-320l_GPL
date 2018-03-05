#!/bin/sh

#!/bin/sh

#echo "cp $PROJECT_NAME drivers"
driver_path=${ROOTDIR}/module/crfs/${PROJECT_NAME}/driver
echo "copy path=" $driver_path
if [ ! -d $driver_path ];then
	echo "can not find $driver_path folder"
	exit 1
fi

#sudo cp drivers/scsi/sd_mod.ko $driver_path
#sudo cp drivers/usb/storage/usb-storage.ko $driver_path
#sudo cp drivers/usb/class/usblp.ko $driver_path
#sudo cp fs/fat/fat.ko $driver_path
#sudo cp fs/vfat/vfat.ko $driver_path

# copy drivers for ipv6
cp -avf net/ipv4/tunnel4.ko              $driver_path
cp -avf net/ipv6/ipv6.ko                 $driver_path
cp -avf net/ipv6/sit.ko                  $driver_path
cp -avf net/ipv6/xfrm6_mode_beet.ko      $driver_path
cp -avf net/ipv6/xfrm6_mode_transport.ko $driver_path
cp -avf net/ipv6/xfrm6_mode_tunnel.ko    $driver_path
cp -avf net/ipv4/ipip.ko 				$driver_path
cp -avf net/ipv6/tunnel6.ko				$driver_path
cp -avf net/ipv6/ip6_tunnel.ko			$driver_path
cp -avf drivers/net/bonding/bonding.ko   $driver_path
