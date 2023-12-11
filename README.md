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
Debian/RaspberryPiOS: 
```
sudo apt install cmake libmodbus-dev libmosquitto-dev libssl-dev build-essential
```

Building
-------------
```
mkdir build
cd build
cmake ..
make
```
