#include <stdio.h>
#include <string.h>

#include "arraysplit.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include "globals.h"
#include "ctinfo.h"
#include "memory.h"
#include "print.h"
#include "copy.h"

/*
 * INFO structure
 */
struct INFO {
		node* head;
		node* last;
};

#define INFO_HEAD(n) ((n)->head)
#define INFO_LAST(n) ((n)->last)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_HEAD(result) = NULL;
  INFO_LAST(result) = NULL;
  
  return result;
}

static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}

node* ARRAYSPLITbody(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("ARRAYSPLITbody");

	if (BODY_VARDECS( arg_node) != NULL)
	{
		BODY_VARDECS( arg_node) = TRAVopt( BODY_VARDECS( arg_node), arg_info);

		node* vdecs = BODY_VARDECS( arg_node);
		while (VARDECS_NEXT( vdecs) != NULL)
		{
			vdecs = VARDECS_NEXT( vdecs);
		}
		VARDECS_NEXT( vdecs) = INFO_HEAD( arg_info);
	}
	INFO_HEAD( arg_info) = NULL;
	INFO_LAST( arg_info) = NULL;

	BODY_FUNDEFS( arg_node) = TRAVopt( BODY_FUNDEFS( arg_node), arg_info);

	DBUG_RETURN (arg_node);
}

/*node* ARRAYSPLITfor(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ARRAYSPLITfor");

  node* itr = FOR_ITER( arg_node);
	FOR_ITER( arg_node) = COPYdoCopy( itr);

  ITER_FOR( itr) = arg_node;
	node* vdecs = TBmakeVardecs( itr, NULL);

	if (INFO_HEAD( arg_info) == NULL)
	{
		INFO_HEAD( arg_info) = vdecs;
		INFO_LAST( arg_info) = vdecs;
	}	
	else
	{
		VARDECS_NEXT( INFO_LAST( arg_info)) = vdecs;
		INFO_LAST( arg_info) = vdecs;
	}

	FOR_DO( arg_node) = TRAVopt( FOR_DO( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}*/

node *ARRAYSPLITdoSplit(node *syntaxtree)
{
  info* info;

	DBUG_ENTER("ARRAYSPLITdoSplit");

	info = MakeInfo();

  TRAVpush( TR_arraysplit);

  syntaxtree = TRAVdo( syntaxtree, info);

  TRAVpop();

  info = FreeInfo(info);

  DBUG_RETURN( syntaxtree);
}