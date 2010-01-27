#!/usr/bin/perl
use strict;
use File::Copy;

chdir 'source/lua_wrap';
my @files = <*.i>;

foreach my $file(@files) {
    print `swig -lua $file`;
}

@ARGV = <*.c>;
$^I = "";
while (<>) {
    $_ =~ s/malloc\.h/stdlib\.h/g;
    print $_;
}

