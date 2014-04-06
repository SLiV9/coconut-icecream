#include <stdio.h>
#include <string.h>

#include "asplitfcall.h"
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

node* ASPLITFCALLexprs(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ASPLITFCALLexprs");

  // don't mind me
  (void) MakeInfo;
  (void) FreeInfo;

  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, "nonempty info!");
  EXPRS_NEXT( arg_node) = TRAVopt( EXPRS_NEXT( arg_node), arg_info);
  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, "nonempty info!");

  EXPRS_EXPR( arg_node) = TRAVdo( EXPRS_EXPR( arg_node), arg_info);

  node* exprs = arg_node;

	if (INFO_HEAD( arg_info) != NULL)
	{
		EXPRS_NEXT( INFO_LAST( arg_info)) = exprs;
		exprs = INFO_HEAD( arg_info);
		INFO_HEAD( arg_info) = NULL;
		INFO_LAST( arg_info) = NULL;
	}

  DBUG_RETURN (exprs);
}

node* ASPLITFCALLvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ASPLITFCALLvarcall");

  // no need to travdo

  if (VARCALL_DEPTH( arg_node) > 0)
  {
	  node* dec = VARCALL_DEC( arg_node);
	  node* dimdecs;
	  switch (NODE_TYPE( dec))
	  {
	  	case N_globdef:
	  		dimdecs = GLOBDEF_DIMDECS( dec);
	  		break;
	  	case N_globdec:
	  		dimdecs = GLOBDEC_DIMDECS( dec);
	  		break;
	  	case N_vardec:
	  		dimdecs = VARDEC_DIMDECS( dec);
	  		break;
	  	case N_param:
	  		dimdecs = PARAM_DIMDECS( dec);
	  		break;
	  	case N_iter:
	  	case N_dim:
	  		dimdecs = NULL;
	  		break;
	  	default:
	  		DBUG_ASSERT(0, "illegal varcall dec detected!");
	  }

	  while (dimdecs != NULL)
	  {
	  	node* dim = DIMDECS_DIM( dimdecs);
	  	node* dcall = TBmakeVarcall( STRcpy( DIM_NAME( dim)), NULL);
	  	VARCALL_DEC( dcall) = dim;
	  	VARCALL_SCOPEDIFF( dcall) = VARCALL_SCOPEDIFF( arg_node);
	  	VARCALL_TYPE( dcall) = VT_int;

			node* itms = TBmakeExprs( dcall, NULL);
			
			if (INFO_HEAD( arg_info) == NULL)
			{
				INFO_HEAD( arg_info) = itms;
				INFO_LAST( arg_info) = itms;
			}
			else
			{
				EXPRS_NEXT( INFO_LAST( arg_info)) = itms;
				INFO_LAST( arg_info) = itms;
			}

			dimdecs = DIMDECS_NEXT( dimdecs);
	  }
	}

  DBUG_RETURN (arg_node);
}
