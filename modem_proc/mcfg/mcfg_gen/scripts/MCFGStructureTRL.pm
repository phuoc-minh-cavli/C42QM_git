#!/usr/bin/perl
#******************************************************************************
#* $Header: //components/rel/mcfg.mpss/10.1.1/mcfg_gen/scripts/MCFGStructureTRL.pm#1 $
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

# This pm file is used to maintain MetaData related to TRL items. 
package MCFGStructureTRL;
use Data::Dumper;

#====== Initialize TRL MetaData ======
my $MCFG_TRL_STRUCT_VERSION_STRUCT = [
  {
    'name' => 'MCFG_trl_struct_version_type',																				
    'description' => 'Type ID for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint8',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'MCFG_trl_struct_version_len',																				
    'description' => 'Length for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint16',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'MCFG_trl_struct_version',																				
    'description' => 'Value for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint16',
    'sizeOf' => '1',
    'value' => undef,
  },
];

my $MCFG_version_STRUCT = [
  {
    'name' => 'MCFG_version_type',																				
    'description' => 'Type ID for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint8',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'MCFG_version_len',																				
    'description' => 'Length for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint16',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'MCFG_version',																				
    'description' => 'Value for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint32',
    'sizeOf' => '1',
    'value' => undef,
  },
];

my $MCC_MNC_Info_type_STRUCT = [
  {
    'name' => 'MCC_MNC_Info_type',																				
    'description' => 'Type ID for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint8',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'MCC_MNC_Info_len',																				
    'description' => 'Length for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint16',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'MCC',																				
    'description' => 'Length for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint16',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'MNC',																				
    'description' => 'Length for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint16',
    'sizeOf' => '1',
    'value' => undef,
  },
];

my $Carrier_Name_type_STRUCT = [
  {
    'name' => 'Configuration_Name_type',																				
    'description' => 'Type ID for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint8',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'Configuration_Name_len',																				
    'description' => 'Length for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint16',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'Configuration_Name',																				
    'description' => 'Value for Trailer Record TLV',
    'comment' => '',
    'type' => 'char8[]',
    'sizeOf' => undef,
    'value' => undef,
  },
];

my $IIN_List_type_STRUCT = [
  {
    'name' => 'IIN_List_type',																				
    'description' => 'Type ID for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint8',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'IIN_List_len',																				
    'description' => 'Length for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint16',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'IIN_Flag',																				
    'description' => 'Value for Trailer Record TLV',
    'comment' => 'IIN_Flag = 0: IINs compared to SIM during autoselection / IIN_Flag = 1: Configuration used as fallback during autoselection',
    'type' => 'uint8',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'IIN_List_Count',																				
    'description' => 'Value for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint8',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'IIN_List',																				
    'description' => 'Value for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint32[]',
    'sizeOf' => undef,
    'value' => undef,
  },
];

my $QC_version_type_STRUCT = [
  {
    'name' => 'QC_version_type',																				
    'description' => 'Type ID for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint8',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'QC_version_len',																				
    'description' => 'Length for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint16',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'QC_version',																				
    'description' => 'Value for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint32',
    'sizeOf' => '1',
    'value' => undef,
  },
];

my $MCC_MNC_List_type_STRUCT = [
  {
    'name' => 'MCC_MNC_List_type',																				
    'description' => 'Type ID for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint8',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'MCC_MNC_List_len',																				
    'description' => 'Length for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint16',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'MCC_MNC_Flag',																				
    'description' => 'Value for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint8',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'MCC_MNC_List_Count',																				
    'description' => 'Value for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint8',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'MCC_MNC_List',																				
    'description' => 'Value for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint16[]',
    'sizeOf' => undef,
    'value' => undef,
  },
];

