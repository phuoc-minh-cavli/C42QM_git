#******************************************************************************
#* $Header: //components/rel/mcfg.mpss/10.1.1/mcfg_gen/scripts/McfgGenUtils.pm#1 $
#* $DateTime: 2020/02/19 23:07:48 $
#*
#* 
#******************************************************************************
#*
#* Copyright (c) 2016 Qualcomm Technologies, Inc.
#* All rights reserved.
#* Qualcomm Technologies, Inc. Confidential and Proprietary.
#*
#******************************************************************************
# Helper functions for data manipulation / validation
package McfgGenUtils;

use Exporter;
our @ISA = qw(Exporter);
@EXPORT = qw( 
  Trim IsBlank IsSubsMaskPresent ItemIsSoftwareOnly 
  ItemIsDelete ItemIsDeleteAndSWOnly ItemIsMuxd 
  ItemIsIndexed ItemIsSubscriber SizeIsValid 
  IsHexString IsDefined strmatch
);

# Trims leading and trailing whitespace from variable contents. 
sub Trim {
   my ($val) = @_;

   $val =~ s/^\s+|\s+$//g;
   return $val;
}

# Determines if a field is blank (not including whitespace)
sub IsBlank {
   my ($field) = @_;

   if(defined($field)) {
     $field = Trim($field);
   }

   return $field eq '';
}

# Determines if the string sub_mask or subs_mask is present in the field
sub IsSubsMaskPresent
{
  my ($field) = @_;

  if(defined($field)) {
    $field = Trim( lc($field) );
  }

  return ($field =~ m/sub(s)?_mask/);
}

# from the attributes of an item, determine if it's software only
sub ItemIsSoftwareOnly($) {
   my ($attributes) = @_;

   $attributes = hex($attributes);

   return not ($attributes & 0x01);
}

# Determines if delete bit is set in the attributes
sub ItemIsDelete($) {
   my ($attributes) = @_;

   return ($attributes & 0x40);
}

# Determines if delete bit is set and software only in the attributes
sub ItemIsDeleteAndSWOnly {
   my ($attributes) = @_;

   if ( ItemIsDelete( $attributes ) && ItemIsSoftwareOnly( $attributes )) {
      return 1;
   }
   else {
      return 0;
   }
}

# Determines if mux'd bit is set in the attributes
sub ItemIsMuxd($) {
   my ($attributes) = @_;

   return ($attributes & 0x02);
}

# Determines if indexed bit is set in the attributes
sub ItemIsIndexed($) {
   my ($attributes) = @_;

   return ($attributes & 0x20);
}

# Determines if subscriber bit is set in the attributes
sub ItemIsSubscriber($) {
   my ($attributes) = @_;

   return ($attributes & 0x10);
}

sub SizeIsValid($) {
   my ($size) = @_;

   return ($size == 1) || ($size == 2) || ($size == 4) || ($size == 8);
}

sub IsHexString($) {
   my ($string) = @_;

   return $string =~ /^0x[0-9a-fA-F]+$/;
}

# Parameters: Any single variable
# Returns:   Boolean value indicating whether value is present or not
sub IsDefined {
   my $variable = shift;

   return ( (defined $variable) ? 1 : 0 );
}

# Compares two strings without respect to upper/lower case
# Returns true if they match
sub strmatch($$)
{
  (my $a, $b) = @_;

  return (lc $a eq lc $b);
}

1;
