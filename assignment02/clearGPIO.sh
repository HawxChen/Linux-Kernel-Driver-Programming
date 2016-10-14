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

#IO4 Trigger Pin
echo -n "0" > /sys/class/gpio/export || echo "gpio0 alreay exported"  
echo -n "18" > /sys/class/gpio/export || echo "gpio18 already exported"  
echo -n "66" > /sys/class/gpio/export || echo "gpio66 already exported"  

echo -n "out" > /sys/class/gpio/gpio0/direction || "Failed to set gpio0 out"  
echo -n "out" > /sys/class/gpio/gpio18/direction || "Failed to set gpio18 out"  
echo -n "out" > /sys/class/gpio/gpio66/direction || "Failed to set gpio66 out"  

echo -n "0" > /sys/class/gpio/gpio0/value || "Failed to set gpio0 value 0"
echo -n "0" > /sys/class/gpio/gpio18/value || "Failed to set gpio18 value 0"
echo -n "0" > /sys/class/gpio/gpio66/value || "Failed to set gpio66 value 0"

echo -n "6" > /sys/class/gpio/unexport || echo "gpio0 alreay unexported"  
echo -n "18" > /sys/class/gpio/unexport || echo "gpio18 already unexported"  
echo -n "66" > /sys/class/gpio/unexport || echo "gpio66 already unexported"  

#IO12 Echo Pin
echo -n "1" > /sys/class/gpio/export || echo "gpio1 alreay exported"  
echo -n "20" > /sys/class/gpio/export || echo "gpio20 already exported"  
echo -n "41" > /sys/class/gpio/export || echo "gpio21 already exported"  
echo -n "68" > /sys/class/gpio/export || echo "gpio68 already exported"  

echo -n "out" > /sys/class/gpio/gpio1/direction || "Failed to set gpio1 out"  
echo -n "out" > /sys/class/gpio/gpio20/direction || "Failed to set gpio20 out"  
echo -n "out" > /sys/class/gpio/gpio21/direction || "Failed to set gpio21 out"  
echo -n "out" > /sys/class/gpio/gpio68/direction || "Failed to set gpio68 out"  

echo -n "0" > /sys/class/gpio/gpio1/value || "Failed to set gpio1 value 0"
echo -n "0" > /sys/class/gpio/gpio20/value || "Failed to set gpio20 value 0"
echo -n "0" > /sys/class/gpio/gpio21/value || "Failed to set gpio21 value 0"
echo -n "0" > /sys/class/gpio/gpio68/value || "Failed to set gpio68 value 0"

echo -n "1" > /sys/class/gpio/unexport || echo "gpio1 alreay unexported"  
echo -n "20" > /sys/class/gpio/unexport || echo "gpio20 already unexported"  
echo -n "21" > /sys/class/gpio/unexport || echo "gpio21 already unexported"  
echo -n "68" > /sys/class/gpio/unexport || echo "gpio68 already unexported"  
