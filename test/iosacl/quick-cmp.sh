#!/bin/sh


DIFFCMD="diff -C 5 -c -b -B -w -I \"Generated\" -I 'Activating ' -I 'Firewall Builder  fwb_iosacl v' -I 'Can not find file' -I '====' -I 'log '"

for f in $(ls *.fw.orig)
do
    V="$f   <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
    echo "echo \"$V\" | cut -c1-72"
    new_f=$(echo $f | sed 's/.orig//')
    echo "$DIFFCMD $f $new_f"
done
exit 0

run_diffs_for_file() {
xmlfile=$1
folder=$2
fwbedit list -f $xmlfile -o $folder -c -F%name% | sort | while read fwobj; do
  V="$fwobj   <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
  echo "echo \"$V\" | cut -c1-72"
  echo "$DIFFCMD ${fwobj}.fw.orig ${fwobj}.fw"
done
}


run_diffs_for_file objects-for-regression-tests.fwb /User/Firewalls
# run_diffs_for_file cluster-tests.fwb /User/Clusters

