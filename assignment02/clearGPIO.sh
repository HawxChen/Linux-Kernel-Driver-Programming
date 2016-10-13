#!/bin/sh
#IO4 Trigger Pin
echo -n "6" > /sys/class/gpio/export || echo "gpio6 alreay exported"  
echo -n "36" > /sys/class/gpio/export || echo "gpio36 already exported"  

echo -n "out" > /sys/class/gpio/gpio6/direction || "Failed to set gpio6 out"  
echo -n "out" > /sys/class/gpio/gpio36/direction || "Failed to set gpio36 out"  

echo -n "0" > /sys/class/gpio/gpio6/value || "Failed to set gpio6 value 0"
echo -n "0" > /sys/class/gpio/gpio36/value || "Failed to set gpio36 value 0"

echo -n "6" > /sys/class/gpio/unexport || echo "gpio6 alreay unexported"  
echo -n "36" > /sys/class/gpio/unexport || echo "gpio36 already unexported"  

#IO12 Echo Pin
echo -n "15" > /sys/class/gpio/export || echo "gpio15 alreay exported"  
echo -n "42" > /sys/class/gpio/export || echo "gpio42 already exported"  
echo -n "43" > /sys/class/gpio/export || echo "gpio43 already exported"  

echo -n "out" > /sys/class/gpio/gpio15/direction || "Failed to set gpio15 out"  
echo -n "out" > /sys/class/gpio/gpio42/direction || "Failed to set gpio42 out"  
echo -n "out" > /sys/class/gpio/gpio43/direction || "Failed to set gpio43 out"  

echo -n "0" > /sys/class/gpio/gpio15/value || "Failed to set gpio15 value 0"
echo -n "0" > /sys/class/gpio/gpio42/value || "Failed to set gpio42 value 0"
echo -n "0" > /sys/class/gpio/gpio43/value || "Failed to set gpio43 value 0"

echo -n "15" > /sys/class/gpio/unexport || echo "gpio15 alreay unexported"  
echo -n "42" > /sys/class/gpio/unexport || echo "gpio42 already unexported"  
echo -n "43" > /sys/class/gpio/unexport || echo "gpio43 already unexported"  
