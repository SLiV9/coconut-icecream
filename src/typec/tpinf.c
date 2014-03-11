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
  
  VARLET_TYPE( arg_node) = getType( VARLET_DEC( arg_node));

  DBUG_RETURN (arg_node);
}

node* TPINFvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFvarcall");
  
  VARCALL_INDX( arg_node) = TRAVopt( VARCALL_INDX( arg_node), arg_info);
  
  VARCALL_TYPE( arg_node) = getType( VARCALL_DEC( arg_node));

  DBUG_RETURN (arg_node);
}

node* TPINFfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFfuncall");
  
  FUNCALL_ARGS( arg_node) = TRAVopt( FUNCALL_ARGS( arg_node), arg_info);
  
  FUNCALL_TYPE( arg_node) = getType( FUNCALL_DEC( arg_node));

  DBUG_RETURN (arg_node);
}

node* TPINFmonop(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFmonop");
  
  MONOP_EXPR( arg_node) = TRAVdo( MONOP_EXPR( arg_node), arg_info);
  
  vtype t = getType( MONOP_EXPR( arg_node));
  
  switch (MONOP_OP( arg_node))
  {
  	case MO_neg:
  		switch (t)
  		{
  			case VT_int:
  			case VT_float:
  			case VT_bool:
  				MONOP_TYPE( arg_node) = t;
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
  				CTIerror("file %s, line %d\n"
							"operator %s called on expression of type %s", \
							myglobal.fn, NODE_LINE( arg_node), \
							monop_name[MONOP_OP( arg_node)], vtype_name[t]);
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

node* TPINFbinop(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFbinop");
  
  BINOP_LEFT( arg_node) = TRAVdo( BINOP_LEFT( arg_node), arg_info);
  BINOP_RIGHT( arg_node) = TRAVdo( BINOP_RIGHT( arg_node), arg_info);
  
  vtype t1 = getType( BINOP_LEFT( arg_node));
  vtype t2 = getType( BINOP_RIGHT( arg_node));
  
  if (t1 == t2)
	{
		BINOP_TYPE( arg_node) = t1;
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
