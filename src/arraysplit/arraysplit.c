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

  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, "nonempty info!");
  DECLARS_NEXT( arg_node) = TRAVopt( DECLARS_NEXT( arg_node), arg_info);
  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, "nonempty info!");

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

node* ARRAYSPLITvardecs(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ARRAYSPLITvardecs");

  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, "nonempty info!");
  VARDECS_NEXT( arg_node) = TRAVopt( VARDECS_NEXT( arg_node), arg_info);
  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, "nonempty info!");

  VARDECS_DEC( arg_node) = TRAVdo( VARDECS_DEC( arg_node), arg_info);

  node* vardecs = arg_node;

	if (INFO_HEAD( arg_info) != NULL)
	{
		VARDECS_NEXT( INFO_LAST( arg_info)) = vardecs;
		vardecs = INFO_HEAD( arg_info);
		INFO_HEAD( arg_info) = NULL;
		INFO_LAST( arg_info) = NULL;
	}

  DBUG_RETURN (vardecs);
}

node* ARRAYSPLITparams(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ARRAYSPLITparams");

  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, "nonempty info!");
  PARAMS_NEXT( arg_node) = TRAVopt( PARAMS_NEXT( arg_node), arg_info);
  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, "nonempty info!");

  PARAMS_PARAM( arg_node) = TRAVdo( PARAMS_PARAM( arg_node), arg_info);

  node* params = arg_node;

	if (INFO_HEAD( arg_info) != NULL)
	{
		PARAMS_NEXT( INFO_LAST( arg_info)) = params;
		params = INFO_HEAD( arg_info);
		INFO_HEAD( arg_info) = NULL;
		INFO_LAST( arg_info) = NULL;
	}

  DBUG_RETURN (params);
}

static void insertItems(node* arg_node, info* arg_info, node* dimdecs, \
		nodetype t)
{
  while (dimdecs != NULL)
  {
  	node* dim = DIMDECS_DIM( dimdecs);
  	DIMDECS_DIM( dimdecs) = COPYdoCopy( dim);
  	DIM_DEC( dim) = arg_node;

		node* itms;
		switch (t)
		{
			case N_declars:
		  	itms = TBmakeDeclars( dim, NULL);
		  	break;
		  case N_vardecs:
		  	itms = TBmakeVardecs( dim, NULL);
		  	break;
		  case N_params:
		  	itms = TBmakeParams( dim, NULL);
		  	break;
			default:
  			DBUG_ASSERT(0, "insertItems called on illegal nodetype!");
		}
		
		if (INFO_HEAD( arg_info) == NULL)
		{
			INFO_HEAD( arg_info) = itms;
			INFO_LAST( arg_info) = itms;
		}
		else
		{
			switch (t)
			{
				case N_declars:
					DECLARS_NEXT( INFO_LAST( arg_info)) = itms;
					break;
				case N_vardecs:
					VARDECS_NEXT( INFO_LAST( arg_info)) = itms;
					break;
				case N_params:
					PARAMS_NEXT( INFO_LAST( arg_info)) = itms;
					break;
				default:
	  			DBUG_ASSERT(0, "insertItems called on illegal nodetype!");
			}
			INFO_LAST( arg_info) = itms;
		}

		dimdecs = DIMDECS_NEXT( dimdecs);
  }
}

node* ARRAYSPLITglobdef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ARRAYSPLITglobdef");

  // no need to travdo

  insertItems(arg_node, arg_info, GLOBDEF_DIMDECS( arg_node), N_declars);

  DBUG_RETURN (arg_node);
}

node* ARRAYSPLITglobdec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ARRAYSPLITglobdec");

  // no need to travdo

  insertItems(arg_node, arg_info, GLOBDEC_DIMDECS( arg_node), N_declars);

  DBUG_RETURN (arg_node);
}

node* ARRAYSPLITvardec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ARRAYSPLITvardec");

  // no need to travdo

  insertItems(arg_node, arg_info, VARDEC_DIMDECS( arg_node), N_vardecs);

  DBUG_RETURN (arg_node);
}

node* ARRAYSPLITparam(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ARRAYSPLITparam");

  // no need to travdo

  insertItems(arg_node, arg_info, PARAM_DIMDECS( arg_node), N_params);

  DBUG_RETURN (arg_node);
}

node* ARRAYSPLITvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ARRAYSPLITvarcall");

  // no need to travdo

  /*
   * TODO
   */

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
