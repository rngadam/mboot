mboot for Lophilo (Tabby)
=========================

mboot is a bootloader located in micro SDcard for booting Linux on Lophilo tabby board.

This program is developed by:

* Shyu Lee `shyul@lophilo.com` (code draft up, experience design.)
* Ricky Ng-Adam `rngadam@lophilo.com` (Linux specialist.)

Copyright (C) 2012 Lophilo

This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

How to use mboot
----------------
1. The microSD card should be larger than 8GByte, and prepared by Lophilo OS card partition tool.
2. The prepared card has two partitions: boot (1GByte, MSDOS FAT), linux (from 7GByte to card size limit, Linux EXT4).
3. Insert the SDcard to PC's card reader, and mount the "boot" partition (on Windows, only "boot" partition is recognized and automatically list in "My Computer").
4. Copy the generated excutable binary "boot.bin" (File name is sensitive) to the "boot" partition.
5. Also copy "boot.ini", "zImage" and "grid.rbf" (File names are all sensitive) to the "boot" partition.
6. DD the OS filesystem to the "linux" partition.
7. Put microSD card to the card holder on XEM, and we are done. 

Template of kcmd.txt
--------------------
	
	noinitrd mem=128M console=ttyS0,115200 root=/dev/mmcblk0p2 rootfstype=ext4 rw rootwait
	
	
Facts about mboot
-----------------
1. Only for booting Linux on Lophilo's hardware platform.
2. Running directly from micro SDcard, no need to program any special flash on hardware.
3. Initially developed with Keil MDK, then the project was converted for developing with gcc.
4. Final excutable binary size should be limited under 55kByte to fit into SAM9M10's IRAM.
5. Some code is based on ATMEL's at91lib.
