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
		int count;
};

#define INFO_COUNT(n) ((n)->count)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_COUNT(result) = 1;
  
  return result;
}

static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}

node* VARCOUNTfundec(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("VARCOUNTfundec");

	// do nothing

	DBUG_RETURN (arg_node);
}

node* VARCOUNTfundef(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("VARCOUNTfundef");

	DBUG_ASSERT( arg_info == NULL, "fundef entered with running arg_info!");

	arg_info = MakeInfo();

	FUNDEF_HEAD( arg_node) = TRAVopt( FUNDEF_HEAD( arg_node), arg_info);

	FUNDEF_BODY( arg_node) = TRAVdo( FUNDEF_BODY( arg_node), arg_info);

	arg_info = FreeInfo(arg_info);

	DBUG_ASSERT( arg_info == NULL, "fundef left with running arg_info!");

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

	DIM_SCOPEPOS( arg_node) = INFO_COUNT( arg_info);
	INFO_COUNT( arg_info) = INFO_COUNT( arg_info) + 1;

	DBUG_RETURN (arg_node);
}

node *VARCOUNTdoCount(node *syntaxtree)
{
	DBUG_ENTER("VARCOUNTdoCount");

  TRAVpush( TR_varcount);

  syntaxtree = TRAVdo( syntaxtree, NULL);

  TRAVpop();

  DBUG_RETURN( syntaxtree);
}
