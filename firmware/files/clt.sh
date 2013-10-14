#!/bin/ash
tcpdump -n -e -tt -vvv -i mon.wlan0 type mgt subtype probe-req 2>/errors | /processing.exe
