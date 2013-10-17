/zip.sh
sleep 2

URI=/upload
HOST=data.visitero.com
RESULT_FILE=/tmp/post_result.post
SEND_FILE=/tmp/post_result.send

CORRECT="NO"
while [ "$CORRECT" == "NO" ]; do
  CORRECT="YES"
  RANDOM=
  while [ "$RANDOM" == "" ]; do
    RANDOM=random_separator_$(head -c 30 /dev/urandom | tr -dc 'a-zA-Z0-9')
  done

  for i in $( ls /result/*.gz); do
    if [ ! "$( cat $i | grep $RANDOM )" == "" ]; then
      CORRECT="NO"
      break
    fi
  done
done

#add MAC addr
echo -e "--$RANDOM\r">>$RESULT_FILE
echo -e "Content-Disposition: form-data; name=\"MAC\"\r" >> $RESULT_FILE
echo -e "\r" >> $RESULT_FILE
#cat /sys/class/net/mon.wlan0/address >> $RESULT_FILE
cat /sys/class/net/eth1/address >> $RESULT_FILE
echo -e "\r" >> $RESULT_FILE

#add Version info
echo -e "--$RANDOM\r">>$RESULT_FILE
echo -e "Content-Disposition: form-date; name=\"sys_version\"\r" >> $RESULT_FILE
echo -e "\r" >> $RESULT_FILE
cat /proc/version >> $RESULT_FILE
echo -e "\r" >> $RESULT_FILE

#add ifconfig result
echo -e "--$RANDOM\r">>$RESULT_FILE
echo -e "Content-Disposition: form-data; name=\"ifconfig_result\"\r" >> $RESULT_FILE
echo -e "\r" >> $RESULT_FILE
ifconfig >> $RESULT_FILE
echo -e "\r" >> $RESULT_FILE

#add df result
echo -e "--$RANDOM\r">>$RESULT_FILE
echo -e "Content-Disposition: form-data; name=\"df_result\"\r" >> $RESULT_FILE
echo -e "\r" >>$RESULT_FILE
df -h >> $RESULT_FILE
echo -e "\r" >> $RESULT_FILE

n=1
for f in $( ls /result/*.gz); do
  echo -e "--$RANDOM\r" >> $RESULT_FILE
  echo -e "Content-Disposition: form-data; name=\"dump_file_$n\"; filename=\"$f\"\r" >> $RESULT_FILE
  echo -e "Content-Type: application/octet-stream\r" >> $RESULT_FILE
  echo -e "Content-Transfer-Encoding: binary\r" >> $RESULT_FILE
  echo -e "\r" >> $RESULT_FILE
  cat $f >> $RESULT_FILE
  echo -e "\r" >> $RESULT_FILE
  n=$(expr $n + 1)
done
echo -e "--$RANDOM--\r" >>$RESULT_FILE

SIZE=$( ls -l $RESULT_FILE | awk '{ print $5}' )

echo -e "POST $URI HTTP/1.1\r" > $SEND_FILE
echo -e "Host: $HOST\r" >> $SEND_FILE
echo -e "Content-Type: multipart/form-data; boundary=$RANDOM\r" >> $SEND_FILE
echo -e "Content-Length: $SIZE\r" >> $SEND_FILE
echo -e "\r" >> $SEND_FILE
cat $RESULT_FILE >> $SEND_FILE

rm $RESULT_FILE

SEND_RESULT=$( /netcat $HOST 80 < $SEND_FILE | grep "200 OK" )

if [ ! "$SEND_RESULT" == "" ]; then
  rm /result/*.gz
fi

rm $SEND_FILE
