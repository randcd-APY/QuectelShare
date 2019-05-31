#!/bin/sh

# Usage: enable_mesh.sh [ssid key [channel]]
# If no arguments are passed the mesh is disabled
# If the channel is not passed it is assumed that the
# wlan channel is being shared.


MESH_INTF="allplay_mesh"
MESH_CONFIG_FILE="/etc/config/wpa_supplicant-allplay_mesh.conf"
MESH_PID="/var/run/wpa_supplicant-allplay_mesh.pid"
MESH_CTRL="/var/run/wpa_supplicant-allplay_mesh"

ssid=
key=
freq=
macaddr=

create_mesh_interface() {
	echo "creating mesh interface"

	kill_wpa_supplicant

	iw dev "$MESH_INTF" del
	iw dev wlan0 interface add "$MESH_INTF" type ibss

	ip link set "$MESH_INTF" address "$macaddr"
	ip link set "$MESH_INTF" up

	wpa_supplicant -C "$MESH_CTRL" -i "$MESH_INTF" \
			-B -P "$MESH_PID" \
			-c "$MESH_CONFIG_FILE" -Dnl80211,wext
}

kill_wpa_supplicant() {
	if [ -f "$MESH_PID" ] && pidof wpa_supplicant | grep -w -q $(cat "$MESH_PID") ; then
	        kill -9 $(cat "$MESH_PID")
	        rm -rf "$MESH_PID"
	        rm -rf "$MESH_CTRL/$MESH_INTF"
	fi
}

remove_mesh_interface() {
	kill_wpa_supplicant

	iw dev "$MESH_INTF" del
}

create_config_file() {
	cat > $MESH_CONFIG_FILE <<EOF
ctrl_interface=$MESH_CTRL
ap_scan=2
fast_reauth=1

config_methods=virtual_push_button virtual_display push_button keypad

network={
	mode=1
	scan_ssid=1
	ssid="$ssid"
	key_mgmt=WPA-PSK
	proto=RSN
	frequency=$freq
	fixed_freq=1
	psk="$key"
}
EOF

}

bring_up_mesh() {
	create_config_file

	create_mesh_interface
}

if [ $# -eq 0 ]; then
	echo "disabling mesh"
	remove_mesh_interface
	/etc/init.d/babeld stop
	exit 0
elif [ $# -lt 3 ]; then
	echo "usage: $0 [ macaddr ssid key [channel] ]"
	exit 1
fi

macaddr="$1"
ssid="$2"
key="$3"
if [ $# -eq 4 ]; then
	chan="$4"
	# -eq tries to convert the value to a number.
	# If the conversion fails, the operator will return false irrespective of the value.
	if [ "$chan" -eq "$chan" 2>/dev/null ]; then
		if [ $chan -le 14 ]; then
			freq=$((2407+$chan*5))
		else
			freq=$((5000+$chan*5))
		fi
	fi
else
	freq=$(iw dev wlan0 link | awk '/freq:/ { print $2 }')
fi

if [ -z "$freq" ]; then
	exit 1
fi

if ! [ "$freq" -eq "$freq" 2>/dev/null ]; then
	exit 1
fi

bring_up_mesh

# Assign link local IP address
assign_link_local_ip.sh "$MESH_INTF"

