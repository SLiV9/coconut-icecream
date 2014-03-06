#include "idpre.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"
#include "str.h"
#include <string.h>

/*
 * INFO structure
 */
struct INFO {
  node *stack;
};

#define INFO_STACK(n) ((n)->stack)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_STACK(result) = NULL;
  
  return result;
}

static node* FreeNameDecs( node *nd)
{
	if (nd == NULL)
	{
		return NULL;
	}
	
	NAMEDECS_NEXT( nd) = FreeNameDecs( NAMEDECS_NEXT( nd));
	
	nd = MEMfree( nd);
	
	return nd;
}

static info *FreeInfo( info *info)
{
  FreeNameDecs( INFO_STACK( info));
  
  info = MEMfree( info);

  return info;
}

node* NAMELINKvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKvarcall");
  
  node* nd = INFO_STACK( arg_info);
  while (nd != NULL)
  {
  	if (STReq( NAMEDECS_NAME( nd), VARCALL_NAME( arg_node)))
  	{
  		VARCALL_DEC( arg_node) = NAMEDECS_DEC( nd);
  		VARCALL_NAME( arg_node) = NULL;
  		break;
  	}
  	else
  	{
  		nd = NAMEDECS_NEXT( nd);
  	}
  }
  if (nd == NULL)
  {
  		printf("Unknown call to variable %s.\n", VARCALL_NAME( arg_node));
  }
  
  VARCALL_INDX( arg_node) = TRAVopt( VARCALL_INDX( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKvarlet(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKvarlet");
  
  node* nd = INFO_STACK( arg_info);
  while (nd != NULL)
  {
  	if (STReq( NAMEDECS_NAME( nd), VARLET_NAME( arg_node)))
  	{
  		VARLET_DEC( arg_node) = NAMEDECS_DEC( nd);
  		VARLET_NAME( arg_node) = NULL;
  		break;
  	}
  	else
  	{
  		nd = NAMEDECS_NEXT( nd);
  	}
  }
  if (nd == NULL)
  {
  		printf("Unknown let to variable %s.\n", VARLET_NAME( arg_node));
  }
  
  VARLET_INDX( arg_node) = TRAVopt( VARLET_INDX( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKfuncall");
  
  node* nd = INFO_STACK( arg_info);
  while (nd != NULL)
  {
  	if (STReq( NAMEDECS_NAME( nd), FUNCALL_NAME( arg_node)))
  	{
  		FUNCALL_DEC( arg_node) = NAMEDECS_DEC( nd);
  		FUNCALL_NAME( arg_node) = NULL;
  		break;
  	}
  	else
  	{
  		nd = NAMEDECS_NEXT( nd);
  	}
  }
  if (nd == NULL)
  {
  		printf("Unknown call to function %s.\n", FUNCALL_NAME( arg_node));
  }
  
  FUNCALL_ARGS( arg_node) = TRAVopt( FUNCALL_ARGS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKvardec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKvardec");

	INFO_STACK( arg_info) = TBmakeNamedecs( VARDEC_NAME(arg_node), \
			arg_node, INFO_STACK( arg_info));
  
  VARDEC_EXPR( arg_node) = TRAVopt( VARDEC_EXPR( arg_node), arg_info);
  
  VARDEC_DIMDEFS( arg_node) = TRAVopt( VARDEC_DIMDEFS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKglobdef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKglobdef");

	INFO_STACK( arg_info) = TBmakeNamedecs( GLOBDEF_NAME(arg_node), \
			arg_node, INFO_STACK( arg_info));
  
  GLOBDEF_EXPR( arg_node) = TRAVopt( GLOBDEF_EXPR( arg_node), arg_info);
  
  GLOBDEF_DIMDEFS( arg_node) = TRAVopt( GLOBDEF_DIMDEFS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKglobdec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKglobdec");

	INFO_STACK( arg_info) = TBmakeNamedecs( GLOBDEC_NAME(arg_node), \
			arg_node, INFO_STACK( arg_info));
  
  GLOBDEC_DIMDECS( arg_node) = TRAVopt( GLOBDEC_DIMDECS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKparam(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKparam");

	INFO_STACK( arg_info) = TBmakeNamedecs( PARAM_NAME(arg_node), \
			arg_node, INFO_STACK( arg_info));
  
  PARAM_DIMDECS( arg_node) = TRAVopt( PARAM_DIMDECS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKiter(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKiter");

	INFO_STACK( arg_info) = TBmakeNamedecs( ITER_NAME(arg_node), \
			arg_node, INFO_STACK( arg_info));

  DBUG_RETURN (arg_node);
}

node* NAMELINKdim(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKdim");

	INFO_STACK( arg_info) = TBmakeNamedecs( DIM_NAME(arg_node), \
			arg_node, INFO_STACK( arg_info));

  DBUG_RETURN (arg_node);
}

node* NAMELINKfundef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKfundef");

	INFO_STACK( arg_info) = TBmakeNamedecs( HEADER_NAME(FUNDEF_HEAD(arg_node)), \
			arg_node, INFO_STACK( arg_info));
			
	node* old = INFO_STACK( arg_info);
  
  FUNDEF_HEAD( arg_node) = TRAVopt( FUNDEF_HEAD( arg_node), arg_info);
  FUNDEF_BODY( arg_node) = TRAVopt( FUNDEF_BODY( arg_node), arg_info);
  
  INFO_STACK( arg_info) = old;

  DBUG_RETURN (arg_node);
}

node* NAMELINKfundec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKfundec");

	INFO_STACK( arg_info) = TBmakeNamedecs( HEADER_NAME(FUNDEC_HEAD(arg_node)), \
			arg_node, INFO_STACK( arg_info));
  
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


node *NAMELINKdoLinkNames(node *syntaxtree)
{
	info *info;
	
	DBUG_ENTER("NAMELINKdoLinkNames");
	
	info = MakeInfo();

  TRAVpush( TR_namelink);

  syntaxtree = TRAVdo( syntaxtree, info);

  TRAVpop();
  
  info = FreeInfo(info);

  DBUG_RETURN( syntaxtree);
}
