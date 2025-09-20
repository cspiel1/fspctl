#!/bin/bash
# subscribe to remote broker

# read settings from .fspctl
while read line; do
  key=$(echo "$line" | awk '{print $1}')
  value=$(echo "$line" | awk '{print $2}')
  export "$key=$value"
done < .fspctl

echo "Subscribing to ${mqtthost}:${mqttport} with user ${mqttuser} on topic pv/inverter/#"

mosquitto_sub -h ${mqtthost} -p ${mqttport} -u ${mqttuser} -P ${mqttpass} --capath /etc/ssl/certs -t "pv/inverter/#" -v
