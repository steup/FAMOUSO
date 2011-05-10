rmmod xeno_can_peak_pci
modprobe xeno_can_peak_pci
/usr/xenomai/sbin/rtcanconfig rtcan0 -b 250000 up
