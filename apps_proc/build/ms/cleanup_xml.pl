#!/usr/bin/perl

# This script does some cleanup on an XML file. It ensures parameter
# values are contained within quotes. It resolves any arithmetic
# expressions in the parameter values. It also removes XML comments
# because at the moment, elfweaver does not handle them.

use strict;

my $input_xml = "";
my $output_xml = "";
my $count;
my $line;

# Command line parsing...
for ($count = 0; $count < @ARGV; $count++) {
  if (@ARGV[$count] eq "-o") {
    $output_xml = @ARGV[$count + 1];
    $count++;
  } elsif (@ARGV[$count] eq "-i") {
    $input_xml = @ARGV[$count + 1];
    $count++;
  } else {
    print "Unrecognized command line option @ARGV[$count].\n";
    print "Usage:\n";
    print " -i <filename>: Specify an input XML file.\n";
    print " -o <filename>: Specify an output XML file.\n";
    print "";
    exit(-1);
  }
}

# Make sure we have required parameters.
if (($input_xml eq "") || ($output_xml eq "")) {
  print "You must specify an input XML file and an output XML file.\n";
  exit(-1);
}

open (input_xml, $input_xml) || die("Could not open the input XML file $input_xml.");
open (output_xml, "+>$output_xml") || die("Could not open the output XML file $output_xml.");

# Read input XML in one line at a time.
while ($line = <input_xml>) {
  # We only care about lines that have XML tags in them. These lines
  # must begin with any combination of spaces or tabs ( ,\t)* followed
  # by < , any number of characters, > , then more spaces or tabs.
  if ($line =~ m/( ,\t)*<.*>( ,\t)*/ && $line !~ m/<!--.*>/) {
    # This line has an XML tag in it.

    # Make sure all parameter values are in quotes. Since parameter values
    # may be arithmetic expressions, this gets a little complex. An explanation
    # of this regular expression is in order...
    #
    # (\w+)= is the name of the parameter and the equals sign following it.
    #
    # ([\w\s()+-]+?) is the value of the key, which may be an arithmetic expression.
    #   It possibly contains alphanumeric characters, parenthesis, spaces, and + or
    #   - signs. We make it not greedy, because if this parameter is followed by another,
    #   a greedy match would steal all but one character of the following parameter name.
    #
    # (\s*\w+=|\s*>|\s*\/>) Things that can follow the parameter value. There can be
    #   0 or more whitespace characters, then either the name of another parameter or
    #   the end of the tag (specified as either > or />).
    #
    # We must run this over and over until there are no more matches. Doing a single
    # global match doesn't work, probably because a match consumes part of an
    # adjacent parameter (meaning that parameter then won't be matched).
    while($line =~ s/ (\w+)=([\w\s()\+\-\*]+?)(\s*\w+=|\s*>|\s*\/>)/ $1=\"$2\"$3/g) {
    }

    # Resolve any arithmetic expressions.
    $line =~ s/ (\w+)=\"\s*\(([\w\s()\+\-\*]+)\)\"/ " " . $1 ."=\"" . sprintf("0x%x",eval($2)) . "\""/ge;
    print output_xml $line;
  }
}

close input_xml;
close output_xml;
