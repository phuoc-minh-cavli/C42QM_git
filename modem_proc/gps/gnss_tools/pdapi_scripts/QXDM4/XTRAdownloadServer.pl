#!/usr/bin/perl

use strict;
use warnings;

use LWP::Simple;
system "mkdir c:\\predictedorbit";
system "mkdir c:\\Diag_Inject_Xtra";
system "copy \/y \\\\asiafs\\source\\atm\\AST_GPS\\IZat\\GNSSTest_padpi\\Diag_Inject_Xtra\\ c:\\Diag_Inject_Xtra\\";

my $url = 'http://xtrapath3.izatcloud.net/xtra3grc.bin';
my $file = 'c:\\predictedorbit\\xtra.bin';
my $file2 = 'c:\\Diag_Inject_Xtra\\xtra.bin';

getstore($url, $file);
getstore($url, $file2);