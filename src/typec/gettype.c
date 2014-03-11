#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"
#include "str.h"

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
			if (r == VT_void) { /* TODO */ };
			return r;
		default:
			DBUG_ASSERT( 0, "unknown value type detected!");
			return VT_void;
	}
}

