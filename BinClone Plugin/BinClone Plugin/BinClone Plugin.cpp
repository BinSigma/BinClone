//******************************************************************************//
// Copyright 2014 Concordia University											//
//																				//		
// Licensed under the Apache License, Version 2.0 (the "License");				//
// you may not use this file except in compliance with the License.				//
// You may obtain a copy of the License at										//
//																				//
//    http://www.apache.org/licenses/LICENSE-2.0								//
//																				//
// Unless required by applicable law or agreed to in writing, software			//
// distributed under the License is distributed on an "AS IS" BASIS,			//
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.		//
// See the License for the specific language governing permissions and			//
// limitations under the License.												//
//******************************************************************************//

#ifdef USE_DANGEROUS_FUNCTIONS
#undef USE_DANGEROUS_FUNCTIONS
#endif

#ifdef USE_STANDARD_FILE_FUNCTIONS
#undef USE_STANDARD_FILE_FUNCTIONS
#endif

#define USE_STANDARD_FILE_FUNCTIONS

#define NO_OBSOLETE_FUNCS

#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <kernwin.hpp>


#define BUFLEN 1024

#include <windows.h>
#include <string>

static char help[] =
        "This plugin will launch the Clone Searcher to search for \n"
		"clones for the current highlighted function fragment. \n"
        "\n"
        "\n"
        "\n"
        "Please enter the full path of the CloneDetectorGUI.exe\n";

static const char form[] =
"HELP\n"
"%s\n"
"ENDHELP\n"
"Enter the Clone Searcher executable\n"
"\n"
"  <~C~loneSearcher executable:f:0:50:::>\n"
"\n"
"\n";

std::string GetCurrentPath() {
    char buffer[MAX_PATH];
	GetTempPath(MAX_PATH,buffer);
	return std::string(buffer);
}

char *get_asm_text (ea_t sEA) {

    flags_t sFlags;                                 // Flags of EA
    char isn_buf[BUFLEN];                           // Instruction ASM text buffer
    char *isn_ptr;                                  // Instruction ASM text pointer
    isn_ptr = &isn_buf[0];

    sFlags = getFlags(sEA);                         // Get EA Flags
    if(isCode(sFlags)) {                            // Test ea is INS start.
        generate_disasm_line(sEA, isn_ptr,BUFLEN);
        tag_remove(isn_ptr, isn_ptr, BUFLEN);       // remove color code tags
        return isn_ptr;
    } else {
        // Do error
        return "";      // error -the EA is not the start of an isntruction.
    }

}

bool getCloneSearcherExeFromUser( std::string & p_cs)
{
	char buf[MAXSTR];
	char cs[MAXSTR] = "C:/CloneDetectorGUI.exe";
	qsnprintf(buf, sizeof(buf), form, help);
	p_cs = "";
	if ( AskUsingForm_c(buf, cs) == 0 )
	{
		msg("User selected Cancel");
		return false;
	}
	p_cs = cs;
	return true;
}

bool writeCloneSearcherExeToFile( std::string & p_cs)
{
	std::string tmpExeFile = GetCurrentPath();
	tmpExeFile += "cstmp.txt";
	FILE *exeFileH;
	exeFileH = qfopen(tmpExeFile.c_str(),"w");
	if( exeFileH == NULL)
    {
		warning( "Failed to open temp CS file %s (%d).\n", tmpExeFile.c_str(), GetLastError() );
		return false;
	}
	qfseek(exeFileH,0,0);
	qfwrite(exeFileH,p_cs.c_str(),p_cs.size());
	qfclose(exeFileH);
	return true;

}

bool getCloneSearcherExeFromFile( std::string & p_cs)
{
  p_cs = "";
  std::string tmpExeFile = GetCurrentPath();
  tmpExeFile += "cstmp.txt";
  FILE *exeFileH;
  exeFileH = qfopen(tmpExeFile.c_str(),"r");   
  if( exeFileH == NULL)
  {
 	  msg( "Failed to open temp CS file %s (%d).\n", tmpExeFile.c_str(), GetLastError() );
	  return false;
  }
  char csBuf[MAXSTR] = {0};
  int numBytes = qfread(exeFileH,csBuf,MAXSTR);
  p_cs = csBuf;
  qfclose(exeFileH);
  return true;
}

