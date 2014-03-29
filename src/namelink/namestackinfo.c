#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "ctinfo.h"
#include "memory.h"
#include "str.h"
#include "myglobals.h"

#include "namestackinfo.h"

static int scopediff = 0;

info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_STACK(result) = NULL;
  
  INFO_DEPTH(result) = 0;
  INFO_LEVEL(result) = 0;
  
  return result;
}

info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}

void putUndeclaredError(int line, const char* name, bool isFun)
{
	if (isFun)
	{
		CTIerror("file %s, line %d\nreference to undeclared function '%s'", \
  				myglobal.fn, line, name);
  }
  else
	{
		CTIerror("file %s, line %d\nreference to undeclared variable '%s'", \
  				myglobal.fn, line, name);
  }
}

bool isFunction(node* dec)
{
	switch (NODE_TYPE(dec))
	{
		case N_fundef:
		case N_fundec:
		case N_funcall:
			return TRUE;
		default:
			return FALSE;
	}
}

node* findNameDec(node *arg_node, info *arg_info, const char* name)
{
	node* dec;
	node* nd = INFO_STACK( arg_info);
	while (nd != NULL)
	{
		dec = NAMEDECS_DEC( nd);
		
		if (STReq( NAMEDECS_NAME( nd), name) && 
				(isFunction( dec) == isFunction( arg_node)))
		{
			if (NAMEDECS_DEPTH( nd) < INFO_DEPTH( arg_info))
			{
				switch (NODE_TYPE( dec))
				{
					case N_vardec:
							VARDEC_ESCAPING( dec) = TRUE;
						break;
					case N_param:
							PARAM_ESCAPING( dec) = TRUE;
						break;
					default:
						/* do nothing */ ;
				}

				if (NAMEDECS_DEPTH( nd) > 0)
				{
					scopediff = NDSD_OUTER( INFO_DEPTH( arg_info) - NAMEDECS_DEPTH( nd));
				}
				else
				{
					scopediff = NDSD_GLOBAL();
				}
			}
			else if (NAMEDECS_DEPTH( nd) > 0)
			{
				scopediff = NDSD_LOCAL();
			}
			else
			{
				scopediff = NDSD_GLOBAL();
			}
			
			return dec;
		}
		
		nd = NAMEDECS_NEXT( nd);
	}
	
	putUndeclaredError(NODE_LINE( arg_node), name, isFunction( arg_node));
	
	return NULL;
}

int getNameDecScopeDiff()
{
		return scopediff;
}

void pushNameDec(node *arg_node, info *arg_info, const char* name)
{
	node* nd = INFO_STACK( arg_info);
	while (nd != NULL)
	{
		if (STReq( NAMEDECS_NAME( nd), name) && 
				(isFunction( NAMEDECS_DEC( nd)) == isFunction( arg_node)))
		{
			if (NAMEDECS_LEVEL( nd) >= INFO_LEVEL( arg_info))
			{
				if (isFunction( arg_node))
				{
					CTIerror("file %s, line %d\n"
							"redeclaration of function '%s'\n"
							"(first declaration at line %d)", \
							myglobal.fn, NODE_LINE( arg_node), name, NODE_LINE( nd));
				}
				else
				{
					CTIerror("file %s, line %d\n"
							"redeclaration of variable '%s'\n"
							"(first declaration at line %d)", \
							myglobal.fn, NODE_LINE( arg_node), name, NODE_LINE( nd));
				}
				return;
			}
			
			break;
		}
		
		nd = NAMEDECS_NEXT( nd);
	}
	
	INFO_STACK( arg_info) = TBmakeNamedecs( STRcpy( name), \
			INFO_DEPTH( arg_info), INFO_LEVEL( arg_info), arg_node, \
			INFO_STACK( arg_info));
}
