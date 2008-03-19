#!/usr/bin/perl

$XMLFILE=@ARGV[0];

$DIFFCMD="diff -0 -u -b -B -I \"!  Generated\" ";

while (<>) {
  $str=$_;
  while ( $str=~ /<Firewall / ) {
    $str=~ /<Firewall [^>]+name="([^"]*).*$"/;
    $fw=$1;
    printf "$DIFFCMD %s.fw.orig %s.fw\n",$fw,$fw;
    $str=~ s/^.*<Firewall [^>]+name="$fw"[^>]+>//;
  }
}
