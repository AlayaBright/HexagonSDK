Hexagon LLVM Linker Plugin Example

The ChangeSectionOrder example demonstrates the use of the OutputSectionIteratorPlugin.
This makes use of a simple ChangeOrder plugin that overrides how the blocks are
merged in a specific linker script section (.ReorderSection, in this example.)

This plugin reorders blocks by sorting the sections alphabetically that appear
in the output section.

The linker.script places these sections within .ReorderSection in this sequence:
	.text.third
	.text.first
	.text.second

The ChangeSectionOrder linker plugin then re-orders these sections as:
	.text.first
	.text.second
	.text.third

Read through the commented ChangeSectionOrder.cpp plugin source file to understand how this plugin works.


To build and execute the example:  make


The Hexagon linker on Linux will display status messages shown below.
 (your file path will be different)

On Windows, the linker will reference
C:\Qualcomm\HEXAGON_Tools\<version>\Examples\StandAlone_Applications\Linker\ChangeSectionOrder\ChangeSectionOrder.dll

	Note: Using the absolute path /home/myaccount/Qualcomm/HEXAGON_Tools/<version>/Examples/StandAlone_Applications/Linker/Linker_Plugin_examples/ChangeSectionOrder/libChangeSectionOrder.so for library ChangeSectionOrder
	Note: Loaded Library /home/myaccount/Qualcomm/HEXAGON_Tools/<version>/Examples/StandAlone_Applications/Linker/Linker_Plugin_examples/ChangeSectionOrder/libChangeSectionOrder.so requested
	Note: Registration function found RegisterAll in Library libChangeSectionOrder.so
	Note: Plugin handler getPlugin found in Library libChangeSectionOrder.so
	Note: Registering all plugin handlers for plugin types
	Note: Found plugin handler for plugin type ORDERBLOCKS in Library libChangeSectionOrder.so

	This plugin only operates on the section named: .ReorderSection
	This is the orignal order of this section:

	.text.third
	.text.first
	.text.second
	Note: Running handler for section .reordersection
	Note: Plugin returned 3 blocks when processing section .reordersection
	Note: Plugin ORDERBLOCKS Destroyed


[After the Hexagon linker is done, the makefile runs hexagon-nm to show
that the order of the three sections has actually been changed:]

	hexagon-nm -n target/main.elf | grep -E "first|second|third"
	00004060 T first
	00004070 T second
	00004080 T third
