# Copyright 2013 Google Inc. All Rights Reserved.

""" A small IDAPython script that loads debug information from a PDB file
into your IDB even if you are not under Windows. Depends on pdbparse.

"""

__author__ = "thomasdullien@google.com (Thomas Dullien)"

import pdbparse

# Needed for PDBs without object map directory
class DummyOmap(object):
  def remap(self, addr):
    return addr

def ExtractPESymbols(PDB_path, image_base):
  """ Processes a PE file, loads it's corresponding symbols, and writes
  the _prologues, _prefixes, _function_entry_points file. 
  
  Expects to be given a path to a PE file (in which the corresponding PDB can
  also be found), the filename of the input file, and an output path into which
  the results are placed.
  """
  symbols = []
  pdb = pdbparse.parse(PDB_path)
  # We now need to obtain the object map.
  try:
    sects = pdb.STREAM_SECT_HDR_ORIG.sections
    omap = pdb.STREAM_OMAP_FROM_SRC
  except AttributeError as e:
    # In this case there is no OMAP, so we use the given section
    # headers and use the identity function for omap.remap
    sects = pdb.STREAM_SECT_HDR.sections
    omap = DummyOmap()
  for symbol in pdb.STREAM_GSYM.funcs:
    symbol = pdb.STREAM_GSYM.funcs[symbol]
    # Get the RVA of the address
    function_address = omap.remap(
      symbol.offset+sects[symbol.segment-1].VirtualAddress)
    symbols.append((function_address+image_base, symbol.name))
  for symbol in pdb.STREAM_GSYM.globals:
    # Get the RVA of the address
    try:
      function_address = omap.remap(
        symbol.offset+sects[symbol.segment-1].VirtualAddress)
      symbols.append((function_address+image_base, symbol.name))
    except IndexError as e:
      print "[!] symbol.segment is %d, len(sects) is %d" % (symbol.segment, len(sects))
    except AttributeError as e:
      print "[!] KeyError ?"
      
  return symbols

symbols = ExtractPESymbols("/usr/local/google/home/thomasdullien/kimint/js32.pdb", 0x10000000)
for sym in symbols:
  print "MakeName(0x%08lx, \"%s\");" % (sym[0], sym[1])
