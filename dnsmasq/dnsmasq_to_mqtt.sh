#!/bin/sh

# Copied from http://jpmens.net/2013/10/21/tracking-dhcp-leases-with-dnsmasq/
# Add the following to dnsmasq.conf
# dhcp-script=/usr/local/bin/dnsmasq_to_mqtt.sh

op="${1:-op}"
mac="${2:-mac}"
ip="${3:-ip}"
hostname="${4}"

tstamp="`date '+%Y-%m-%d %H:%M:%S'`"



topic="network/dhcp/${mac}"
payload="{\"operation\":\"${op}\", \"ip\":\"${ip}\", \"timestamp\":\"${tstamp}\" \"hostname\":\"${hostname}\"}"

mosquitto_pub -h mosquitto.choffee.co.uk -u inhouse -P in_H77se -t "${topic}" -m "${payload}" -r
