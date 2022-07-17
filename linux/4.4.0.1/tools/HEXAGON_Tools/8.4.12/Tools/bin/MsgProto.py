#!/pkg/qct/software/python/2.7.6/bin/python
'''===------ MsgProto.py ---------------------------------------------------===

                       The MCLinker Project

   This file is distributed under the University of Illinois Open Source
   License. See LICENSE.TXT for details.

===-----------------------------------------------------------------------==='''

import sys
import msg_table_pb2

# Print all the MsgTable Header fields.
def PrintHeader(MsgTable):
  for entry in MsgTable.entries:
    print ("Callsite: ", hex(entry.callsiteAddr))
    print ("RO Data: ", hex(entry.rodataAddr))


# Main procedure:  Reads the entire MsgTable from a data file and prints all the information.
def PrintTable(table):

  MsgTable = msg_table_pb2.MsgTable()

  # Read the data file.
  f = open(table, "rb")
  MsgTable.ParseFromString(f.read())
  f.close()

  # Print MsgTable Header.
  PrintHeader(MsgTable)
