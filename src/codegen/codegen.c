#include <stdio.h>
#include <string.h>

#include "codegen.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include "globals.h"
#include "ctinfo.h"
#include "memory.h"
#include "print.h"

/*
 * INFO structure
 */
struct INFO {
  FILE* fs;
};

#define INFO_FILE(n) ((n)->fs)

static info *MakeInfo(FILE* f)
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_FILE(result) = f;
  
  return result;
}

static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}

node *CODEGENfundef(node* arg_node, info* arg_info)
{
	DBUG_ENTER("CODEGENfundef");
	
	node* hd = FUNDEF_HEAD( arg_node);
	
	if (FUNDEF_EXPORT( arg_node))
	{
		fprintf(INFO_FILE(arg_info), ".export \"%s\" %s %s\n", \
				HEADER_NAME( hd), vtype_name[HEADER_TYPE( hd)], HEADER_NAME( hd));
	}
	
	DBUG_RETURN(arg_node);
}

node *CODEGENdoCodegen(node *syntaxtree)
{
  
	DBUG_ENTER("CODEGENdoCodegen");
	
	FILE* out = fopen(global.outfile, "w+");
	if (out == NULL)
	{
		CTIabort( "Cannot open file '%s'.", global.outfile);
	}
	
	info *info = MakeInfo(out);

  TRAVpush( TR_codegen);

  syntaxtree = TRAVdo( syntaxtree, info);

  TRAVpop();
  
  info = FreeInfo(info);

  DBUG_RETURN( syntaxtree);
}
