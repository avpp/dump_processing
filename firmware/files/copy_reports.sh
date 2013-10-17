#!/bin/ash

if [ -f /report.time ]; then
  mv /report.time /result/report.time.$(tail -n 1 /report.time).dump
fi

if [ -f /*.errors ]; then
  for %f in /*.errors; do
    mv $f /result/$(basename $f).$(date +"%s").dump
  done
fi
