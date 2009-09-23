#!/bin/sh


XMLFILE=$1
DIFFCMD="diff -C 5 -c -b -B -w -I \"#  Generated\" -I 'Activating ' -I '#  Firewall Builder  fwb_ipt v' -I 'Can not find file' -I '====' -I 'log '"

fwbedit list -f $XMLFILE -o /User/Firewalls -c -F%name% | sort | while read fwobj; do
  V="$fwobj   <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
  echo "echo \"$V\" | cut -c1-72"
  echo "$DIFFCMD ${fwobj}.fw.orig ${fwobj}.fw"
done

