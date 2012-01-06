#!/bin/bash

if [ "`uname -s`" = "Linux" ]; then
	mydev=`sudo losetup -f` # Get the name of the next free loop device
	sudo losetup $mydev build/floppy.img # Add a loop device from our disk image file
	mpoint='/mnt/osdev'
	mcmd='mount -t ext2'
	mcmd2=''
else
	if [ "`uname -s`" = "Darwin" ]; then
		mydev='build/floppy.img'
		mpoint='/Volumes/osdev'
		mcmd='fuse-ext2'
		mcmd2='-o force'
	fi
fi

sudo mkdir $mpoint
sudo $mcmd $mydev $mpoint $mcmd2 # Mount the loop device to /mnt/osdev
echo "   Copying"
sudo cp kernel/kernel $mpoint # Copy the kernel image
sudo cp init/init.elf $mpoint # Copy init.elf
sudo cp drivers/drivers.tar $mpoint # Copy drivers
echo "   Copied"
sudo umount -f $mpoint # Unmount disk image
sudo rm -r $mpoint
if [ "`uname -s`" = "Linux" ]; then
	sudo losetup -d $mydev # Free loop device
fi
echo "  Unmounted"
