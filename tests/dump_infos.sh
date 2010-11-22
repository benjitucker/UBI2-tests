for i in /sys/class/ubi/ubi0/* ; do echo -n $i " : " && cat $i ; done
for i in /sys/class/ubi/ubi0/ubi0_0/* ; do echo -n $i " : " && cat $i ; done
