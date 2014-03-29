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

node* ARRAYSPLITdeclars(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ARRAYSPLITdeclars");

  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, \
  		"going into declars with nonempty info!");

  DECLARS_NEXT( arg_node) = TRAVopt( DECLARS_NEXT( arg_node), arg_info);

  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, \
  		"going into declar with nonempty info!");

  DECLARS_DECLAR( arg_node) = TRAVdo( DECLARS_DECLAR( arg_node), arg_info);

  node* declars = arg_node;

	if (INFO_HEAD( arg_info) != NULL)
	{
		DECLARS_NEXT( INFO_LAST( arg_info)) = declars;
		declars = INFO_HEAD( arg_info);
		INFO_HEAD( arg_info) = NULL;
		INFO_LAST( arg_info) = NULL;
	}

  DBUG_RETURN (declars);
}

static void insertDimdecs(node* arg_node, info* arg_info, node* dimdecs)
{
  while (dimdecs != NULL)
  {
  	node* dim = DIMDECS_DIM( dimdecs);
  	DIMDECS_DIM( dimdecs) = COPYdoCopy( dim);
  	DIM_DEC( dim) = arg_node;
		node* declar = TBmakeDeclars( dim, NULL);
		
		if (INFO_HEAD( arg_info) == NULL)
		{
			INFO_HEAD( arg_info) = declar;
			INFO_LAST( arg_info) = declar;
		}
		else
		{
			DECLARS_NEXT( INFO_LAST( arg_info)) = declar;
			INFO_LAST( arg_info) = declar;
		}

		dimdecs = DIMDECS_NEXT( dimdecs);
  }
}

node* ARRAYSPLITglobdef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ARRAYSPLITglobdef");

  // no need to travdo

  insertDimdecs(arg_node, arg_info, GLOBDEF_DIMDECS( arg_node));

  DBUG_RETURN (arg_node);
}

node* ARRAYSPLITglobdec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ARRAYSPLITglobdec");

  // no need to travdo

  insertDimdecs(arg_node, arg_info, GLOBDEC_DIMDECS( arg_node));

  DBUG_RETURN (arg_node);
}

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
