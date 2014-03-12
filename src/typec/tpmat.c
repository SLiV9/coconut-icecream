#include <string.h>

#include "tpmat.h"
#include "tpinf.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"
#include "str.h"
#include "ctinfo.h"
#include "myglobals.h"

#include "gettype.h"
#include "print.h"

/*
 * INFO structure
 */
struct INFO {
  node *args;
};

#define INFO_ARGS(n) ((n)->args)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_ARGS(result) = NULL;
  
  return result;
}

static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}


node* TPMATassign(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPMATassign");
  
  ASSIGN_LET( arg_node) = TRAVdo( ASSIGN_LET( arg_node), arg_info);
  ASSIGN_EXPR( arg_node) = TRAVdo( ASSIGN_EXPR( arg_node), arg_info);
  
  vtype vrt = getType( ASSIGN_LET( arg_node));
  vtype ext = getType( ASSIGN_EXPR( arg_node));
  
  switch (vrt)
  {
  	case VT_int:
  	case VT_float:
  	case VT_bool:
  		switch (ext)
	 		{
	 			case VT_int:
	 			case VT_float:
	 			case VT_bool:
	 				if (vrt != ext)
	 				{
	 					CTIerror("file %s, line %d\n"
								"cannot assign expr of type %s to variable '%s' of type %s",\
								myglobal.fn, NODE_LINE( arg_node), vtype_name[ext], \
								VARLET_NAME( ASSIGN_LET( arg_node)), vtype_name[vrt]);
	 				}
	 				break;
				case VT_unknown:
					break;
	 			default:
	 				DBUG_ASSERT( 0, "invalid expr type detected!");
			}
  		break;
		case VT_unknown:
			break;
  	default:
  		DBUG_ASSERT( 0, "invalid varlet type detected!");
  }

	DBUG_RETURN( arg_node);
}

node* TPMATfundef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPMATfundef");
  
  FUNDEF_HEAD( arg_node) = TRAVdo( FUNDEF_HEAD( arg_node), arg_info);
  FUNDEF_BODY( arg_node) = TRAVdo( FUNDEF_BODY( arg_node), arg_info);
  
  vtype hrt = getType( arg_node);
  node* ret = BODY_RETURN( FUNDEF_BODY( arg_node));
  
  switch (hrt)
  {
  	case VT_int:
  	case VT_float:
  	case VT_bool:
  		if (ret == NULL)
  		{
  			CTIerror("file %s, line %d\n"
						"missing return statement in function '%s' returning %s", \
						myglobal.fn, NODE_LINE( arg_node), \
						HEADER_NAME( FUNDEF_HEAD( arg_node)), vtype_name[hrt]);
  		}
  		else if (getType( ret) != hrt)
  		{
  			CTIerror("file %s, line %d\n"
						"return statement of type %s in function '%s' returning %s", \
						myglobal.fn, NODE_LINE( arg_node), vtype_name[getType( ret)], \
						HEADER_NAME( FUNDEF_HEAD( arg_node)), vtype_name[hrt]);
  		}
  		break;
  	case VT_void:
  		if (ret != NULL)
  		{
  			CTIerror("file %s, line %d\n"
						"return statement of type %s in function '%s' returning void", \
						myglobal.fn, NODE_LINE( arg_node), vtype_name[getType( ret)], \
						HEADER_NAME( FUNDEF_HEAD( arg_node)));
  		}
  		break;
		case VT_unknown:
			break;
  	default:
  		DBUG_ASSERT( 0, "invalid return type detected!");
	}

  DBUG_RETURN (arg_node);
}

node* TPMATfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPMATfuncall");
	
	// TODO
	
	/* 	Idee: info aanmaken en args in info->args stoppen,
			Dan TRAVdo(Header) ofzo. Ugh maar echt heel kut.
			Niet TRAVdo(Dec) want dan doe je de body van een
			fundef opnieuw. Sowieso niet echt logisch.
			Misschien gewoon Params opvragen, juist die in
			info stoppen en dan TRAVdo op args? */
  
	DBUG_RETURN( arg_node);
}

