#!/bin/sh


XMLFILE=$1
DIFFCMD="diff -C 1 -c -b -B -I \"#  Generated\" -I 'Activating ' -I '#  Firewall Builder  fwb_ipt v' -I 'Can not find file' -I '====' -I 'log '"

for f in *.conf
do
  echo "$DIFFCMD ${f}.orig $f"
done

for f in *.fw
do
  echo "$DIFFCMD ${f}.orig $f"
done

#fwbedit list -f $XMLFILE -o /User/Firewalls -c -F%name% | sort | while read fwobj; do
#  echo "$DIFFCMD ${fwobj}.fw.orig ${fwobj}.fw"
#  echo "$DIFFCMD ${fwobj}.conf.orig ${fwobj}.conf"
#done

