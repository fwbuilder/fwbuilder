#!/bin/sh


DIFFCMD="diff -C 5 -c -b -B -w -I \"#  Generated\" -I 'Activating ' -I '#  Firewall Builder  fwb_ipf v' -I 'Can not find file' -I '====' -I 'log '"

for f in $(ls *.fw.orig *.conf.orig)
do
    V="$f   <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
    echo "echo \"$V\" | cut -c1-72"
    new_f=$(echo $f | sed 's/.orig//')
    echo "$DIFFCMD $f $new_f"
done
