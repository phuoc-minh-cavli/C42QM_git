#!/usr/bin/perl
#******************************************************************************
#* $Header: //components/rel/mcfg.mpss/10.1.1/mcfg_gen/scripts/read_apex_xml_dump_nv_xml.pl#1 $
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
use strict;
use File::Spec;
use File::Basename;
use Data::Dumper;
use JSON;
use JSON::XS;
use Getopt::Long;
use XML::Simple; # for writing
use XML::Fast;
# Prevent Perl version 5.18+ from emitting unnecessary warnings regarding the use of 'keys' on a hashref. 
#no warnings 'experimental';
no if ($] >= 5.018), 'warnings' => 'experimental'; 

my @globalEnumJson;

my $startTime = time;

# Flush immediately
$| = 0;

#############################################
##
##  Local configuration
##
##
#############################################

my $srcDir = dirname($0);
my $dbDir = File::Spec->catfile($srcDir,'../../mcfg_tools/qxdm_db');
my $dbx = File::Spec->catfile($dbDir,'APEXNvItems.dbx');
my $db_json = File::Spec->catfile($dbDir,'APEXNvItems.json');

sub uploadFieldAndType($$$);

sub slurp($){
  my $file = shift(@_);
  #print Dumper($file);
  open my $fh, "<", $file or die "could not open $file: $!";
  my $doc = do {
      local $/ = undef;      
      <$fh>;
  };
  close $fh;
  return $doc;
}

my $apexTables;
if (0) {
  ################################
  # XML::Fast only needed
  # when converting XML db to JSON
  # Otherwise XML::Fast not needed
  ################################
  require XML::Fast; # for reading

  ################################
  # Read XML file to hash 
  ################################
  my $nvsTmp = xml2hash (slurp($dbx), attr=>'');
  #my $nvs = xml2hash (slurp('NvDefinition.xml'), array => ['Member']);

  #print Dumper($nvsTmp);
  
  
  $apexTables = {
    '__tblTypes' => 1, 
    '__tblFields' => 1, # Using parentId as hash key
    '__tblUnits' => 1, 
    '__tblEnumValues' => 1, # Using parentId as hash key
    '__tblDescriptions' => 1, 
    '__tblCategories' => 1, 
    '__tblFormats' => 1, 
    '__tblFormulas' => 1, 
    'tblNvItems' => 1
  };

  for my $table (@{$nvsTmp->{'Db'}->{'Table'}}){
    my $tableName = $table->{'name'};
    if($apexTables->{$tableName}){
      $apexTables->{$tableName} = {};
      for my $record (@{$table->{'Record'}}){
        if('__tblDescriptions' eq $tableName) {
          if ($apexTables->{$tableName}->{$record->{'id'}}){
            $apexTables->{$tableName}->{$record->{'id'}}->{'text'} .= $record->{'text'};
            next;
          } 
        }
        ## Use parentId as key
        elsif (('__tblFields' eq $tableName) || ('__tblEnumValues' eq $tableName)) {
          if(!$apexTables->{$tableName}->{$record->{'parentId'}}){
            $apexTables->{$tableName}->{$record->{'parentId'}} = [];
          }
          push(@{$apexTables->{$tableName}->{$record->{'parentId'}}}, $record);
          next;
        }

        $apexTables->{$tableName}->{$record->{'id'}} = $record;
      }
    }
  }

  print encode_json($apexTables);
  undef ($nvsTmp);
  exit(1);
}
else {
  $apexTables = decode_json(slurp ($db_json));
}

################################
# Helper to find type memebrs
#
# args: 
#   parentId  : number
################################
sub findFields($){
  my $parentId = shift(@_);
  return $apexTables->{'__tblFields'}->{$parentId};
}

################################
# Helper to find category
#
# args: 
#   categoryId  : number
################################
sub findCategories($){
  my $categoryId = shift(@_);
  return $apexTables->{'__tblCategories'}->{$categoryId};
}

