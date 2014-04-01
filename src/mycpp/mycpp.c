#include <libgen.h>

#include "mycpp.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include "ctinfo.h"
#include "str.h"

node *MYCPPdoPreprocessing(node *syntaxtree)
{
	int  err;
  char *cppcallstr;
  char *dirnm, *basenm;
  
  DBUG_ENTER("MYCPPdoPreprocessing");

  char *buffer1, *buffer2;
  buffer1 = STRcpy(global.infile);
  buffer2 = STRcpy(global.infile);
	dirnm = dirname( buffer1);
	basenm = basename( buffer2);

  cppcallstr = STRcatn( 7, 
                        "cpp ",
                        global.infile,
                        " > ",
                        dirnm,
                        "/.",
                        basenm,
                        ".cpp");
  
  err = system( cppcallstr);

  cppcallstr = MEMfree( cppcallstr);

  if ( err) {
    CTIabort( "Unable to run C preprocessor");
  }
  
  global.cpp = TRUE;

  MEMfree(buffer1);
  MEMfree(buffer2);
  MEMfree(cppcallstr);

  DBUG_RETURN( syntaxtree);
}
