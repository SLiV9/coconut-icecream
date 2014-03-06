#include <stdio.h>
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

/*
 * file handle for parsing
 */
FILE *yyin;

/*
 * external parser function from fun.y
 */
extern node *YYparseTree();

node *MYSCPdoScanParse(node *syntaxtree)
{  
	node *result = NULL;
  char *dirnm, *basenm, *filename;
  
  DBUG_ENTER("MYSCPdoScanParse");

  DBUG_ASSERT( syntaxtree == NULL, 
               "MYSCPdoScapParse() called with existing syntax tree.");
  
  if (global.cpp) {
  	dirnm = dirname( STRcpy(global.infile));
  	basenm = basename( STRcpy(global.infile));
  
    filename = STRcatn( 4,
                        dirnm,
                        "/.",
                        basenm,
                        ".cpp");
  }
  else {
    filename = STRcpy( global.infile);
  }
  
  yyin = fopen( filename, "r");
  
  if (yyin == NULL) {
    CTIabort( "Cannot open file '%s'.", filename);
  }

  MEMfree( filename);
  
  result = YYparseTree();

  DBUG_RETURN( result);
}
