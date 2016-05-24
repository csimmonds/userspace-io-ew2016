#!/bin/bash

echo "P9_12 gpio 60) is input, trigger on falling edge"
echo 60 > /sys/class/gpio/export
echo falling > /sys/class/gpio/gpio60/edge

echo "P9_16 (gpio 51) is output"
echo 51 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio51/direction
