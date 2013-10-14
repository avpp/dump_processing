#!/bin/ash

REPORT_FILE_NAME=/report.time
LAST_TIME_FILE=/last.time


echo -e "$(date +%s)" >> $REPORT_FILE_NAME
if [ "$1" == "REBOOT" ]; then
  echo -e "r" >> $REPORT_FILE_NAME
else
  echo -e "m" >> $REPORT_FILE_NAME
fi

ntpd -nq -p 0.openwrt.pool.ntp.org -p 1.openwrt.pool.ntp.org -p 2.openwrt.pool.ntp.org -p 3.openwrt.pool.ntp.org
if [ "$?" == "0" ]; then
  echo -e "n" >> $REPORT_FILE_NAME
  /save_time.sh
else
  TIME=@0
  if [ -f $LAST_TIME_FILE ]
  then
    TIME=$(cat $LAST_TIME_FILE)
  fi
  date -s "$TIME"
  echo -e "f" >> $REPORT_FILE_NAME
fi

echo -e "$(date +%s)" >> $REPORT_FILE_NAME
