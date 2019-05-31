Sample Apps
===========

This folder contains programs written in C language. These are a few sample apps and
re-usable utilities to develop the DSRC driver/application. This is to exemplify
the support in the Qualcomm's wlan Host driver.

There are four sample applications to demonstrate the configuration, sending
and receiving packets on 802.11p channels. See the [BUILDING] section for how to
build the applications from sources.

Simple RX and TX applications:
It encapsulates a packet with a 802.3 custom header before transmitting.
The receiver then deencapsulates this packet and verifies the contents.
Note that we use sendmsg() and recvmsg() for sending/receiving packets.

BUILDING
========

The code in ths folder has been verified with `gcc` cross compiled to different
linux variants. Identify the linux run-time needed for your target device. Make
sure the compiler is in your path.

Use `make` to compile the applications.

- make will look at the CROSS_COMPILE environment variable to determine which
compiler to use. If CROSS_COMPILE is already set:
    make all

- To cross compile for a specific target:
    CROSS_COMPILE=arm-oe-linux-gnueabi- make all

- or
    CROSS_COMPILE=arm-linux-gnueabihf- make all

- To cross compile using native `gcc`:
    make all

- Clean the corresponding intermediate and final objects:
    [CROSS_COMPILE=<variant>] make clean

DSRC additional control commands are implemented using nl80211 sockets. These
require the nl80211 library to be installed in the host and target. In order to
build the `dsrc_config` app, add a configuration variable according to the library
version that is available in the target:

- For libnl-2:


    HAVE_LIBNL=1 CROSS_COMPILE=<variant> make dsrc_config

- For libnl-3:


    HAVE_LIBNL3=1 CROSS_COMPILE=<variant> make dsrc_config

- Follow these commands to install libnl-3 on the build machine


    sudo apt-get install bison flex
    wget http://www.infradead.org/~tgr/libnl/files/libnl-3.2.25.tar.gz
    tar -xzf libnl-3.2.25.tar.gz
    cd libnl-3.2.25
    ./configure --host=arm-linux-gnueabihf --prefix=/usr/arm-linux-gnueabihf
    make
    sudo make install
    cd include/
    sudo make install

USING APPLICATIONS
==================

First configure the channels for OCB communications using `dsrc_set_chan` app.
Channels are expressed with center frequency and optionally with bandwidth.
Default channel bandwidth if not provided is the maximum bandwidth allowed by
that channel (10 MHz for most channels in the 5.9 GHz band).

__Example__ 5860 MHz corresponds to Channel 172.

Once channels have been setup use the `dsrc_tx` app to transmit and `dsrc_rx`
app to recieve.


CONFIGURE CHANNEL
=================

Usage
-----

    dsrc_set_chan [-i <interface>] [-c <channel frequency>] [-c <channel options>]

    Defaults: interface wlanocb0, channel 5860
    -c <channel options> :
        num_channels={1|2} :
           when 1 channel_freq0 and channel_bandwidth0 are needed.
           when 2 same as 1, additionally freq1 and bandwith1 are needed.
        channel_freq0=<center frequency in MHz>
        channel_bandwidth0=<bandwidth in MHz>
        tx_power0=<an integer value, max transmit power in 0.5dbm step size. For example value 6 results in 3dbm>
        channel_freq1=<center frequency in MHz>
        channel_bandwidth1=<bandwidth in MHz>
        tx_power1=<an integer value, max transmit power on channel_freq1, in 0.5dmb step size>

__Note__ `num_channels=2` will mean the alternate channel switching. Channel switching
is only supported on devices that are capable of feeding the Pulse Per Second
signal to WLAN processor. Please check with your platform configuration whether
this support exists.

Example
-------

- To set a channel 5900 on wlanocb0, with 10MHz (default) bandwidth
    dsrc_set_chan -c 5900

- To set a channel 5900 on wlanocb0, with 20MHz bandwidth and max tx power of 17dbm
    dsrc_set_chan -c num_channels=1,channel_freq0=5900,channel_bandwidth0=20,tx_power0=34

- To set 2 channels alternating
    dsrc_set_chan -c num_channels=2,channel_freq0=5860,channel_bandwidth0=10,tx_power0=34,channel_freq1=5900,channel_bandwidth1=20,tx_power1=44


