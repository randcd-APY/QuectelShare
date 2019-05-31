#!/bin/bash
# a script to install the binaries to different host devices using
# secure copy. This assumes access to root user id on the targeted
# device.

# Test an IP address for validity:
# Usage:
#      valid_ip IP_ADDRESS
#      if [[ $? -eq 0 ]]; then echo good; else echo bad; fi
#   OR
#      if valid_ip IP_ADDRESS; then echo good; else echo bad; fi
#
function valid_ip()
{
    local  ip=$1
    local  stat=1

    if [[ $ip =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
        OIFS=$IFS
        IFS='.'
        ip=($ip)
        IFS=$OIFS
        [[ ${ip[0]} -le 255 && ${ip[1]} -le 255 \
            && ${ip[2]} -le 255 && ${ip[3]} -le 255 ]]
        stat=$?
    fi
    return $stat
}

function usage()
{
	echo "Usage $0 <ip_address1> [<ip_address2>...]"
	exit
}

if [ "$#" -lt  1 ]; then
	usage
fi

for ip_addr in "$@"; do

	valid_ip $ip_addr

	if [[ $? -eq 1 ]]; then
		echo "Error : Bad Ip address $ip_addr"
		usage
	fi

	declare -a bins=("dsrc_set_chan" "dsrc_rx" "dsrc_tx")

	for i in "${bins[@]}"; do
		set -x
		scp arm-linux-gnueabihf-bin/"$i" root@$ip_addr:~
		set +x
	done
done
