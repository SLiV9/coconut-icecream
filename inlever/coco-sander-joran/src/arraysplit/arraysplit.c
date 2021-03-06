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

#include "asplit_info.h"

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

		node* dec;
  	switch (t)
  	{
  		case N_globdef:
				dec = TBmakeGlobdef( STRcpy( DIM_NAME( dim)), \
						VT_int, GLOBDEF_EXPORT( arg_node), NULL, NULL, NULL);
				GLOBDEF_DEPTH( dec) = 0;
				break;
  		case N_globdec:
				dec = TBmakeGlobdec( STRcpy( DIM_NAME( dim)), \
						VT_int, NULL);
				GLOBDEC_DEPTH( dec) = 0;
				break;
  		case N_vardec:
				dec = TBmakeVardec( STRcpy( DIM_NAME( dim)), \
						VT_int, NULL, NULL, NULL);
				VARDEC_DEPTH( dec) = 0;
				VARDEC_ESCAPING( dec) = VARDEC_ESCAPING( arg_node);
				break;
  		case N_param:
				dec = TBmakeParam( STRcpy( DIM_NAME( dim)), \
						VT_int, NULL);
				PARAM_DEPTH( dec) = 0;
				PARAM_ESCAPING( dec) = PARAM_ESCAPING( arg_node);
				break;
			default:
  			DBUG_ASSERT(0, "insertItems called on illegal nodetype!");
		}

		DIM_REPLACER( dim) = dec;

		node* itms;
		switch (t)
		{
			case N_globdef:
			case N_globdec:
		  	itms = TBmakeDeclars( dec, NULL);
		  	break;
		  case N_vardec:
		  	itms = TBmakeVardecs( dec, NULL);
		  	break;
		  case N_param:
		  	itms = TBmakeParams( dec, NULL);
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
				case N_globdef:
				case N_globdec:
					DECLARS_NEXT( INFO_LAST( arg_info)) = itms;
					break;
				case N_vardec:
					VARDECS_NEXT( INFO_LAST( arg_info)) = itms;
					break;
				case N_param:
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

  insertItems(arg_node, arg_info, GLOBDEF_DIMDECS( arg_node), N_globdef);

  DBUG_RETURN (arg_node);
}

node* ARRAYSPLITglobdec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ARRAYSPLITglobdec");

  // no need to travdo

  /* rename dimdecs */
  node* dimdecs = GLOBDEC_DIMDECS( arg_node);
  int i = 0;
  while (dimdecs != NULL)
  {
  	node* dim = DIMDECS_DIM( dimdecs);
  	MEMfree(DIM_NAME( dim));
    char* stri = STRitoa(i);
  	DIM_NAME( dim) = STRcatn(4, "", stri, "_", GLOBDEC_NAME( arg_node));
  	MEMfree(stri);

  	dimdecs = DIMDECS_NEXT( dimdecs);
  	i++;
  }

  insertItems(arg_node, arg_info, GLOBDEC_DIMDECS( arg_node), N_globdec);

  DBUG_RETURN (arg_node);
}

node* ARRAYSPLITvardec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ARRAYSPLITvardec");

  // no need to travdo

  insertItems(arg_node, arg_info, VARDEC_DIMDECS( arg_node), N_vardec);

  DBUG_RETURN (arg_node);
}

node* ARRAYSPLITparam(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ARRAYSPLITparam");

  // no need to travdo

  insertItems(arg_node, arg_info, PARAM_DIMDECS( arg_node), N_param);

  DBUG_RETURN (arg_node);
}

node *ARRAYSPLITdoSplit(node *syntaxtree)
{
  info* info;

	DBUG_ENTER("ARRAYSPLITdoSplit");

	info = MakeInfo();
  TRAVpush( TR_asplitfcall);
  syntaxtree = TRAVdo( syntaxtree, info);
  TRAVpop();
  info = FreeInfo(info);

	info = MakeInfo();
  TRAVpush( TR_arraysplit);
  syntaxtree = TRAVdo( syntaxtree, info);
  TRAVpop();
  info = FreeInfo(info);

  DBUG_RETURN( syntaxtree);
}
