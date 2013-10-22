#!/bin/sh
RESULT="FALSE"
while [ $RESULT == "FALSE" ]; do
  iw phy phy0 interface add mon.wlan0 type monitor
  VALUE=$( ifconfig mon.wlan0 up | grep "No such device" )
  if [ "$VALUE" == "" ]; then
    RESULT="TRUE"
  else
    echo -e "[$(date)]Can't create or up mon.wlan0 interface." >> boot.errors
    sleep 60
  fi
  sleep 1
done
