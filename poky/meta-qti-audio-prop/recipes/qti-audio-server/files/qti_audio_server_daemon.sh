#! Copyright (c) 2017 Qualcomm Technologies, Inc.
#! All Rights Reserved.
#! Confidential and Proprietary - Qualcomm Technologies, Inc.

#! /bin/sh
set -e

case "$1" in
  start)
        echo -n "starting qti_audio_server_deamon: "
        start-stop-daemon -S -b -a /usr/bin/qti_audio_server_daemon

        echo "done"
        ;;
  stop)
        echo -n "stopping qti_audio_server_daemon: "
        start-stop-daemon -K -n qti_audio_server_daemon
        echo "done"
        ;;
  restart)
        $0 stop
        $0 start
        ;;
  *)
        echo "Usage qti_audio_server_daemon { start | stop | restart}" >&2
        exit 1
        ;;
esac

exit 0
