#!/bin/sh

if [ $(cat /etc/debian_version) != "7.4" ]; then
    echo "Only tested with version 7.4. It may not work with this version"
fi

# Load the device tree binaries to enable the PWM subsystem
echo am33xx_pwm > /sys/devices/bone_capemgr.?/slots

# Configure pin P8_13 as pwm
echo bone_pwm_P8_13 > /sys/devices/bone_capemgr.?/slots

# Remove the (unnecessary) pwm_test module, which otherwises binds
# to pwm6. A better solution would be to modify the device tree binaries,
# but this works OK for now.
sleep 1
rmmod pwm_test

echo "Exporting pwm6"
echo 6 > /sys/class/pwm/export
echo 1000000 > /sys/class/pwm/pwm6/period_ns
echo 0 > /sys/class/pwm/pwm6/duty_ns
echo 1 > /sys/class/pwm/pwm6/run
