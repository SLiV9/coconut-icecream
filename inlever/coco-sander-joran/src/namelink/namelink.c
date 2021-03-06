#include <string.h>

#include "namelink.h"
#include "funhead.h"
#include "globnames.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"
#include "ctinfo.h"
#include "str.h"
#include "myglobals.h"

#include "namestackinfo.h"

node* NAMELINKvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKvarcall");
  
  VARCALL_DEC( arg_node) = findNameDec( arg_node, arg_info, \
  		VARCALL_NAME( arg_node));
  VARCALL_SCOPEDIFF( arg_node) = getNameDecScopeDiff();
  
  VARCALL_INDX( arg_node) = TRAVopt( VARCALL_INDX( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKvarlet(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKvarlet");
  
  VARLET_DEC( arg_node) = findNameDec( arg_node, arg_info, \
  		VARLET_NAME( arg_node));
  VARLET_SCOPEDIFF( arg_node) = getNameDecScopeDiff();
  
  VARLET_INDX( arg_node) = TRAVopt( VARLET_INDX( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKfuncall");
  
  FUNCALL_DEC( arg_node) = findNameDec( arg_node, arg_info, \
  		FUNCALL_NAME( arg_node));
  FUNCALL_SCOPEDIFF( arg_node) = getNameDecScopeDiff();
  
  FUNCALL_ARGS( arg_node) = TRAVopt( FUNCALL_ARGS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKvardec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKvardec");

	pushNameDec( arg_node, arg_info, VARDEC_NAME( arg_node));
  
  VARDEC_EXPR( arg_node) = TRAVopt( VARDEC_EXPR( arg_node), arg_info);
  
  VARDEC_DIMDEFS( arg_node) = TRAVopt( VARDEC_DIMDEFS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKglobdef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKglobdef");
  
  // nothing, see globnames

  DBUG_RETURN (arg_node);
}

node* NAMELINKglobdec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKglobdec");
  
  // nothing, see globnames

  DBUG_RETURN (arg_node);
}

node* NAMELINKparam(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKparam");

	pushNameDec( arg_node, arg_info, PARAM_NAME(arg_node));
  
  PARAM_DIMDECS( arg_node) = TRAVopt( PARAM_DIMDECS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* NAMELINKiter(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKiter");

	pushNameDec( arg_node, arg_info, ITER_NAME(arg_node));

  DBUG_RETURN (arg_node);
}

node* NAMELINKdim(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKdim");

	pushNameDec( arg_node, arg_info, DIM_NAME(arg_node));

  DBUG_RETURN (arg_node);
}

node* NAMELINKfundef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKfundef");
			
	node* old = INFO_STACK( arg_info);
	INFO_DEPTH( arg_info) = INFO_DEPTH( arg_info) + 1;
	INFO_LEVEL( arg_info) = INFO_LEVEL( arg_info) + 1;
  
  FUNDEF_HEAD( arg_node) = TRAVopt( FUNDEF_HEAD( arg_node), arg_info);
  FUNDEF_BODY( arg_node) = TRAVopt( FUNDEF_BODY( arg_node), arg_info);
	
	INFO_LEVEL( arg_info) = INFO_LEVEL( arg_info) - 1;
	INFO_DEPTH( arg_info) = INFO_DEPTH( arg_info) - 1;
  popUntilNameDec(arg_info, old);

  DBUG_RETURN (arg_node);
}

node* NAMELINKfundec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKfundec");
			
	node* old = INFO_STACK( arg_info);
	INFO_DEPTH( arg_info) = INFO_DEPTH( arg_info) + 1;
	INFO_LEVEL( arg_info) = INFO_LEVEL( arg_info) + 1;
  
  FUNDEC_HEAD( arg_node) = TRAVopt( FUNDEC_HEAD( arg_node), arg_info);
	
	INFO_LEVEL( arg_info) = INFO_LEVEL( arg_info) - 1;
	INFO_DEPTH( arg_info) = INFO_DEPTH( arg_info) - 1;
  popUntilNameDec(arg_info, old);

  DBUG_RETURN (arg_node);
}

node* NAMELINKfor(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKfor");

	node* old = INFO_STACK( arg_info);
	INFO_LEVEL( arg_info) = INFO_LEVEL( arg_info) + 1;
	
	FOR_FROM( arg_node) = TRAVdo( FOR_FROM( arg_node), arg_info);
	FOR_TO( arg_node) = TRAVdo( FOR_TO( arg_node), arg_info);
	FOR_INCR( arg_node) = TRAVopt( FOR_INCR( arg_node), arg_info);
	
	FOR_ITER( arg_node) = TRAVdo( FOR_ITER( arg_node), arg_info);
	
	FOR_DO( arg_node) = TRAVopt( FOR_DO( arg_node), arg_info);
	
	INFO_LEVEL( arg_info) = INFO_LEVEL( arg_info) - 1;
  popUntilNameDec(arg_info, old);

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

  TRAVpush( TR_namelinkglobnames);

  syntaxtree = TRAVdo( syntaxtree, info);

  TRAVpop();

  TRAVpush( TR_namelink);

  syntaxtree = TRAVdo( syntaxtree, info);

  TRAVpop();
  
  info = FreeInfo(info);

  DBUG_RETURN( syntaxtree);
}
