Hexagon LLVM Linker Plugin Example

This plugin takes all the sections contained in a redistribute section and maps
sections into hot/cold/unlikely sections and calculates the size of the
redistribute section.

This uses the CLADE library for compression.

This uses the OutputSectionIteratorPlugin interface.

Read through the commented MoveSectionsFromOutputSection.cpp plugin source file to understand how this plugin works.

To build and execute the example:  make

The Hexagon linker on Linux will display status messages shown below.
 (your file path will be different)

On Windows, the linker will reference
C:\Qualcomm\HEXAGON_Tools\<version>\Examples\StandAlone_Applications\Linker\Linker_Plugin_examples\MoveSectionsFromOutputSection\MoveSectionsFromOutputSection.dll

Note: Using the absolute path  /home/myaccount/Qualcomm/HEXAGON_Tools/<version>/Examples/StandAlone_Applications/Linker/Linker_Plugin_examples/MoveSectionsFromOutputSection/libMoveSectionsFromOutputSection.so for library MoveSectionsFromOutputSection
Note: Loaded Library   /home/myaccount/Qualcomm/HEXAGON_Tools/<version>/Examples/StandAlone_Applications/Linker/Linker_Plugin_examples/MoveSectionsFromOutputSection/libMoveSectionsFromOutputSection.so requested
Note: Registration function found RegisterAll in Library libMoveSectionsFromOutputSection.so
Note: Plugin handler getPlugin found in Library libMoveSectionsFromOutputSection.so
Note: Cleanup function found Cleanup in Library libMoveSectionsFromOutputSection.so
Note: Registering all plugin handlers for plugin types
Note: Found plugin handler for plugin type GETOUTPUT in Library libMoveSectionsFromOutputSection.so
Note: Initializing Plugin libMoveSectionsFromOutputSection.so, requested by Plugin GETOUTPUT having Name GETOUTPUT
Note: Running Plugin libMoveSectionsFromOutputSection.so, requested by Plugin GETOUTPUT having Name GETOUTPUT
Note: Plugin GETOUTPUT Destroyed
Note: Initializing Plugin libMoveSectionsFromOutputSection.so, requested by Plugin GETOUTPUT having Name GETOUTPUT
Note: Running Plugin libMoveSectionsFromOutputSection.so, requested by Plugin GETOUTPUT having Name GETOUTPUT
Note: Plugin GETOUTPUT Destroyed
Note: Initializing Plugin libMoveSectionsFromOutputSection.so, requested by Plugin GETOUTPUT having Name GETOUTPUT
Note: Running Plugin libMoveSectionsFromOutputSection.so, requested by Plugin GETOUTPUT having Name GETOUTPUT
Note: Plugin GETOUTPUT Destroyed
Note: Unloaded Library libMoveSectionsFromOutputSection.so
Size of __ehdr__	52
Size of __pHdr__	128
Size of .start	16064
Size of .init	84
Size of .redistribute	7928
Size of .hot	84
Size of .cold	84
Size of .unlikely	84
Size of .text	0
Size of .fini	48
Size of .rodata	232
Size of .eh_frame_hdr	0
Size of .eh_frame	4
Size of .gcc_except_table	0
Size of .ctors	16
Size of .dtors	20
Size of .data	1499
Size of .sdata	0
Size of .sbss	0
Size of .bss	3576
Size of .comment	252
Size of .debug	0
Size of .line	0
Size of .debug_aranges	0
Size of .debug_pubnames	0
Size of .debug_info	0
Size of .debug_abbrev	0
Size of .debug_line	0
Size of .debug_frame	0
Size of .debug_str	0
Size of .debug_loc	0
Size of .debug_pubtypes	0
Size of .debug_ranges	0
Size of .note.GNU-stack	0
Size of /DISCARD/	0
Size of .shstrtab	1
Size of .symtab	16
Size of .strtab	1
