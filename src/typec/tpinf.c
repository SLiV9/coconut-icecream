#include <string.h>

#include "tpinf.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"
#include "str.h"

vtype getType(node* nd)
{
	if (nd == NULL)
	{
		return VT_unknown;
	}
	
	switch (NODE_TYPE(nd))
	{
		case N_int:
			return VT_int;
		case N_float:
			return VT_float;
		case N_bool:
			return VT_bool;
		case N_cast:
			return CAST_TYPE(nd);
		case N_binop:
			return BINOP_TYPE(nd);
		case N_monop:
			return MONOP_TYPE(nd);
		case N_vardec:
			return VARDEC_TYPE(nd);
		case N_globdef:
			return GLOBDEF_TYPE(nd);
		case N_globdec:
			return GLOBDEC_TYPE(nd);
		case N_param:
			return PARAM_TYPE(nd);
		case N_iter:
			return VT_int;
		case N_dim:
			return VT_int;
		case N_fundef:
			return HEADER_TYPE( FUNDEF_HEAD( nd));
		case N_fundec:
			return HEADER_TYPE( FUNDEC_HEAD( nd));
		case N_varlet:
			return VARLET_TYPE(nd);
		case N_varcall:
			return VARCALL_TYPE(nd);
		case N_funcall:
			return FUNCALL_TYPE(nd);
		default:
			return VT_unknown;
	}
}



node* TPINFvarlet(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFvarlet");
  
  VARLET_INDX( arg_node) = TRAVdo( VARLET_INDX( arg_node), arg_info);
  
  VARLET_TYPE( arg_node) = getType( VARLET_DEC( arg_node));

  DBUG_RETURN (arg_node);
}

node* TPINFvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFvarcall");
  
  VARCALL_INDX( arg_node) = TRAVdo( VARCALL_INDX( arg_node), arg_info);
  
  VARCALL_TYPE( arg_node) = getType( VARCALL_DEC( arg_node));

  DBUG_RETURN (arg_node);
}

node* TPINFfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFfuncall");
  
  FUNCALL_ARGS( arg_node) = TRAVdo( FUNCALL_ARGS( arg_node), arg_info);
  
  FUNCALL_TYPE( arg_node) = getType( FUNCALL_DEC( arg_node));

  DBUG_RETURN (arg_node);
}

node* TPINFmonop(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFmonop");

  DBUG_RETURN (arg_node);
}

node* TPINFbinop(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINFbinop");

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
