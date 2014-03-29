#include <string.h>

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


node* TPINFvarlet(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFvarlet");
  
  VARLET_INDX( arg_node) = TRAVopt( VARLET_INDX( arg_node), arg_info);

  DBUG_ASSERT( VARLET_DEC( arg_node) != NULL, "varlet dec is null!");
  
  VARLET_TYPE( arg_node) = getType( VARLET_DEC( arg_node));
  if (VARLET_INDX( arg_node) != NULL)
    VARLET_DEPTH( arg_node) = 0;
  else
    VARLET_DEPTH( arg_node) = getDepth( VARLET_DEC( arg_node));

  DBUG_RETURN (arg_node);
}

node* TPINFvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFvarcall");
  
  VARCALL_INDX( arg_node) = TRAVopt( VARCALL_INDX( arg_node), arg_info);
  
  node* dec = VARCALL_DEC( arg_node);

  DBUG_ASSERT( dec != NULL, "varcall dec is null!");

  if (NODE_TYPE( dec) == N_globdec)
  {
    if (VARCALL_INDX( arg_node) == NULL && GLOBDEC_DIMDECS( dec) != NULL)
    {
      CTIerror("file %s, line %d\n"
          "extern array variable '%s' used without dereference", \
          myglobal.fn, NODE_LINE( arg_node), \
          GLOBDEC_NAME( dec));
    }
  }

  VARCALL_TYPE( arg_node) = getType( dec);
  if (VARCALL_INDX( arg_node) != NULL)
    VARCALL_DEPTH( arg_node) = 0;
  else
    VARCALL_DEPTH( arg_node) = getDepth( dec);

  DBUG_RETURN (arg_node);
}

node* TPINFfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFfuncall");
  
  FUNCALL_ARGS( arg_node) = TRAVopt( FUNCALL_ARGS( arg_node), arg_info);
  
  FUNCALL_TYPE( arg_node) = getType( FUNCALL_DEC( arg_node));

  DBUG_RETURN (arg_node);
}

node* TPINFarraylit(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFarraylit");
  
  ARRAYLIT_EXPRS( arg_node) = TRAVdo( ARRAYLIT_EXPRS( arg_node), arg_info);
  
  node* exprs = ARRAYLIT_EXPRS( arg_node);
  vtype t, t1;
  int d = -1, d1;
  bool valid = TRUE;
  
  do
  {
  	node* expr = EXPRS_EXPR( exprs);
  	if (NODE_TYPE( expr) == N_arraylit)
		{
			t1 = ARRAYLIT_TYPE( expr);
			d1 = ARRAYLIT_DEPTH( expr);
		}
		else
		{
			t1 = getType( expr);
      checkDereferenced( expr);
			d1 = 0;
		}
  	
  	if (d < 0)
  	{
  		t = t1;
  		d = d1;
  	}
  	else if (d1 != d)
  	{
  		if (d1 >= 0)
  		{
				CTIerror("file %s, line %d\n"
						"invalid array literal because of dimension mismatch", \
						myglobal.fn, NODE_LINE( arg_node));
			}
  		valid = FALSE;
  	}
  	else if (t1 != t)
  	{
  		if (t1 >= 0)
  		{
				CTIerror("file %s, line %d\n"
						"invalid array literal because of type mismatch", \
						myglobal.fn, NODE_LINE( arg_node));
			}
  		valid = FALSE;
  	}
  	
  	exprs = EXPRS_NEXT( exprs);
  }
  while (valid && exprs != NULL);
  
  if (valid)
  {
  	ARRAYLIT_TYPE( arg_node) = t;
  	ARRAYLIT_DEPTH( arg_node) = d + 1;
  }
  else
  {
  	ARRAYLIT_TYPE( arg_node) = VT_unknown;
  	ARRAYLIT_DEPTH( arg_node) = -1;
  }

  DBUG_RETURN (arg_node);
}

