#!/bin/ash
echo some > /tmp/force.dump
#while [ -f /tmp/force.dump ]; do
sleep 60
#done

/copy_reports.sh

gzip /result/*.dump

