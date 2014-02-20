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
  
  DBUG_ENTER("MYCPPdoPreprocessing");

  cppcallstr = STRcatn( 5, 
                        "cpp ",
                        global.infile,
                        " > ",
                        global.infile,
                        ".cpp");
  
  err = system( cppcallstr);

  cppcallstr = MEMfree( cppcallstr);

  if ( err) {
    CTIabort( "Unable to run C preprocessor");
  }
  
  global.cpp = TRUE;

  DBUG_RETURN( syntaxtree);
}
