#!/usr/bin/perl
#
#

use strict;

my $file = $ARGV[0];

die "rehash-ids.pl  data_file.fwb\n" if ($file eq "");

my %ids;
my $idCntr = time;

open F, "$file" or die "Could not open file $file for reading";
while (<F>) {
  if ($_ =~ /id=\"([^\"]+)\"/) {
    $ids{$1}=$1;
  }
}
close F;

while ( my ($k,$v) = each %ids ) {
  if (length($k)>20) {
    $ids{$k} = sprintf("id%d", $idCntr);
    $idCntr++;
    printf "ID=%s -> %s\n",$k, $ids{$k};
  }
}

my $newfile = "$file" . ".new";
open F, "$file" or die "Could not open file $file for reading";
open W, ">$newfile" or die "Coule not open file $newfile for writing";

while (<F>) {
  if ($_ =~ /id=\"([^\"]+)\"/) {
    my $oldid=$1;
    my $newid=$ids{$oldid};
    $_ =~ s/id=\"[^\"]+\"/id=\"$newid\"/;
  }

  if ($_ =~ /ref=\"([^\"]+)\"/) {
    my $oldid=$1;
    my $newid=$ids{$oldid};
    $_ =~ s/ref=\"[^\"]+\"/ref=\"$newid\"/;
  }

  print W $_;
}
close F;
close W;
