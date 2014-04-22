"""
 Constants.py

Description: Immutable items (constants) used in Service Logic.

 Copyright (c) 2015 - 2016 by QUALCOMM, Incorporated.  
             All Rights Reserved.

========================================================================

 EDIT HISTORY FOR MODULE

 $Header: //components/rel/build.tn/4.0/myps/QcMAT40/helper/Constants.py#1 $

 when            who        what, where, why
 --------      ---------  ------------------------------------------------
 Nov 13, 2015   swapneel   First Draft
 ==========================================================================
 """

class Constants:
    """description of class"""
    PADDING_ALIGNMENT_STR      = "PADDING_ALIGNMENT"
    PADDING_STR                = "PADDING"
    LINKER_SCRIPTS_STR         = "Linker scripts used"
    ALLOCATING_COMMON_SYMBOLS  = "Allocating common symbols"
    TOTAL_LINKER_SCRIPT_STR    = "Total Linker Scripts are "

    F3_COUNT_STR               = "_F3_COUNT_"
    FATAL_COUNT_STR            = "_FATAL_COUNT_"


    '''Output  File Names'''
    SUMMARY_FILE_NAME          = "summary"

    PROGRAMHEADER_CSV_STR      = "programHeaders.csv"

    QSR_STRING_STR             = "QSR_string_ptr"
    
    SECTIONFILENAME            = "sections"
    MEMORYANALYSISFILENAME     = "memoryAnalysis"
    INFOFILENAME               = "info"
    COMPRESSION_SEC_FILENAME   = "compressionSectionList"
    SMALL_TRUE_STR             = "true"
    COMMON_STR                 = "COMMON"
    SPECIAL_SYMBOLS_STR        = "SpecialSymbols"
    ROOT_STR                   = "ROOT"
    
    '''non-printable characters'''
    TAB_STR                   = "\t"
    NEWLINE_UNIX_STR          = "\n"
    NEWLINE_WIN_STR           = "\r\n"
    SPACE_STR                 = " "
    HASH_STR                  = "#"
    SLASH_STR                 = "/"
    OPEN_ROUND_BRACKET_STR    = "("
    CLOSE_ROUND_BRACKET_STR   = ")"
    COMMA_STR                 = ","
    UNDERSCORE_STR            = "_"
    DOUBLE_UNDERSCORE_STR     = "__"
    SLASH_STR                 = "/"
    COLON_STR                 = ":"
    DOUBLE_COLON_STR          = "::"
    BACKSLASH_STR             = "\\"
    SINGLE_QUOTE_STR          = "'"
    DOT_STR                   = "."
    OBJ_EXT_STR               = ".o"
    DOUBLE_DASH_STR           = "--"
    EQUAL_TO_STR              = "="

    FW_PREFIX_STR             = "fw_"
    SLASH_FW_SLASH_STR        = "/fw/"
    BIN_PATH_STR              = "/bin/"
    BUILD_PATH_STR            = "/build/"
    SLASH_STAR_SLASH_STR      = "/*/"
    STAR_COLON_OPEN_BRACKET_STR = "*:("

    FILL_SECTION_STR          = "*fill*"

    '''Other'''
    EMPTY_STR             = ""
    WRITE_MODE            = "w"
    READ_MODE             = "r"


    '''Number system related'''
    HEX_START_SMALL_STR   = "0x"
    HEX_START_CAPITAL_STR = "0X"

    ZERO_ADDRESS_32BIT_STR  = "00000000"

    SIXTEEN_SPACES_STR      = "                "  # NOTE: Do not alter the spacing in the value.


    #Settings related constants
    PROPERTIES_FILE_NAME                              = "qcmat.properties";
    SL_VERSION_STR                                    = "SL_VERSION";
    PROCESSORS_SUPPORTED_STR                          = "PROCESSORS_SUPPORTED";
    BUILD_TYPES_SUPPORTED_STR                         = "BUILD_TYPES_SUPPORTED";
    LOGGING_ENABLED_STR                               = "LOGGING_ENABLED";
    ELF_PATH_STR                                      = "ELF_PATH";
    ELF_AND_MAP_LOCATION_SAME_STR                     = "ELF_AND_MAP_LOCATION_SAME";
    PROCESSOR_STR                                     = "PROCESSOR";
    LOG_FILEPATH_STR                                  = "LOG_FILEPATH";
    SETTING_PROPERTY_VALUE_SEPARATOR                  = ":->";
    CONFIG_END_STR                                    = ":=:END:=:"
    ROOT_FOLDER_STR                                   = "ROOT_FOLDER";
    HEXAGON_OBJDUMP_PATH_UNIX_STR                     = "HEXAGON_OBJDUMP_PATH_UNIX";
    SYMBOL_NO_SIZES_STR                               = "SYMBOL_NO_SIZES";
    HEXAGON_6X_MAP_START_STR                           = "HEXAGON_6X_MAP_START";
    HEXAGON_6X_MAP_END_STR                             = "HEXAGON_6X_MAP_END";    
    HEXAGON_7X_MAP_START_STR                          = "HEXAGON_7X_MAP_START";
    HEXAGON_7X_MAP_END_STR                            = "HEXAGON_7X_MAP_END";
    Q6_START_Q6ZIP_STR                                = "Q6_START_Q6ZIP";
    Q6_END_Q6ZIP_STR                                  = "Q6_END_Q6ZIP";
    Q6_COMPRESSION_ADDRESS_STR                        = "Q6_COMPRESSION_ADDRESS";
    MEM_ANALYSIS_FILE_STR                             = "MEM_ANALYSIS_FILE";
    SKIP_PATH_STR                                     = "SKIP_PATH";
    Q6ZIP_SECTIONS_STR                                = "Q6ZIP_SECTIONS";
    STACK_ANALYSIS_STR                                = "STACK_ANALYSIS";

    DEBUG                                             = False