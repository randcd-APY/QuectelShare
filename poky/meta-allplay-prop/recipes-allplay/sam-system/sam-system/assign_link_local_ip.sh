#!/bin/sh


if [ $# -ne 1 ]; then
	exit 1
else
	IFNAME="$1"
fi

ip link set up dev "$IFNAME"
if [ -f "/var/run/zcip_$IFNAME.pid" ]; then
	kill -KILL $(cat "/var/run/zcip_$IFNAME.pid")
	rm -f "/var/run/zcip_$IFNAME.pid"
fi

zcip -f "$IFNAME" /lib/netifd/zcip.script  >&/dev/null &
echo $! >"/var/run/zcip_$IFNAME.pid"
