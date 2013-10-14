#!/bin/ash

if [ -f /report.time ]; then
  mv /report.time /result/report.time.$(tail -n 1 /report.time).dump
fi
