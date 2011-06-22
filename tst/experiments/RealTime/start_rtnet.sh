modprobe xeno_rtdm
ifconfig eth4 down
ifconfig eth5 down
rmmod skge
rmmod 8139too
rmmod 8139cp
insmod /usr/local/rtnet/modules/rtnet.ko global_rtskbs=100
insmod /usr/local/rtnet/modules/rt_8139too.ko media=100011000,100011000,100011000,11000,10000,10000,10000,10000
insmod /usr/local/rtnet/modules/rtpacket.ko
insmod /usr/local/rtnet/modules/rtipv4.ko
insmod /usr/local/rtnet/modules/rtudp.ko
insmod /usr/local/rtnet/modules/rtcap.ko
sleep 1
/usr/local/rtnet/sbin/rtifconfig rteth0 up `cat my_ip` promisc
#/usr/local/rtnet/sbin/rtifconfig rteth0 up `cat my_ip`
sleep 3
/usr/local/rtnet/sbin/rtroute add `cat the_route` dev rteth0
ifconfig rteth0 up
cat /proc/rtnet/devices