TRANSMIT APP
============
Usage
-----

    dsrc_tx [-i <interface>] [-c <channel freq>] [-d]
       [-h] [-n] [-p <power>] [-m <MCS index>]
       [-r <retry limit>] [-t <TSF expire high>]
       [-s <TSF expire low>] [-a <chain mask>]
       [-o <traffic ID>] [-x <repeat interval msec>]
       [-p <tx power>] [-k <number of packets>]
       [-l <payload length>]
    Defaults: interface wlanocb0, channel 5860, payload length 16 bytes, no dump
    -h : this message
    -d : dump TX packet
    -n : no TX control header
    -m [MCS index] : (data rates @ 20 MHz BW
        0: 6 MBPS
        1: 9 MBPS
        2: 12 MBPS
        3: 18 MBPS
        4: 24 MBPS
        5: 36 MBPS
        6: 48 MBPS
        7: 54 MBPS
    -a : chain mask (antenna)
    -o : Traffic ID (priority 0-15)
    -x : repeat with the interval in milliseconds
    -p : tx power at 0.5dbm step. (eg : value 10 is 5dbm)
    -k : number of packets
    -l : payload length in bytes

Example
-------
To transmit 10 packets of 500 bytes each 100 milliseconds apart, at 2.5dbm transmit power
at 6mbps data rate (assume 10Mhz channel bandwidth set by dsrc_set_chan)

    dsrc_tx -m 2 -p 5 -k 10 -l 500 -x 100

If two channels have been configured to alternate according to a schedule, the
-c option would be used to specify which channel to transmit on. Note that -c
cannot be used to transmit on a channel that has not been configured. For example,
after configuring channels 5860 and 5900 without `dsrc_set_chan` or `dsrc_config`,
this command can be used to transmit a packet on channel 5860 at 5 dbm

    dsrc_tx -c 5860 -p 10

If the device has been configured for two channels but the -c option is not provided,
the packet may be transmitted on either channel.

RECEIVE APP
===========
Usage
-----

    dsrc_rx [-i <interface>] [-d]
    Defaults: interface wlanocb0, no dump
    -d : dump raw packet
    -i <interface> : interface name as found in listing by ifconfig


CONFIGURATION APP
=================
Usage
-----

    dsrc_config [-i <interface>] -c <command> <additional parameters>
    Defaults: interface wlanocb0
    -i <interface> : interface name as found in listing by ifconfig
    -c <command> : command name as specified in the list of commands

Each command will have a separate list of additional parameters.

List of commands:

### 1. set_config ###
Description: Sets the complete OCB configuration including DCC paramters.
The DCC parameters will be read from the dcc.dat file which should be in the
same folder as the dsrc_config executable.

    Usage: dsrc_config -c set_config [-i <interface>] [-o <channel options>]
    -o <channel options> :
        num_channels={1|2} :
           when 1 channel_freq0 and channel_bandwidth0 are needed.
           when 2 same as 1, additionally freq1 and bandwith1 are needed.
        channel_freq0=<center frequency in MHz>
        channel_bandwidth0=<bandwidth in MHz>
        tx_power0=<max transmit power in dbm>
        duration0=<channel schedule duration in ms>
        guard0=<schedule start guard interval in ms>
        disable_rx_stats0={0|1} :
            when 0 (default) RX stats headers are added.
            when 1 RX stats headers are not added.
        channel_freq1=<center frequency in MHz>
        channel_bandwidth1=<bandwidth in MHz>
        tx_power1=<max transmit power on channel_freq1>
        duration1=<channel schedule duration in ms>
        guard1=<schedule start guard interval in ms>
        disable_rx_stats1={0|1} :
            when 0 (default) RX stats headers are added.
            when 1 RX stats headers are not added.
        expiry_tsf={0|1} :
            when 0 (default) expiry time in TX options is relative.
            when 1 expiry time in TX options is interpreted as a TSF time.
    -d : enable DCC parameters if this option is present

For example, to configure channel 5860

    dsrc_config -c set_config -o channel_freq0=5860

To configure channel 5875 with a 20 MHz bandwidth

    dsrc_config -c set_config -o channel_freq0=5875,channel_bandwidth0=20

Note, there must not be any spaces between the suboptions provided for -o.

To configure channels 5860 and 5900 to alternate with 100 ms on each channel with a
15 ms guard interval

    dsrc_config -c set_config -o num_channels=2,channel_freq0=5860,duration0=100,guard0=15,channel_freq1=5900,duration1=100,guard1=15

Note, the guard interval is included in the duration.

### 2. ndl_update ###
Description: Updates the DCC NDL parameters. The DCC parameters will be read
from the dcc.dat file which should be in the same folder as the dsrc_config
executable.

    Usage: dsrc_config -c ndl_update

### 3. get_stats ###
Description: Reads the DCC statistics and displays them.

    Usage: dsrc_config -c get_stats

### 4. stats_event ###
Description: Listen for DCC statistics events. The WLAN driver will send
periodical updates of the DCC statistics and they will get displayed in the
terminal.

    Usage: dsrc_config -c stats_event -o <channel options>
    -o <channel options> :
        num_channels={1|2} :
        channel_freq0=<center frequency in MHz>
        channel_freq1=<center frequency in MHz>

Note, there must not be any spaces between the suboptions provided for -o.

### 5. clear_stats ###
Description: Resets the DCC statistics to their initial state. This will clear
all the DCC statistics.

    Usage: dsrc_config -c clear_stats

### 6. set_utc ###
Description: Sends the system time in 802.11p defined UTC format to the WLAN
host driver. UTC time in this case is defined as the number of nanoseconds from
Jan 1st 1958.

    Usage: dsrc_config -c set_utc

### 7. start_ta ###
Description: Start sending timing advertisement frames on a specified channel
and specified repeat rate. The repeat rate is defined as the number of times the
TA frame will be sent during a 5 seconds interval. The set_utc command must
be executed before this command.

    Usage: dsrc_config -c start_ta -o channel_freq0=<channel freq> -r <repeat rate>

For example, to configure the device to transmit the timing advertisement frame
once a second on channel 5860

    dsrc_config -c set_config -o channel_freq0=5860
    dsrc_config -c set_utc
    dsrc_config -c start_ta -o channel_freq0=5860 -r 5

### 8. stop_ta ###
Description: Stop sending timing advertisement frames on a specified channel.

    Usage: dsrc_config -c stop_ta -o channel_freq0=<channel freq>

### 9. get_tsf ###
Description: Gets the Timing Synchronization Function timer value and displays
it as a 64-bit integer.

    Usage: dsrc_config -c get_tsf

EXAMPLE USE CASES
=================

Multi-channel operation using 3 devices (DUTs)
==============================================

1pps signal must be present at GPIO1

TX multi-channel, RX single channel
-----------------------------------

DUT 1: TX multi-channel, chan 5870 and 5900

    ./dsrc_set_chan -c num_channels=2,channel_freq0=5870,channel_bandwidth0=10,channel_freq1=5900,channel_bandwidth1=10
    ./dsrc_tx -c 5870 -p 1 -m 0 -a 1 -x 5000& ./dsrc_tx -c 5900 -p 1 -m 0 -a 1 -x 5000

DUT 2: RX single channel, chan 5870

    ./dsrc_set_chan -c 5870
    ./dsrc_rx

DUT 3: RX single channel, chan 5900

    ./dsrc_set_chan -c 5900
    ./dsrc_rx

TX multi-channel, RX multi-channel
----------------------------------

DUT 1: TX multi-channel, chan 5870 and 5900

    ./dsrc_set_chan -c num_channels=2,channel_freq0=5870,channel_bandwidth0=10,channel_freq1=5900,channel_bandwidth1=10
    ./dsrc_tx -c 5870 -p 1 -m 0 -a 1 -x 2000& ./dsrc_tx -c 5900 -p 1 -m 0 -a 1 -x 2000

DUT 2: RX multi-channel, chan 5870 and 5900

    ./dsrc_set_chan -c num_channels=2,channel_freq0=5870,channel_bandwidth0=10,channel_freq1=5900,channel_bandwidth1=10
    ./dsrc_rx

DUT 3: RX multi-channel, chan 5870 and 5900

    ./dsrc_set_chan -c num_channels=2,channel_freq0=5870,channel_bandwidth0=10,channel_freq1=5900,channel_bandwidth1=10
    ./dsrc_rx


Timing Advertisement
====================

DUT 1: TX

Connect 1pps

    ./dsrc_set_chan -c 5870
    ./dsrc_config -c set_utc
    ./dsrc_config -c start_ta -r 3 -o channel_freq0=5870
    ./dsrc_config -c stop_ta

DUT 2: RX

    ./dsrc_set_chan -c 5870

Channel Load
============
Channel load is the percentage of time that the channel is in use, in steps
of 0.1%.

DUT 1: TX

    ./dsrc_config -o num_channels=1,channel_freq0=5860
    ./dsrc_tx -p 10 -m 0 -x 0 -l 1400

DUT 2: RX

    ./dsrc_config -o num_channels=1,channel_freq0=5860 -d
    ./dsrc_config -c get_stats

Observe channel load in get_stats output
e.g. DCC NDL channel load = 980
