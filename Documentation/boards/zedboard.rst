Avnet Zedboard
==============

Barebox replaces both the FSBL and the u-boot build from Xilinx. To configure it
use ``zedboard_defconfig``.

SD card
^^^^^^^

Gerät      Boot  Anfang    Ende Sektoren Größe Kn Typ
/dev/sda1          2048  104447   102400   50M  b W95 FAT32
/dev/sda2        104448  923647   819200  400M 83 Linux
/dev/sda3        923648 1742847   819200  400M 83 Linux
/dev/sda4       1742848 3176447  1433600  700M 83 Linux

while /dev/sda1 (FAT32) is used as boot partition. /dev/sda2 (ext4) is used for the userland.
/dev/sda3 (ext4) is used for data storage and /dev/sda4 (reserved)

The Boot ROM searches for a file boot.bin on the first partition of the DS card.
This partition has to be FAT32.

.. code-block:: sh

  make zedboard_defconfig
  make

.. code-block:: sh

  cp images/barebox-avnet-zedboard.img /mnt/DF32-32A4/BOOT.bin
  umount /mnt

