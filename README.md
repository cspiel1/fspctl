fspctl README
=============

A modbus/RTU controller and daemon for the FSP Powermanager 10kW. Connect via a
USB to RS-485 converter to the FSP Modbus Card P17.

The FSP Modbus Card P17 has DIP switches to configure the RS485 address and
other communication parameters.

Configuration
-------------

On first start fspctl writes a configuration template to ~/.config/.fspctl

```
ttydev     /dev/ttyUSB0
stopbits   2
slaveid    10
mqtthost   mqtt.host.org
mqttport   1883
mqttuser   user
mqttpass   ***
capath     The CA path to verify TLS certificate of the MQTT broker.
cafile     The CA file to verify TLS certificate of the MQTT broker.
publish0   address bit mqtt-topic
publish1   address bit mqtt-topic
...
publish9   address bit mqtt-topic
```

Requirements
-------------
E.g. on Debian/Ubuntu:
```
sudo apt install cmake libmodbus-dev libmosquitto-dev libssl-dev
```

Building
--------
```
mkdir build
cd build
cmake ..
make
```

Cross Compile for ARM
---------------------

- Download toolchain for ARM architecture from
https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
- Install toolchain
```
tar -xf arm-gnu-toolchain-*-x86_64-arm-none-linux-gnueabihf.tar.xz
# move it to your preferred location and create a symlink,
# e.g. ~/dev/arm-toolchain
```
- scp the sysroot to your build machine
```
rsync -ac root@fspctl:/{lib,usr} ~/dev/arm-sysroot
```
- Edit arm.cmake to point to the toolchain and sysroot

- Cross compile fspctl
```
./arm.sh
```
