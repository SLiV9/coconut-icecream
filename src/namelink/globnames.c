#include <string.h>

#include "globnames.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "ctinfo.h"
#include "memory.h"
#include "str.h"
#include "myglobals.h"

#include "namestackinfo.h"

node* NAMELINKGLOBNAMESglobdef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKGLOBNAMESglobdef");

	pushNameDec( arg_node, arg_info, GLOBDEF_NAME(arg_node));
	
	TRAVpush( TR_namelink);
	
  GLOBDEF_EXPR( arg_node) = TRAVopt( GLOBDEF_EXPR( arg_node), arg_info);
  
  GLOBDEF_DIMDEFS( arg_node) = TRAVopt( GLOBDEF_DIMDEFS( arg_node), arg_info);
  
	TRAVpop();

  DBUG_RETURN (arg_node);
}

node* NAMELINKGLOBNAMESglobdec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKGLOBNAMESglobdec");

	pushNameDec( arg_node, arg_info, GLOBDEC_NAME(arg_node));
	
	TRAVpush( TR_namelink);
	
  GLOBDEC_DIMDECS( arg_node) = TRAVopt( GLOBDEC_DIMDECS( arg_node), arg_info);
	
	TRAVpop();

  DBUG_RETURN (arg_node);
}

node* NAMELINKGLOBNAMESfundef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKGLOBNAMESfundef");

	// do nothing, see funhead

  DBUG_RETURN (arg_node);
}

node* NAMELINKGLOBNAMESfundec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKGLOBNAMESfundec");

	// do nothing, see funhead

  DBUG_RETURN (arg_node);
}

