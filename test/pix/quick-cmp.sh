#!/usr/bin/perl

$XMLFILE=@ARGV[0];

$DIFFCMD="diff -C 1 -c -b -B -I \"!  Generated\" -I 'Activating ' -I '!  Firewall Builder  fwb_pix v' -I 'Can not find file'";

#$DIFFCMD="diff -u -b -B -I \"!  Generated\" ";

while (<>) {
  $str=$_;
  while ( $str=~ /<Firewall / ) {
    $str=~ /<Firewall [^>]+name="([^"]*).*$"/;
    $fw=$1;
    printf "$DIFFCMD %s.fw.orig %s.fw\n",$fw,$fw;
    $str=~ s/^.*<Firewall [^>]+name="$fw"[^>]+>//;
  }
}
