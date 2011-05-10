ifconfig rteth0 down
/usr/local/rtnet/sbin/rtifconfig rteth0 down
sleep 3
rmmod rtcap.ko
rmmod rtudp.ko
rmmod rtipv4.ko
rmmod rtpacket.ko
rmmod rt_8139too.ko
rmmod rtnet.ko