node* TPMATheader(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPMATheader");
	
	if (HEADER_PARAMS( arg_node) != NULL)
	{
		// TODO
  	HEADER_PARAMS( arg_node)	= TRAVdo( HEADER_PARAMS( arg_node), arg_info);
  }
  else
	{
		// TODO
	}
  
	DBUG_RETURN( arg_node);
}

node* TPMATparams(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPMATparams");
	
	// TODO
  
	DBUG_RETURN( arg_node);
}

node* TPMATif(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPMATif");
  
  IF_COND( arg_node)	= TRAVdo( IF_COND( arg_node), arg_info);
  IF_THEN( arg_node) 	= TRAVdo( IF_THEN( arg_node), arg_info);
  IF_ELSE( arg_node)	= TRAVopt( IF_ELSE( arg_node), arg_info);
  
  vtype cdt = getType( IF_COND( arg_node));
  
  switch (cdt)
  {
  	case VT_bool:
  		break;
  	case VT_int:
  	case VT_float:
			CTIerror("file %s, line %d\n"
					"if statement condition of type %s", \
					myglobal.fn, NODE_LINE( arg_node), vtype_name[cdt]);
			break;
		case VT_unknown:
			break;
  	default:
  		DBUG_ASSERT( 0, "invalid if cond type detected!");
  }

	DBUG_RETURN( arg_node);
}

node* TPMATwhile(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPMATwhile");
  
  WHILE_COND( arg_node)	= TRAVdo( WHILE_COND( arg_node), arg_info);
  WHILE_DO( arg_node) 	= TRAVdo( WHILE_DO( arg_node), arg_info);
  
  vtype cdt = getType( WHILE_COND( arg_node));
  
  switch (cdt)
  {
  	case VT_bool:
  		break;
  	case VT_int:
  	case VT_float:
			CTIerror("file %s, line %d\n"
					"while statement condition of type %s", \
					myglobal.fn, NODE_LINE( arg_node), vtype_name[cdt]);
			break;
		case VT_unknown:
			break;
  	default:
  		DBUG_ASSERT( 0, "invalid while cond type detected!");
  }

	DBUG_RETURN( arg_node);
}

static void checkForExpr(node* arg_node, vtype t)
{
	switch (t)
  {
  	case VT_int:
  		break;
  	case VT_float:
  	case VT_bool:
			CTIerror("file %s, line %d\n"
					"for statement expression of type %s", \
					myglobal.fn, NODE_LINE( arg_node), vtype_name[t]);
			break;
		case VT_unknown:
			break;
  	default:
  		DBUG_ASSERT( 0, "invalid for expr type detected!");
  }
}

node* TPMATfor(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPMATfor");
  
  FOR_ITER( arg_node) = TRAVdo( FOR_ITER( arg_node), arg_info);
  FOR_FROM( arg_node)	= TRAVdo( FOR_FROM( arg_node), arg_info);
  FOR_TO( arg_node) 	= TRAVdo( FOR_TO( arg_node), arg_info);
  FOR_INCR( arg_node)	= TRAVopt( FOR_INCR( arg_node), arg_info);
  
  checkForExpr( arg_node, getType( FOR_FROM( arg_node)));
  checkForExpr( arg_node, getType( FOR_TO( arg_node)));
  if (FOR_INCR( arg_node) != NULL)
  {
  	checkForExpr( arg_node, getType( FOR_INCR( arg_node)));
  }

	DBUG_RETURN( arg_node);
}

node *TPMATdoMatching(node *syntaxtree)
{	
	DBUG_ENTER("TPMATdoMatching");

  TRAVpush( TR_tpmat);

  syntaxtree = TRAVdo( syntaxtree, NULL);

  TRAVpop();

  DBUG_RETURN( syntaxtree);
}