my $MCFG_capability_STRUCT = [
  {
    'name' => 'MCFG_capability_type',																				
    'description' => 'Type ID for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint8',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'MCFG_capability_len',																				
    'description' => 'Length for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint16',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'MCFG_capability',																				
    'description' => 'Value for Trailer Record TLV',
    'comment' => '(BitX=LO/HI): bit0(Applicable to HW mbn only)=SS/DS  bit1=OpenMarket/Subsidized  bit2=NonVoLTE/VoLTE  bit3:  commerical/lab  bits4-31=reserved',
    'type' => 'uint32',
    'sizeOf' => '1',
    'value' => undef,
  },
];
   
my $IIN_list_string_STRUCT = [
  {
    'name' => 'IIN_List_String_type',																				
    'description' => 'Type ID for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint8',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'IIN_list_string_length',
    'description' => 'Length for Trailer Record TLV',
    'comment' => '',
    'type' => 'uint16',
    'sizeOf' => '1',
    'value' => undef,
  },
  {
    'name' => 'IIN_list_string_value',																				
    'description' => 'Value for Trailer Record TLV',
    'comment' => '',
    'type' => 'char8[]',
    'sizeOf' => undef,
    'value' => undef,
  },
];

# List new TRL TYPE ID structures in corresponding index of @TRL_TLV_TYPES
my @TRL_TLV_TYPES = (
  $MCFG_TRL_STRUCT_VERSION_STRUCT, 
  $MCFG_version_STRUCT, 
  $MCC_MNC_Info_type_STRUCT, 
  $Carrier_Name_type_STRUCT, 
  $IIN_List_type_STRUCT, 
  $QC_version_type_STRUCT, 
  $MCC_MNC_List_type_STRUCT, 
  $MCFG_capability_STRUCT, 
  undef, # index 8 -- reserved for digest TLV (not present in XML/source files)
  undef, # index 9 -- used for CRM BUILD VER TLV (not present in XML/source files)
  $IIN_list_string_STRUCT,
);

#====== Function Declarations ======
# getTlvId()                    Uses MetaData structure definitions to collect TLV IDs from item hash
# @param: $id                   ID corresponding to TRL TLV type
# @return: $TRL_TLV_TYPES[$id]  hash reference representing structure for TLV type 
sub getTlvId
{
  my ($id) = @_;
  return $TRL_TLV_TYPES[$id];
}

# getTlvIdList()        Uses MetaData structure definitions to collect TLV IDs from item hash
# @param: $item         hash containig TRL data (typically from spreadsheet)
# @return: @tlv_id_arr  array of TRL TLV type ids 
sub getTlvIdList
{
  my ($item) = @_;
  my $tlv_id = 0;
  my @tlv_id_arr = ();

  # can't assume TRL records are in any specific order
  my $last_idx = $#{ $item->{Elements} };
  #print Dumper($item);
  for (my $i = 0; $i<=$last_idx; $i++)
  {
    #print "i: $i\n";
    # Get TLV ID value from line $i of TRL data
    $tlv_id = $item->{Elements}->[$i]->{Value};
    # Value may be in array form if parsing data from XML (as opposed to spreadsheet)
    if (ref($tlv_id) eq 'ARRAY')
    {
      $tlv_id = $tlv_id->[0];
    }
    #print "tlv_id: $tlv_id \n";
    if (!defined($TRL_TLV_TYPES[$tlv_id]))
    {
      die "No metadata defined in TRL_TLV_TYPES for tlv id '$tlv_id'. Despite TRL TLVs being optional the associated metadata needs to be defined in MCFGStructureTRL.pm when generating XMLs from workbook.\n"
    }
    else
    {
      push @tlv_id_arr, $tlv_id;
    }

    my $record_members = $TRL_TLV_TYPES[$tlv_id];
    #print "record_members: " . scalar(@$record_members) . "\n";
    # Skip to last member associated with this TRL TLV ID
    $i += scalar(@$record_members) - 1;
    #print "i: $i\n";

    #print "element \n";
    #print Dumper($item->{Elements}->[$i]);
    if (!defined($item->{Elements}->[$i]))
    {
      die "Not enough info to populate record for TRL TLV type id $tlv_id\n";
    }
  }

  #print 'id array';
  #print Dumper(@tlv_id_arr);
  return @tlv_id_arr;
}

