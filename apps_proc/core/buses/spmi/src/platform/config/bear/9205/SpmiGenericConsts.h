#define SPMI_MAX_CHANNELS_SUPPORTED   (128)

#define INIT_SPMI_DEV_GENERICS {\
   .mNumChannels                                                            = (SPMI_MAX_CHANNELS_SUPPORTED),\
   .mNumPortPriorities                                                      = (7),\
   .mNumPvcPorts                                                            = (6),\
   .mNumPvcPortPPIDs                                                        = (4),\
   .mNumMasters                                                             = (6),\
   .mNumProgRamRegs                                                         = (256),\
   .mNumMappingTableEntries                                                 = (SPMI_MAX_CHANNELS_SUPPORTED-1),\
   .mNumPicAccStatusRegs                                                    = (4),\
}

