#!/usr/bin/perl

$XMLFILE=@ARGV[0];

$DIFFCMD="diff -C 1 -b -B -c -I \"#  Generated\" -I 'Activating '  -I '#  Firewall Builder  fwb_pf v'";


while (<>) {
  $str=$_;
  while ( $str=~ /<Firewall / ) {
    $str=~ /<Firewall [^>]+name="([^"]*).*$"/;
    $fw=$1;
    printf "$DIFFCMD %s.fw.orig %s.fw\n",$fw,$fw;
    printf "$DIFFCMD %s.conf.orig %s.conf\n",$fw,$fw;
    $str=~ s/^.*<Firewall [^>]+name="$fw"[^>]+>//;
  }
}