# Adjustment functions below ensure metadata and Value given for a TLV object match
my $adjust_handler = {
  'Configuration_Name' => \&AdjustConfigNameData,
  'IIN_List' => \&AdjustIINData,
  'MCC_MNC_List' => \&AdjustMccMncData,
  'IIN_List_String' => \&AdjustIINStringData,
};

# AdjustConfigNameData -- Corrects Configuration Name data based on the values given in xml
sub AdjustConfigNameData
{
  my ($idStructure) = @_;
  my ($TYPE, $LIST_LEN, $LIST) = (0,1,2);
  my @Config_name = @{ $idStructure->[$LIST]->{value} };

  # convert decimal array to ascii string for easy manipulation
  my $conf_name = pack('C*', @Config_name);
  $conf_name =~ s/(\0+)$//g; #remove null padding from end
  $conf_name =~ s/^(\0+)?//g; #remove null padding from front (just to be safe)
  if (length($conf_name) == 0 || (length($conf_name) == 1 && $conf_name == 0))
  {
    $idStructure->[$LIST_LEN]->{value} = [0]; #If there is no string, then the length will be zero.
    $idStructure->[$LIST]->{sizeOf} = 0;
    $idStructure->[$LIST]->{value} = [];
  }
  else
  {
    # update corresponding lengths to reflect new data
    my $str_len = length($conf_name);
    $idStructure->[$LIST_LEN]->{value} = [$str_len];
    $idStructure->[$LIST]->{type} =~ s/\d+/$str_len/;

    @Config_name = unpack('C*', $conf_name);
    $idStructure->[$LIST]->{value} = \@Config_name;
  }
  return $idStructure;
}

# AdjustIINData -- Corrects IIN data based on the values given in the IIN_List
sub AdjustIINData
{
  my ($idStructure) = @_;
  my ($TYPE, $LIST_LEN, $LIST_COUNT, $LIST) = (0,1,3,4);

  @IINs = @{ $idStructure->[$LIST]->{value} };
  if (scalar(@IINs) == 0 || (scalar(@IINs) == 1 && $IINs[0] == 0))
  {
    $idStructure->[$LIST_LEN]->{value} = [2]; # 2B for IIN flag and IIN count values
    $idStructure->[$LIST_COUNT]->{value} = [0];
    $idStructure->[$LIST]->{sizeOf} = 0;
  }
  else
  {
    $idStructure->[$LIST_LEN]->{value} = [2 + 
      scalar(@IINs) * 4]; # 2B for IIN flag and IIN count + 4B for each IIN
    $idStructure->[$LIST_COUNT]->{value} = [scalar(@IINs)];
  }
  return $idStructure; 
}

# AdjustMccMncData -- Corrects IIN data based on the values given in the MCC_MNC_list
sub AdjustMccMncData
{
  my ($idStructure) = @_;
  my ($TYPE, $LIST_LEN, $LIST_COUNT, $LIST) = (0,1,3,4);

  @PLMNs = @{ $idStructure->[$LIST]->{value} };
  if (scalar(@PLMNs) < 2)
  {
    $idStructure->[$LIST_LEN]->{value} = [2]; # 2B for PLMN flag and PLMN count values
    $idStructure->[$LIST_COUNT]->{value} = [0];
    $idStructure->[$LIST]->{sizeOf} = 0;
  }
  else
  {
    $idStructure->[$LIST_LEN]->{value} = [2 + 
      scalar(@PLMNs) * 2]; # 2B for IIN flag and IIN count + 2B for each MCC and MNC
    $idStructure->[$LIST_COUNT]->{value} = [scalar(@PLMNs) / 2]; # count = ttl MCC and MNC combos
  }
  return $idStructure; 
}

