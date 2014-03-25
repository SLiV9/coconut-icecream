#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"
#include "str.h"
#include "ctinfo.h"
#include "myglobals.h"

#include "gettype.h"

vtype getType(node* nd)
{
	if (nd == NULL)
	{
		return VT_unknown;
	}
	
	vtype r;
	
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
			r = FUNCALL_TYPE(nd);
			if (r == VT_void) {
				CTIerror("file %s, line %d\n"
						"void returning function '%s' used in expression", \
						myglobal.fn, NODE_LINE(nd), FUNCALL_NAME(nd));
				return VT_unknown;
			};
			return r;
		case N_arraylit:
			CTIerror("file %s, line %d\n"
					"array literal used outside of array assignment", \
					myglobal.fn, NODE_LINE(nd));
			return VT_unknown;
		default:
			DBUG_ASSERT( 0, "invalid gettype detected!");
			return VT_unknown;
	}
}

int getDepth(node* arg_node)
{
	if (arg_node == NULL)
	{
		return 0;
	}

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

