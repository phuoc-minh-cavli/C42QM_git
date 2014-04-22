#define SPMI_MAX_CHANNELS_SUPPORTED   (2)

#define INIT_SPMI_DEV_GENERICS {\
   .mNumChannels                                                            = (SPMI_MAX_CHANNELS_SUPPORTED),\
   .mNumPortPriorities                                                      = (1),\
   .mNumPvcPorts                                                            = (1),\
   .mNumPvcPortPPIDs                                                        = (1),\
   .mNumMasters                                                             = (1),\
   .mNumProgRamRegs                                                         = (1),\
   .mNumMappingTableEntries                                                 = (SPMI_MAX_CHANNELS_SUPPORTED-1),\
   .mNumPicAccStatusRegs                                                    = (1),\
}