################################
# Helper to upload a full type
#
# args: 
#   typeId           : int
################################
sub uploadType($$);
sub uploadType($$){
  my $typeId = shift(@_);
  my $version = shift(@_);

  my $apexParentType = $apexTables->{'__tblTypes'}->{$typeId};
  if ($apexParentType) {

    my $apexParentTypeFields = $apexTables->{'__tblFields'}->{$typeId};
    if ($apexParentTypeFields) {
      my @typeMebers4Api = ();
      for my $apexField (@{$apexParentTypeFields}) {
        my $fieldTypeId = $apexField->{'typeId'};
        my $apexFieldType = $apexTables->{'__tblTypes'}->{ $fieldTypeId };
        my $apexFieldTypeName = $apexFieldType->{'name'};

        # Convert Char8[] and Char16[] to non-arrays
        $apexFieldTypeName =~ s/\[\]//;

        #print "DBG: Saving type $apexFieldTypeName \n";
        my $dbMemberType;

        if (!$dbMemberType) {        
          if($apexFieldType->{'length'} == -1){
            $apexFieldType->{'length'} = 8;
          }

          $dbMemberType = uploadType($fieldTypeId, $version);
          if ('TYPE_CLASS_UNION' eq $apexFieldType->{'typeClass'}) {
            ## For union find all members with same parent
            ##print "WRN: Found union type, will pick the child at index: $version\n";
            #last;

# TODO: Figure out how to pick correct union member, pass in as argumnet to this function, etc.
# And of course this all goes wrong if there are nested unions in the NV definition
            #print Dumper($dbMemberType->{'children'});
            #print "-----------------\n";
            $dbMemberType->{'children'} = [$dbMemberType->{'children'}[$version]];
          }

          # if ('TYPE_CLASS_ENUM' eq $apexFieldType->{'typeClass'}) {
            # for my $enum (@{$apexTables->{'__tblEnumValues'}->{$fieldTypeId}}){
              # apiPostEnum($dbMemberType->{'DataTypeUID'}, $enum->{'name'}, $enum->{'value'});
            # }
          # }
        }      
        
        my $encoding; 
        if ('TYPE_CLASS_STRING' eq $apexFieldType->{'typeClass'}) {
          # modify count field from length
          $apexField->{'count'} = int($apexField->{'length'} / 8);
          $apexField->{'encoding'} = 'STRING';
        }

        $apexField->{'type'} = $dbMemberType;
        my $fieldDesc = $apexTables->{'__tblDescriptions'}->{$apexField->{'descId'}};
        if ($fieldDesc) {
          $apexField->{'description'} = $fieldDesc->{'text'};
        }
        push(@typeMebers4Api, $apexField);        
      }
      $apexParentType->{'children'} = \@typeMebers4Api;
    }
  }

  return $apexParentType;
}

################################
# Helper to convert to Q format 
#
# args: 
#   data  : hash
#   type  : string
#
################################

sub toQFormat($$) {
  my $data = shift(@_);
  my $type = shift(@_);

  return {} if (!$data);

  # common
  my $newData = {
    'name' => $data->{'name'}, 
    'description' => $data->{'description'}
  };

  my $idd = $data->{'type'}->{'id'};
  my @enumVals = $apexTables->{'__tblEnumValues'}->{$idd};

  my %hashTemp;
  my @arrayTemp = ();

  my $j = 1;
  my $enum = 0;
  my $temp = $apexTables->{'__tblEnumValues'}->{$idd}[0]->{'name'};
  my $rangeValue = $apexTables->{'__tblEnumValues'}->{$idd}[0]->{'value'};
  while($temp ne ''){
    $enum = 1;
    push(@arrayTemp, ([$temp, $rangeValue]));
    $temp = $apexTables->{'__tblEnumValues'}->{$idd}[$j]->{'name'};
    $rangeValue = $apexTables->{'__tblEnumValues'}->{$idd}[$j]->{'value'};
    $j += 1;
  }
  if($enum == 1){
    $hashTemp{'name'} = $data->{'type'}->{'name'};
    $hashTemp{'values'} = \@arrayTemp;
    push(@globalEnumJson, \%hashTemp);
  }


  # type specific
  if ('nv' eq $type) {
    $newData->{'id'} = $data->{'id'};    
    $newData->{'category'} = $data->{'category'};
    $newData->{'Member'} = [];
    foreach my $member (@{$data->{'type'}->{'children'}}) {
      push(@{$newData->{'Member'}}, toQFormat($member, 'member'));
    }    
  }
  else { #('member' eq $type) {
    $newData->{'sizeOf'} = $data->{'count'};
	$newData->{'length'} = $data->{'length'};
    #$newData->['position'] = $data->{'position'};
    $newData->{'type'} = $data->{'type'}->{'name'};

    if ($data->{'type'}->{'children'}) {
      $newData->{'Struct'}->{'Member'} = [];
      foreach my $member (@{$data->{'type'}->{'children'}}) {
        push(@{$newData->{'Struct'}->{'Member'}}, toQFormat($member, 'member'));
      }
    }
    else {
      # Convert basic type name to lower case
      $newData->{'type'} = lc($newData->{'type'});
    }
  }
  return $newData;
}

