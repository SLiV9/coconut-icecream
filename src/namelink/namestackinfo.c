#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "ctinfo.h"
#include "memory.h"
#include "str.h"
#include "myglobals.h"

#include "namestackinfo.h"

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

void putUndeclaredError(int line, const char* name)
{
	CTIerror("file %s, line %d\nundeclared identifier '%s'", \
  				myglobal.fn, line, name);
}

node* findNameDec(node *arg_node, info *arg_info, const char* name)
{
	node* dec;
	node* nd = INFO_STACK( arg_info);
	while (nd != NULL)
	{
		if (STReq( NAMEDECS_NAME( nd), name))
		{
			dec = NAMEDECS_DEC( nd);
			
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
			}
			
			return dec;
		}
		
		nd = NAMEDECS_NEXT( nd);
	}
	
	putUndeclaredError(NODE_LINE( arg_node), name);
	
	return NULL;
}

void pushNameDec(node *arg_node, info *arg_info, const char* name)
{
	node* nd = INFO_STACK( arg_info);
	while (nd != NULL)
	{
		if (STReq( NAMEDECS_NAME( nd), name))
		{
			if (NAMEDECS_LEVEL( nd) >= INFO_LEVEL( arg_info))
			{
				CTIerror("file %s, line %d\n"
						"identifier '%s' already declared in this scope\n"
						"(first declaration at line %d)", \
						myglobal.fn, NODE_LINE( arg_node), name, NODE_LINE( nd));
				
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
