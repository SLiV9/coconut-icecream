#include <string.h>

#include "namelink.h"
#include "funhead.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"
#include "ctinfo.h"
#include "str.h"
#include "myglobals.h"

/*
 * INFO structure
 */
struct INFO {
  node *stack;
  int depth;
};

#define INFO_STACK(n) ((n)->stack)
#define INFO_DEPTH(n) ((n)->depth)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_STACK(result) = NULL;
  
  INFO_DEPTH(result) = 0;
  
  return result;
}

static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}

static void putUndeclaredError(int line, const char* name)
{
	CTIerror("file %s, line %d\nundeclared identifier '%s'", \
  				myglobal.fn, line, name);
}

static node* findNameDec(node *arg_node, info *arg_info, const char* name)
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

node* NAMELINKvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKvarcall");
  
  VARCALL_DEC( arg_node) = findNameDec( arg_node, arg_info, \
  		VARCALL_NAME( arg_node));
  
  VARCALL_INDX( arg_node) = TRAVopt( VARCALL_INDX( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKvarlet(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKvarlet");
  
  VARLET_DEC( arg_node) = findNameDec( arg_node, arg_info, \
  		VARLET_NAME( arg_node));
  
  VARLET_INDX( arg_node) = TRAVopt( VARLET_INDX( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKfuncall");
  
  FUNCALL_DEC( arg_node) = findNameDec( arg_node, arg_info, \
  		FUNCALL_NAME( arg_node));
  
  FUNCALL_ARGS( arg_node) = TRAVopt( FUNCALL_ARGS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKvardec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKvardec");

	INFO_STACK( arg_info) = TBmakeNamedecs( VARDEC_NAME(arg_node), \
			INFO_DEPTH( arg_info), arg_node, INFO_STACK( arg_info));
  
  VARDEC_EXPR( arg_node) = TRAVopt( VARDEC_EXPR( arg_node), arg_info);
  
  VARDEC_DIMDEFS( arg_node) = TRAVopt( VARDEC_DIMDEFS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKglobdef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKglobdef");

	INFO_STACK( arg_info) = TBmakeNamedecs( GLOBDEF_NAME(arg_node), \
			INFO_DEPTH( arg_info), arg_node, INFO_STACK( arg_info));
  
  GLOBDEF_EXPR( arg_node) = TRAVopt( GLOBDEF_EXPR( arg_node), arg_info);
  
  GLOBDEF_DIMDEFS( arg_node) = TRAVopt( GLOBDEF_DIMDEFS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKglobdec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKglobdec");

	INFO_STACK( arg_info) = TBmakeNamedecs( GLOBDEC_NAME(arg_node), \
			INFO_DEPTH( arg_info), arg_node, INFO_STACK( arg_info));
  
  GLOBDEC_DIMDECS( arg_node) = TRAVopt( GLOBDEC_DIMDECS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKparam(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKparam");

	INFO_STACK( arg_info) = TBmakeNamedecs( PARAM_NAME(arg_node), \
			INFO_DEPTH( arg_info), arg_node, INFO_STACK( arg_info));
  
  PARAM_DIMDECS( arg_node) = TRAVopt( PARAM_DIMDECS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKiter(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKiter");

	INFO_STACK( arg_info) = TBmakeNamedecs( ITER_NAME(arg_node), \
			INFO_DEPTH( arg_info), arg_node, INFO_STACK( arg_info));

  DBUG_RETURN (arg_node);
}

node* NAMELINKdim(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKdim");

	INFO_STACK( arg_info) = TBmakeNamedecs( DIM_NAME(arg_node), \
			INFO_DEPTH( arg_info), arg_node, INFO_STACK( arg_info));

  DBUG_RETURN (arg_node);
}

node* NAMELINKfundef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKfundef");

	INFO_STACK( arg_info) = TBmakeNamedecs( HEADER_NAME(FUNDEF_HEAD(arg_node)), \
			INFO_DEPTH( arg_info), arg_node, INFO_STACK( arg_info));
			
	node* old = INFO_STACK( arg_info);
	
	INFO_DEPTH( arg_info) = INFO_DEPTH( arg_info) + 1;
  
  FUNDEF_HEAD( arg_node) = TRAVopt( FUNDEF_HEAD( arg_node), arg_info);
  FUNDEF_BODY( arg_node) = TRAVopt( FUNDEF_BODY( arg_node), arg_info);
	
	INFO_DEPTH( arg_info) = INFO_DEPTH( arg_info) - 1;
  
  INFO_STACK( arg_info) = old;

  DBUG_RETURN (arg_node);
}

node* NAMELINKfundec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKfundec");

	INFO_STACK( arg_info) = TBmakeNamedecs( HEADER_NAME(FUNDEC_HEAD(arg_node)), \
			INFO_DEPTH( arg_info), arg_node, INFO_STACK( arg_info));
  
  FUNDEC_HEAD( arg_node) = TRAVopt( FUNDEC_HEAD( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKfor(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKfor");

	node* old = INFO_STACK( arg_info);
	
	FOR_FROM( arg_node) = TRAVdo( FOR_FROM( arg_node), arg_info);
	FOR_TO( arg_node) = TRAVdo( FOR_TO( arg_node), arg_info);
	FOR_INCR( arg_node) = TRAVopt( FOR_INCR( arg_node), arg_info);
	
	FOR_ITER( arg_node) = TRAVdo( FOR_ITER( arg_node), arg_info);
	
	FOR_DO( arg_node) = TRAVopt( FOR_DO( arg_node), arg_info);
	
	INFO_STACK( arg_info) = old;

  DBUG_RETURN (arg_node);
}

node* NAMELINKbody(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKbody");

  TRAVpush( TR_namelinkfunhead);

  BODY_FUNDEFS( arg_node) = TRAVopt( BODY_FUNDEFS( arg_node), arg_info);

  TRAVpop();
  
  BODY_VARDECS( arg_node) = TRAVopt( BODY_VARDECS( arg_node), arg_info);
  BODY_FUNDEFS( arg_node) = TRAVopt( BODY_FUNDEFS( arg_node), arg_info);
  BODY_INSTRS( arg_node) = TRAVopt( BODY_INSTRS( arg_node), arg_info);
  BODY_RETURN( arg_node) = TRAVopt( BODY_RETURN( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}


node *NAMELINKdoLinkNames(node *syntaxtree)
{
	info *info;
	
	DBUG_ENTER("NAMELINKdoLinkNames");
	
	info = MakeInfo();

  TRAVpush( TR_namelinkfunhead);

  syntaxtree = TRAVdo( syntaxtree, info);

  TRAVpop();

  TRAVpush( TR_namelink);

  syntaxtree = TRAVdo( syntaxtree, info);

  TRAVpop();
  
  info = FreeInfo(info);

  DBUG_RETURN( syntaxtree);
}