int idaapi init(void)
{
  return PLUGIN_OK;
}


void idaapi run(int)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );

   std::string fileNameStr = GetCurrentPath();
   fileNameStr += "tmpFrag.txt";
   msg("Current Path: %s\n",fileNameStr.c_str());

   FILE *tmpFile;
   tmpFile = qfopen(fileNameStr.c_str(),"w+");
   
   if( tmpFile == NULL)
   {
	    warning( "Failed to open temp file: %s (%d).\n", fileNameStr.c_str(), GetLastError() );
		return;
   }

   func_t *f = get_func(get_screen_ea());// get_next_func(0);
   if( f)
   {
	   text_t text;
	   ea_t startEA = f->startEA;
	   ea_t endEA = f->endEA; 
	   gen_disasm_text(startEA,endEA,text,false);
	   size_t numlines = text.size();
	   msg("number of line in the function: %d\n",numlines);
       
	   for( int i = 0; i < numlines; ++i)
	   {
		   auto itr = text[i];
		   char full_buf[BUFLEN];                      // Full ASM text buffer
           char *full_ptr;                             // Full ASM text pointer
		   full_ptr = &full_buf[0]; 
		   tag_remove(itr.line, itr.line, BUFLEN);
		   qstrncpy(full_ptr, &itr.line[8], BUFLEN);
	       msg("%s : string len: %d\n",full_ptr,strlen(full_ptr));
		   qfwrite(tmpFile, full_ptr, strlen(full_ptr));
		   qfwrite(tmpFile,"\n",1);
	   }
	   qfclose(tmpFile);
   }
   
   std::string csPath(""); //"Z:\\CloneSearch\\CloneSearcher\\Debug\\CloneDetectorGUI.exe");
   if( !getCloneSearcherExeFromFile(csPath))
   {
	   if( !getCloneSearcherExeFromUser(csPath))
	   {
			return;
	   }
   }

   bool done=false;
   while( !done)
   {
	   std::string command(csPath);
	   command += " -f ";
	   command += fileNameStr;
	   if( !CreateProcess( NULL,   // No module name (use command line)
			   (char *) command.c_str(), // "Z:\\CloneSearch\\CloneSearcher\\Debug\\CloneDetectorGUI.exe -f c:\\IDA\\tmp.txt\r\nabckde\r\n one two three\r\n",        // Command line
				//"-f c:\\IDA\\tmp.txt",
				NULL,           // Process handle not inheritable
				NULL,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				0,              // No creation flags
				NULL,           // Use parent's environment block
				NULL,           // Use parent's starting directory 
				&si,            // Pointer to STARTUPINFO structure
				&pi )           // Pointer to PROCESS_INFORMATION structure
		) 
		{
			int err = GetLastError();
			if( err == ERROR_FILE_NOT_FOUND)
			{
			    warning( "CloneDetectorGUI.exe could not be located.\n");
				if( getCloneSearcherExeFromUser(csPath))
					continue;
				else
					break;
			}
			else
			{
				warning( "Failed to launch CloneSearcher.exe, error code = %d\n");
			}
			return;
		}
		writeCloneSearcherExeToFile(csPath);
		done = true;
		// Wait until child process exits.
		WaitForSingleObject( pi.hProcess, INFINITE );

		// Close process and thread handles. 
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
   }
}

char wanted_hotkey[] = "Alt-F9";

plugin_t PLUGIN =
{
  IDP_INTERFACE_VERSION,
  PLUGIN_UNL,           // plugin flags
  init,                 // initialize
  NULL,                 // terminate. this pointer may be NULL.
  run,                  // invoke plugin
  NULL,                 // long comment about the plugin
  NULL,                 // multiline help about the plugin
  "BinClone",           // the preferred short name of the plugin
  wanted_hotkey         // the preferred hotkey to run the plugin
};
