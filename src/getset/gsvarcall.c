#include <string.h>

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "ctinfo.h"
#include "memory.h"
#include "str.h"
#include "myglobals.h"

#include "gsvarcall.h"

node* GSVARCALLassign(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("GSVARCALLassign");

  ASSIGN_EXPR( arg_node) = TRAVdo( ASSIGN_EXPR( arg_node), arg_info);

  node* dec = VARLET_DEC( ASSIGN_LET( arg_node));
  if (NODE_TYPE( dec) != N_globdec)
  {
  	DBUG_RETURN (arg_node);
  }

  node* setter = GLOBDEC_SETTER( dec);
  DBUG_ASSERT( setter != NULL, "no setter!");
  char* settername = HEADER_NAME( FUNDEC_HEAD( setter));

  node* args = TBmakeExprs( ASSIGN_EXPR( arg_node), \
  		VARLET_INDX( ASSIGN_LET( arg_node)));

  node* fcall = TBmakeFuncall( STRcpy( settername), args);
  FUNCALL_DEC( fcall) = setter;
  FUNCALL_TYPE( fcall) = VARLET_TYPE( ASSIGN_LET( arg_node));
  FUNCALL_SCOPEDIFF( fcall) = NDSD_GLOBAL();

  node* fstate = TBmakeFunstate( fcall);

  DBUG_RETURN (fstate);
}

node* GSVARCALLvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("GSVARCALLvarcall");

  node* dec = VARCALL_DEC( arg_node);
  if (NODE_TYPE( dec) != N_globdec)
  {
  	DBUG_RETURN (arg_node);
  }

  node* getter = GLOBDEC_GETTER( dec);
  DBUG_ASSERT( getter != NULL, "no getter!");
  char* gettername = HEADER_NAME( FUNDEC_HEAD( getter));

  node* fcall = TBmakeFuncall( STRcpy( gettername), VARCALL_INDX( arg_node));
  FUNCALL_DEC( fcall) = getter;
  FUNCALL_TYPE( fcall) = VARCALL_TYPE( arg_node);
  FUNCALL_SCOPEDIFF( fcall) = NDSD_GLOBAL();

  DBUG_RETURN (fcall);
}
