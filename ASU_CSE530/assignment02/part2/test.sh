#!/bin/sh -x
echo -n "6" > /sys/class/HCSR/HCSRdevice1/trigger
echo -n "15" > /sys/class/HCSR/HCSRdevice1/echo
echo -n "1" > /sys/class/HCSR/HCSRdevice1/mode
echo -n "3" > /sys/class/HCSR/HCSRdevice1/frequency
echo -n "1" > /sys/class/HCSR/HCSRdevice1/enable
sleep 1
echo -n "0" > /sys/class/HCSR/HCSRdevice1/enable
echo -n "1" > /sys/class/HCSR/HCSRdevice1/mode
echo -n "1" > /sys/class/HCSR/HCSRdevice1/enable

echo -n "10" > /sys/class/HCSR/HCSRdevice2/trigger
echo -n "1" > /sys/class/HCSR/HCSRdevice2/echo
echo -n "1" > /sys/class/HCSR/HCSRdevice2/mode
echo -n "5" > /sys/class/HCSR/HCSRdevice2/frequency
echo -n "1" > /sys/class/HCSR/HCSRdevice2/enable

sleep 2
echo -n "0" > /sys/class/HCSR/HCSRdevice1/enable
echo -n "0" > /sys/class/HCSR/HCSRdevice2/enable
