#!/bin/sh
iw phy phy0 interface add mon.wlan0 type monitor
ifconfig mon.wlan0 up
