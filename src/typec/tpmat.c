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
  	default:
  		DBUG_ASSERT( 0, "invalid return type detected!");
	}

  DBUG_RETURN (arg_node);
}

node *TPMATdoMatching(node *syntaxtree)
{	
	DBUG_ENTER("TPMATdoMatching");

  TRAVpush( TR_tpmat);

  syntaxtree = TRAVdo( syntaxtree, NULL);

  TRAVpop();

  DBUG_RETURN( syntaxtree);
}
