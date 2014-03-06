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

	dirnm = dirname( STRcpy(global.infile));
	basenm = basename( STRcpy(global.infile));

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

  DBUG_RETURN( syntaxtree);
}
