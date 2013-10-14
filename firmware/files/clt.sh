#!/bin/ash
tcpdump -n -e -tt -vvv -i mon.wlan0 type mgt subtype probe-req 2>>/tcpdump.errors | /processing.exe 2>>/pprocessing.errors