node* TPINFvardec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFvardec");
  
  VARDEC_DIMDEFS( arg_node) = TRAVopt( VARDEC_DIMDEFS( arg_node), arg_info);
  VARDEC_EXPR( arg_node) = TRAVopt( VARDEC_EXPR( arg_node), arg_info);
  
  VARDEC_DEPTH( arg_node) = getDimDecsLen( VARDEC_DIMDECS( arg_node));

  DBUG_RETURN (arg_node);
}

node* TPINFglobdef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFglobdef");
  
  GLOBDEF_DIMDEFS( arg_node) = TRAVopt( GLOBDEF_DIMDEFS( arg_node), arg_info);
  GLOBDEF_EXPR( arg_node) = TRAVopt( GLOBDEF_EXPR( arg_node), arg_info);
  
  GLOBDEF_DEPTH( arg_node) = getDimDecsLen( GLOBDEF_DIMDECS( arg_node));

  DBUG_RETURN (arg_node);
}

node* TPINFglobdec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFglobdec");
  
  GLOBDEC_DIMDECS( arg_node) = TRAVopt( GLOBDEC_DIMDECS( arg_node), arg_info);
  
  GLOBDEC_DEPTH( arg_node) = getDimDecsLen( GLOBDEC_DIMDECS( arg_node));

  DBUG_RETURN (arg_node);
}

node* TPINFparam(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFparam");
  
  PARAM_DIMDECS( arg_node) = TRAVopt( PARAM_DIMDECS( arg_node), arg_info);
  
  PARAM_DEPTH( arg_node) = getDimDecsLen( PARAM_DIMDECS( arg_node));

  DBUG_RETURN (arg_node);
}

bool checkDereferenced(node* x)
{
  if (x == NULL)
    return TRUE;
  
  if (getDepth(x) > 0)
  {
    CTIerror("file %s, line %d\n"
        "array used in expression without dereferencing", \
        myglobal.fn, NODE_LINE( x));
    return FALSE;
  }

  return TRUE;
}

node* TPINFcast(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFcast");

  CAST_EXPR( arg_node) = TRAVdo( CAST_EXPR( arg_node), arg_info);
  checkDereferenced( CAST_EXPR( arg_node));

  DBUG_RETURN (arg_node);
}

static void putMonopError(node* arg_node, vtype t)
{
	CTIerror("file %s, line %d\n"
			"operator %s called on expression of type %s", \
			myglobal.fn, NODE_LINE( arg_node), \
			monop_name[MONOP_OP( arg_node)], vtype_name[t]);
}

node* TPINFmonop(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFmonop");
  
  MONOP_EXPR( arg_node) = TRAVdo( MONOP_EXPR( arg_node), arg_info);
  
  vtype t = getType( MONOP_EXPR( arg_node));
  checkDereferenced( MONOP_EXPR( arg_node));
  
  switch (MONOP_OP( arg_node))
  {
  	case MO_neg:
  		switch (t)
  		{
  			case VT_int:
  			case VT_float:
  				MONOP_TYPE( arg_node) = t;
  				break;
  			case VT_bool:
  				putMonopError( arg_node, t);
  				break;
  			case VT_unknown:
  				MONOP_TYPE( arg_node) = VT_unknown;
  				break;
  			default:
  				DBUG_ASSERT( 0, "invalid monop expr type detected!");
  		}
  		break;
  	case MO_not:
  		switch (t)
  		{
  			case VT_bool:
  				MONOP_TYPE( arg_node) = t;
  				break;
  			case VT_int:
  			case VT_float:
  				putMonopError( arg_node, t);
  				MONOP_TYPE( arg_node) = VT_unknown;
					break;
  			case VT_unknown:
  				MONOP_TYPE( arg_node) = VT_unknown;
  				break;
  			default:
  				DBUG_ASSERT( 0, "invalid monop expr type detected!");
  		}
  		break;
  	default:
  		DBUG_ASSERT( 0, "invalid monop op detected!");
  }

  DBUG_RETURN (arg_node);
}

