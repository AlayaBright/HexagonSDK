#!/pkg/qct/software/python/2.7.6/bin/python
'''===------ MapParser.py ---------------------------------------------------===

                       The MCLinker Project

   This file is distributed under the University of Illinois Open Source
   License. See LICENSE.TXT for details.

===-----------------------------------------------------------------------==='''

import sys
try:
    from MsgProto import MsgProto
except Exception as e:
    import MsgProto
 
# Main procedure:  Reads the entire MsgTable from a data file and prints all the information.
if len(sys.argv) != 2:
  print ("Usage: " + sys.argv[0] + " <MsgTable>")
  sys.exit(-1)
MsgProto.PrintTable(sys.argv[1])
