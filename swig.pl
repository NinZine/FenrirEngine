#!/usr/bin/perl
use strict;
use File::Copy;

chdir 'source/lua_wrap';
my @files = <*.i>;

foreach my $file(@files) {
    if ($#ARGV >= 0) {
        print `swig -lua -D$ARGV[0] $file`;
    } else {
        print `swig -lua $file`;
    }
}

@ARGV = <*.c>;
$^I = "";
while (<>) {
    $_ =~ s/malloc\.h/stdlib\.h/g;
    print $_;
}

