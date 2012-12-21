//-----------------------------------------------------
// Objective:   This program takes a folder as input,
//              parses each assembly in the folder,
//              finds the clone pairs among the files
//              and within the files, and finally
//              outputs an XML file containing all the
//              clone pairs. This program depends on
//              BFLIB.
//
// History:     2011/10/19 - B. Fung (Created)
// 
//-----------------------------------------------------

#pragma once

#include "resource.h"

void printTime();
long getTime();
long get_runtime();
