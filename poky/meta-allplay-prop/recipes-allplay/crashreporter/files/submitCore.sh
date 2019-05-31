#!/bin/sh

# Maximum dump size (in KB)
MAX_SIZE=10240 #10MB

PID=$1
EXE=$(readlink "/proc/${PID}/exe" 2>/dev/null)
MD5=$(md5sum ${EXE} 2>/dev/null | cut -f 1 -d ' ' 2>/dev/null)
MAC=$(grep -o serialno=[0-9a-f]* </proc/cmdline | cut -d '=' -f2)
PROCESS_NAME=$(basename ${EXE})
FILENAME=${PROCESS_NAME}-${MAC}-${MD5}

CRASH_REPORTER_URL="$(jq -ej .system.crashReporterUrl /etc/allplay/oem.json)" \
  && [ -n "${CRASH_REPORTER_URL}" ] \
  || { logger "Crash reports for $PROCESS_NAME disabled (no crash server)"; exit 1; }

VERSION=$(cat /etc/version)

# - compress it
# - get the first 10MB
# - send it to the crash server
TMP_DUMP="/tmp/dump.gz"
gzip -9 | dd bs=1024 count=$MAX_SIZE > ${TMP_DUMP}  2>/dev/null

# Compress /var/log
TMP_VAR_LOG="/tmp/var_log.tgz"
journalctl > /var/log/journal.log
tar -czf ${TMP_VAR_LOG} /var/log

curl -NsS \
     -F "version=$VERSION" \
     -F "crash=@${TMP_DUMP};filename=${FILENAME}.gz" \
     -F "var_log=@${TMP_VAR_LOG};filename=${FILENAME}-var_log.tar.gz" \
     --connect-timeout 60 --max-time 600 \
     "${CRASH_REPORTER_URL}" 2>&1 | logger

rm -f "${TMP_DUMP}" "${TMP_VAR_LOG}"

