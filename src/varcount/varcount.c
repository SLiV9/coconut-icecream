#include <stdio.h>
#include <string.h>

#include "varcount.h"
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
		int local;
		int global;
		int import;
};

#define INFO_COUNT(n) ((n)->local)
#define INFO_GLOBAL(n) ((n)->global)
#define INFO_IMPORT(n) ((n)->import)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_COUNT(result) = 1;
  INFO_GLOBAL(result) = 1;
  INFO_IMPORT(result) = 1;
  
  return result;
}

static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}

node* VARCOUNTglobdef(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("VARCOUNTglobdef");

	DBUG_ASSERT( arg_info != NULL, "globdef entered without arg_info!");

	GLOBDEF_GLOBALPOS( arg_node) = INFO_GLOBAL( arg_info);
	INFO_GLOBAL( arg_info) = INFO_GLOBAL( arg_info) + 1;

	DBUG_RETURN (arg_node);
}

node* VARCOUNTfundec(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("VARCOUNTfundec");

	DBUG_ASSERT( arg_info != NULL, "fundec entered without arg_info!");

	FUNDEC_IMPORTPOS( arg_node) = INFO_IMPORT( arg_info);
	INFO_IMPORT( arg_info) = INFO_IMPORT( arg_info) + 1;

	// no need to trav

	DBUG_RETURN (arg_node);
}

node* VARCOUNTfundef(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("VARCOUNTfundef");

	info* own_info = MakeInfo();

	FUNDEF_HEAD( arg_node) = TRAVopt( FUNDEF_HEAD( arg_node), own_info);

	FUNDEF_BODY( arg_node) = TRAVdo( FUNDEF_BODY( arg_node), own_info);

	own_info = FreeInfo(own_info);

	DBUG_RETURN (arg_node);
}

node* VARCOUNTbody(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("VARCOUNTbody");

	DBUG_ASSERT( arg_info != NULL, "body entered without arg_info!");

	BODY_VARDECS( arg_node) = TRAVopt( BODY_VARDECS( arg_node), arg_info);

	BODY_FUNDEFS( arg_node) = TRAVopt( BODY_FUNDEFS( arg_node), NULL);

	DBUG_RETURN (arg_node);
}

node* VARCOUNTvardec(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("VARCOUNTvardec");

	VARDEC_SCOPEPOS( arg_node) = INFO_COUNT( arg_info);
	INFO_COUNT( arg_info) = INFO_COUNT( arg_info) + 1;

	DBUG_RETURN (arg_node);
}

node* VARCOUNTparam(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("VARCOUNTparam");

	PARAM_SCOPEPOS( arg_node) = INFO_COUNT( arg_info);
	INFO_COUNT( arg_info) = INFO_COUNT( arg_info) + 1;

	DBUG_RETURN (arg_node);
}

node* VARCOUNTiter(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("VARCOUNTiter");

	ITER_SCOPEPOS( arg_node) = INFO_COUNT( arg_info);
	INFO_COUNT( arg_info) = INFO_COUNT( arg_info) + 1;

	DBUG_RETURN (arg_node);
}

node* VARCOUNTdim(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("VARCOUNTdim");

	if (arg_info != NULL)
	{
		DIM_SCOPEPOS( arg_node) = INFO_COUNT( arg_info);
		INFO_COUNT( arg_info) = INFO_COUNT( arg_info) + 1;
	}

	DBUG_RETURN (arg_node);
}

node* VARCOUNTglobdef(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("VARCOUNTglobdef");

	// do nothing

	DBUG_RETURN (arg_node);
}

node* VARCOUNTglobdec(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("VARCOUNTglobdec");

	// do nothing

	DBUG_RETURN (arg_node);
}

node *VARCOUNTdoCount(node *syntaxtree)
{
	DBUG_ENTER("VARCOUNTdoCount");

  TRAVpush( TR_varcount);

  info* globimp = MakeInfo();

  syntaxtree = TRAVdo( syntaxtree, globimp);

  globimp = FreeInfo(globimp);

  TRAVpop();

  DBUG_RETURN( syntaxtree);
}
