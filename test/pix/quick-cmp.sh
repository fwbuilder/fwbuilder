#!/bin/sh


DIFFCMD="diff -C 5 -c -b -B -w -I \"#  Generated\" -I 'Activating ' -I '#  Firewall Builder  fwb_pix v' -I 'Can not find file' -I '====' -I 'log '"

for f in $(ls *.fw.orig)
do
    V="$f   <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
    echo "echo \"$V\" | cut -c1-72"
    new_f=$(echo $f | sed 's/.org//')
    echo "$DIFFCMD $f $new_f"
done