my $op_fmt = 'json';
my $apexNull = -2147483648;
#for my $nvKey (sort {$a <=> $b} keys($apexTables->{'tblNvItems'})){
#for (my $nvKey=564;$nvKey<65; $nvKey++) {
#for my $nvKey (0 .. 10) {
#for my $nvKey (3420 .. 3430) {
my $version = -1;
my $nvKey = '0'; GetOptions( "item=s" => \$nvKey, "op=s" => \$op_fmt, "version=s" => \$version ); { 
  
  # search by name instead of NV#
  if ($nvKey !~ /^\d+$/)
  {
    #nvKey pass is a path to an item
    foreach my $nvItemKey (keys $apexTables->{'tblNvItems'})
    {
      if ( $apexTables->{'tblNvItems'}->{$nvItemKey}->{'name'} eq $nvKey )
      {
      #if ( $nvItem->{'name'}=~ // ){ # for regex
        $nvKey = $apexTables->{'tblNvItems'}->{$nvItemKey}->{'id'};
        last;
      }
    }
  }

  next if(!$apexTables->{'tblNvItems'}->{$nvKey});
  my $nv = $apexTables->{'tblNvItems'}->{$nvKey};  
  #print "$_\n" for keys $nv;
  my $name = $nv->{'name'};
  my $nvId = $nv->{'id'};
  

  #add category name to nv
  $nv->{'category'} = (findCategories($nv->{'categoryId'}))->{'name'};
  
  my $rootTypeId = $nv->{'typeId'};
  # There are NV items that do not have any type we do not import them.
  if($apexNull == int($rootTypeId)){
    #print "WRN: NV($nvId):$name, does not have type, item will not imported to NVSS.\n";
    next;
  }

  my $rooType = $apexTables->{'__tblTypes'}->{$rootTypeId};
  
  #print $rooType->{'name'};
  my $nvDesc = $apexTables->{'__tblDescriptions'}->{$nv->{'descId'}};
  if ($nvDesc) {
    $nv->{'description'} = $nvDesc->{'text'};
  }

  ## Create whole structure 
  $nv->{'type'} = uploadType($rooType->{'id'}, $version);
  
  ## Just to create one more top level entry
  my $nvOut = {'NvData' => $nv};

  ## Print as Perl hash
  print Dumper($nvOut) if ($op_fmt eq 'hash'); 

  ## Print as pretty JSON
  #print JSON::XS->new->ascii->pretty->encode($nvOut);


  # To output our XML format we need to pluck and renaem fields and add custom ones

  my $inQFmt = toQFormat($nv, 'nv');
  print JSON::XS->new->ascii->pretty->encode($inQFmt) if ($op_fmt eq 'json'); 

  ## Print as XML
  #print XMLout($nvOut, KeepRoot => 1, NoAttr => 1);
  print XMLout({'NvItemData' => $inQFmt}, AttrIndent => 1, KeepRoot => 1) if ($op_fmt eq 'xml_wa');
 
  my %finalEnumhash = (enums=>\@globalEnumJson);
  my $json_enums = JSON::XS->new->ascii->pretty->encode(\%finalEnumhash);
  print "$json_enums" if ($op_fmt eq 'json_enum');
}

my $duration = time - $startTime;
#print "\nExecution time: $duration s\n";
