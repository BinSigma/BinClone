#define NO_OBSOLETE_FUNCS


#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <kernwin.hpp>


#define BUFLEN 1024

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
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
    //GetModuleFileName( NULL, buffer, MAX_PATH );
    //std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
	GetTempPath(MAX_PATH,buffer);
	return std::string(buffer);

    //return std::string( buffer ).substr( 0, pos);
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

int idaapi init(void)
{
  return PLUGIN_OK;
}

void idaapi run(int)
{
  //warning("Hello, world!");
  //system("C:\\CloneD\\CloneDetectorGUI.exe");
  char cs[MAXSTR] = "C:/CloneDetectorGUI.exe";  
  char buf[MAXSTR];
  qsnprintf(buf, sizeof(buf), form, help);
  if ( AskUsingForm_c(buf, cs) == 0 )
  {
	  msg("User selected Cancel");
	  return;
  }

  msg("Clone Searcher: %s\n",cs);

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );

   std::string fileNameStr = GetCurrentPath();
   fileNameStr += "tmpFrag.txt";
   msg("Current Path: %s\n",fileNameStr.c_str());

   FILE *tmpFile;
   tmpFile = fopen(fileNameStr.c_str(),"w+");
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
           //int strlen = 0;
		   full_ptr = &full_buf[0]; 
		   tag_remove(itr.line, itr.line, BUFLEN);
		   qstrncpy(full_ptr, &itr.line[8], BUFLEN);
		   //full_ptr[strlen(full_ptr)] = '\r';
		   //full_ptr[strlen(full_ptr)] = '\n';
	       msg("%s : string len: %d\n",full_ptr,strlen(full_ptr));
		   fwrite(full_ptr,sizeof(char), strlen(full_ptr),tmpFile);
		   fwrite("\n",sizeof(char),1,tmpFile);
	   }
	   fclose(tmpFile);
   }
   
   std::string command(cs); //"Z:\\CloneSearch\\CloneSearcher\\Debug\\CloneDetectorGUI.exe");
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
        warning( "CreateProcess failed (%d).\n", GetLastError() );
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
	
	
}

plugin_t PLUGIN =
{
  IDP_INTERFACE_VERSION,
  PLUGIN_UNL,           // plugin flags
  init,                 // initialize
  NULL,                 // terminate. this pointer may be NULL.
  run,                  // invoke plugin
  NULL,                 // long comment about the plugin
  NULL,                 // multiline help about the plugin
  "Clone Searcher",       // the preferred short name of the plugin
  NULL                  // the preferred hotkey to run the plugin
};