# AdjustIINStringDataD -- Corrects IIN String data based on the values given in the IIN_List_String
sub AdjustIINStringData
{
  my ($idStructure) = @_;
  my ($TYPE, $LIST_LEN, $LIST) = (0,1,2);
  my @IIN_data = @{ $idStructure->[$LIST]->{value} };

  # convert decimal array to ascii string for easy manipulation
  my $iin_str = pack('C*', @IIN_data);
  $iin_str =~ s/(,+)?(\0+)$//g; #remove null padding and trailing commas from end
  $iin_str =~ s/^(,+)?(\0+)?//g; #remove null padding and trailing commas from front (just to be safe)
  if (length($iin_str) == 0 || (length($iin_str) == 1 && $iin_str == 0))
  {
    $idStructure->[$LIST_LEN]->{value} = [0]; #If there is no string, then the length will be zero.
    $idStructure->[$LIST]->{sizeOf} = 0;
    $idStructure->[$LIST]->{value} = [];
  }
  else
  {
    #make list comma separated with no spaces -- assume trailing space already trimmed
    #Comma separated IIN strings are expected by source code on target
    $iin_str =~ s/([a-zA-Z0-9]+),?(\s+)?/\1,/g;

    # update corresponding lengths to reflect new data
    my $str_len = length($iin_str);
    $idStructure->[$LIST_LEN]->{value} = [$str_len];
    $idStructure->[$LIST]->{type} =~ s/\d+/$str_len/;

    @IIN_data = unpack('C*', $iin_str);
    $idStructure->[$LIST]->{value} = \@IIN_data;
  }
  return $idStructure; 
}

# Calls functions for certain TRL TLVs that may need metadata corrected
# based on the data supplied for the Value component of TLV
sub AdjustTrlData
{
  my ($idStructure) = @_;
  my ($TYPE) = (0);
  my ($typename) = $idStructure->[$TYPE]->{name} =~ m/(Configuration_Name|IIN_List_String|IIN_List|MCC_MNC_List)/;

  if (($typename) and (defined($adjust_handler->{$typename})))
  {
    $idStructure = $adjust_handler->{$typename}($idStructure);
  }

  return $idStructure;
}


sub ValidateTRLDataByStruct
{
  my ($item) = @_;

  # TRL records follow TLV (type, length, value) format. 
  # Gather list of TLV ids and verify there is enough data for each id structure
  my @tlv_id_arr = MCFGStructureTRL::getTlvIdList($item);

  # Build list of TRL TLV structures from present config
  my $config_trl_tlvs = [];
  my $tlv_structElem_index = 0;
  my $tlv_structElem_prev_index = 0;

  foreach my $id (@tlv_id_arr)
  {
    my $idStructure = MCFGStructureTRL::getTlvId($id);
    if (!defined($idStructure))
    {
      print "No TRL record structure data for id: '$id'\n";
      print "Moving to next id\n";
    }
    else
    {
      $tlv_structElem_index = $tlv_structElem_prev_index;

      for my $idStructElement (@$idStructure)
      {
        # Map the data to TLV structure definitions
        $idStructElement->{type} = $item->{Elements}->[$tlv_structElem_index]->{Type};
        $idStructElement->{sizeOf} = $item->{Elements}->[$tlv_structElem_index]->{Size};
        $idStructElement->{value} = $item->{Elements}->[$tlv_structElem_index]->{Value};
        $tlv_structElem_index++;
      }
      $idStructure = AdjustTrlData($idStructure);

      # Set start position for next tlv read from extracted elements
      $tlv_structElem_prev_index = $tlv_structElem_index;

      push(@{$config_trl_tlvs}, @{$idStructure});
      #print Dumper($config_trl_tlvs);
    }
  }

  for (my $index = 0; $index <= $#{ $item->{Elements} }; $index++)
  {
    $item->{Elements}->[$index]->{Type} = $config_trl_tlvs->[$index]->{type};
    $item->{Elements}->[$index]->{Size} = $config_trl_tlvs->[$index]->{sizeOf};
    $item->{Elements}->[$index]->{Value} = $config_trl_tlvs->[$index]->{value};
  }
}

1;