static void putBinopError(node* arg_node, vtype t1, vtype t2)
{
	CTIerror("file %s, line %d\n"
			"operator %s called on expressions of types %s and %s", \
			myglobal.fn, NODE_LINE( arg_node), \
			binop_name[BINOP_OP( arg_node)], vtype_name[t1], vtype_name[t2]);
}

node* TPINFbinop(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFbinop");
  
  BINOP_LEFT( arg_node) = TRAVdo( BINOP_LEFT( arg_node), arg_info);
  BINOP_RIGHT( arg_node) = TRAVdo( BINOP_RIGHT( arg_node), arg_info);
  
  vtype t1 = getType( BINOP_LEFT( arg_node));
  vtype t2 = getType( BINOP_RIGHT( arg_node));
  checkDereferenced( BINOP_LEFT( arg_node));
  checkDereferenced( BINOP_RIGHT( arg_node));
  
  if (t1 != VT_unknown && t2 != VT_unknown)
	{
		switch (BINOP_OP( arg_node))
		{
			case BO_add:
			case BO_mul:
				if ((t1 == VT_int 	&& t2 == VT_int) ||
						(t1 == VT_float	&& t2 == VT_float) ||
						(t1 == VT_bool	&& t2 == VT_bool))
				{
					BINOP_TYPE( arg_node) = t1;
				}
				else
				{
					putBinopError( arg_node, t1, t2);
					BINOP_TYPE( arg_node) = VT_unknown;
				}
				break;
			case BO_sub:
			case BO_div:
				if ((t1 == VT_int 	&& t2 == VT_int) ||
						(t1 == VT_float	&& t2 == VT_float))
				{
					BINOP_TYPE( arg_node) = t1;
				}
				else
				{
					putBinopError( arg_node, t1, t2);
					BINOP_TYPE( arg_node) = VT_unknown;
				}
				break;
			case BO_mod:
				if ((t1 == VT_int 	&& t2 == VT_int))
				{
					BINOP_TYPE( arg_node) = t1;
				}
				else
				{
					putBinopError( arg_node, t1, t2);
					BINOP_TYPE( arg_node) = VT_unknown;
				}
				break;
			case BO_eq:
			case BO_ne:
				if ((t1 == VT_int 	&& t2 == VT_int) ||
						(t1 == VT_float	&& t2 == VT_float) ||
						(t1 == VT_bool	&& t2 == VT_bool))
				{
					BINOP_TYPE( arg_node) = VT_bool;
				}
				else
				{
					putBinopError( arg_node, t1, t2);
					BINOP_TYPE( arg_node) = VT_unknown;
				}
				break;
			case BO_lt:
			case BO_le:
			case BO_gt:
			case BO_ge:
				if ((t1 == VT_int 	&& t2 == VT_int) ||
						(t1 == VT_float	&& t2 == VT_float))
				{
					BINOP_TYPE( arg_node) = VT_bool;
				}
				else
				{
					putBinopError( arg_node, t1, t2);
					BINOP_TYPE( arg_node) = VT_unknown;
				}
				break;
			case BO_and:
			case BO_or:
				if ((t1 == VT_bool 	&& t2 == VT_bool))
				{
					BINOP_TYPE( arg_node) = t1;
				}
				else
				{
					putBinopError( arg_node, t1, t2);
					BINOP_TYPE( arg_node) = VT_unknown;
				}
				break;
			default:
  			DBUG_ASSERT( 0, "invalid binop op detected!");
		}
  }
  else
  {
		BINOP_TYPE( arg_node) = VT_unknown;
  }

  DBUG_RETURN (arg_node);
}

node *TPINFdoInference(node *syntaxtree)
{	
	DBUG_ENTER("TPINFdoInference");

  TRAVpush( TR_tpinf);

  syntaxtree = TRAVdo( syntaxtree, NULL);

  TRAVpop();

  DBUG_RETURN( syntaxtree);
}
