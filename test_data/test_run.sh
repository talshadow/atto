#/bin/bash

PATH_TO_SENDER=${HOME}/build/atto/Debug/senders

${PATH_TO_SENDER}/senders -f output_data1.json -p 40960 &
${PATH_TO_SENDER}/senders -f output_data2.json -p 40961 &
${PATH_TO_SENDER}/senders -f output_data3.json -p 40960 &
${PATH_TO_SENDER}/senders -f output_data4.json -p 40961 &
#${PATH_TO_SENDER}/senders -f output_data5.json -p 40960 &
#${PATH_TO_SENDER}/senders -f output_data6.json -p 40961 &
#${PATH_TO_SENDER}/senders -f output_data7.json -p 40960 &
#${PATH_TO_SENDER}/senders -f output_data8.json -p 40961 &
