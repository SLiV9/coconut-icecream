#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"
#include "str.h"
#include "ctinfo.h"
#include "myglobals.h"

#include "gettype.h"

vtype getType(node* arg_node)
{
	if (arg_node == NULL)
	{
		return VT_unknown;
	}
	
	vtype r;
	
	switch (NODE_TYPE( arg_node))
	{
		case N_int:
			return VT_int;
		case N_float:
			return VT_float;
		case N_bool:
			return VT_bool;
		case N_cast:
			return CAST_TYPE( arg_node);
		case N_binop:
			return BINOP_TYPE( arg_node);
		case N_monop:
			return MONOP_TYPE( arg_node);
		case N_hoare:
			return HOARE_TYPE( arg_node);
		case N_vardec:
			return VARDEC_TYPE( arg_node);
		case N_globdef:
			return GLOBDEF_TYPE( arg_node);
		case N_globdec:
			return GLOBDEC_TYPE( arg_node);
		case N_param:
			return PARAM_TYPE( arg_node);
		case N_iter:
			return VT_int;
		case N_dim:
			return VT_int;
		case N_fundef:
			return HEADER_TYPE( FUNDEF_HEAD( arg_node));
		case N_fundec:
			return HEADER_TYPE( FUNDEC_HEAD( arg_node));
		case N_varlet:
			return VARLET_TYPE( arg_node);
		case N_varcall:
			return VARCALL_TYPE( arg_node);
		case N_funcall:
			r = FUNCALL_TYPE( arg_node);
			if (r == VT_void) {
				CTIerror("file %s, line %d\n"
						"void returning function '%s' used in expression", \
						myglobal.fn, NODE_LINE( arg_node), FUNCALL_NAME( arg_node));
				return VT_unknown;
			};
			return r;
		case N_arraylit:
			CTIerror("file %s, line %d\n"
					"array literal used outside of array assignment", \
					myglobal.fn, NODE_LINE( arg_node));
			return VT_unknown;
		default:
			DBUG_ASSERT( 0, "invalid gettype detected!");
			return VT_unknown;
	}
}

int getDepth(node* arg_node)
{
	DBUG_ASSERT( arg_node != NULL, "getDepth called on null!");

	switch (NODE_TYPE( arg_node))
	{
		case N_vardec:
			return VARDEC_DEPTH( arg_node);
		case N_globdef:
			return GLOBDEF_DEPTH( arg_node);
		case N_globdec:
			return GLOBDEC_DEPTH( arg_node);
		case N_param:
			return PARAM_DEPTH( arg_node);
		case N_iter:
		case N_dim:
			return 0;
		case N_varlet:
			return VARLET_DEPTH( arg_node);
		case N_varcall:
			return VARCALL_DEPTH( arg_node);
		case N_funcall:
			return 0;
		case N_binop:
		case N_monop:
		case N_cast:
		case N_int:
		case N_bool:
		case N_float:
			return 0;
		case N_arraylit:
			// see arraylit's getType
			return 0;
		default:
			DBUG_ASSERT( 0, "invalid getdepth type detected!");
			return VT_unknown;
	}
}

int getExprsLen(node* arg_node)
{
	int i = 0;
	if (arg_node == NULL)
	{
		return i;
	}
	
	DBUG_ASSERT( NODE_TYPE( arg_node) == N_exprs, \
			"getExprsLen called on non-Exprs!");
	node* exprs = arg_node;
	while (exprs != NULL)
	{
		i++;
		exprs = EXPRS_NEXT( exprs);
	}
	return i;
}

int getDimDecsLen(node* arg_node)
{
	int i = 0;
	if (arg_node == NULL)
	{
		return i;
	}
	
	DBUG_ASSERT( NODE_TYPE( arg_node) == N_dimdecs, \
			"getDimDecsLen called on non-Exprs!");
	node* dimdecs = arg_node;
	while (dimdecs != NULL)
	{
		i++;
		dimdecs = DIMDECS_NEXT( dimdecs);
	}
	return i;
}

